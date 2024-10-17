/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Utility/ShaderPreprocessor.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"

#include "Common/FileSystem/FileSystem.hpp"

namespace Lina
{
	void ShaderProperty::SaveToStream(OStream& stream) const
	{
		stream << sid;
		stream << type;
		stream << name;
		stream << static_cast<uint32>(data.size());
		stream.WriteRaw(data);
	}

	void ShaderProperty::LoadFromStream(IStream& stream)
	{
		stream >> sid;
		stream >> type;
		stream >> name;
		uint32 sz = 0;
		stream >> sz;
		data = {new uint8[static_cast<size_t>(sz)], static_cast<size_t>(sz)};
		stream.ReadToRaw(data);
	}

	bool ShaderProperty::VerifySimilarity(const Vector<ShaderProperty*>& v1, const Vector<ShaderProperty*>& v2)
	{
		if (v1.empty() || v2.empty())
			return true;

		for (ShaderProperty* p : v1)
		{
			auto it = linatl::find_if(v2.begin(), v2.end(), [p](ShaderProperty* prop) -> bool { return p->type == prop->type && p->sid == prop->sid; });
			if (it == v2.end())
				return false;
		}

		return true;
	}

	void Shader::Metadata::SaveToStream(OStream& stream) const
	{
		stream << variants;
		stream << shaderType;
	}

	void Shader::Metadata::LoadFromStream(IStream& stream)
	{
		stream >> variants;
		stream >> shaderType;
	}

	Shader::~Shader()
	{
		for (ShaderProperty* p : m_properties)
			delete p;
		DestroyHW();
	}

	void Shader::Bind(LinaGX::CommandStream* stream, uint32 gpuHandle)
	{
		LinaGX::CMDBindPipeline* bind = stream->AddCommand<LinaGX::CMDBindPipeline>();
		bind->shader				  = gpuHandle;
	}

	void Shader::AllocateDescriptorSet(DescriptorSet*& outSet, uint32& outIndex)
	{
		/*
		 Return any set that has available allocations.
		 Consumer will use the set pointer and returned allocation index to free up the allocation.
		 */
		for (auto* set : m_descriptorSets)
		{
			if (set->IsAvailable())
			{
				outSet = set;
				set->Allocate(outIndex);
				return;
			}
		}

		// Grow if needed.
		DescriptorSet* set = new DescriptorSet();
		set->Create(m_materialSetDesc);
		m_descriptorSets.push_back(set);
		outSet = set;
		set->Allocate(outIndex);
	}

	void Shader::FreeDescriptorSet(DescriptorSet* set, uint32 index)
	{
		set->Free(index);

		// Shrink if not used anymore.
		if (set->IsEmpty())
		{
			auto it = linatl::find_if(m_descriptorSets.begin(), m_descriptorSets.end(), [set](DescriptorSet* s) -> bool { return s == set; });
			LINA_ASSERT(it != m_descriptorSets.end(), "");
			set->Destroy();
			delete set;
			m_descriptorSets.erase(it);
		}
	}

	bool Shader::LoadFromFile(const String& path)
	{
		if (!FileSystem::FileOrPathExists(path))
			return false;

		const String includePath = FileSystem::GetRunningDirectory();
		if (includePath.empty())
			return false;

		const String txt = FileSystem::ReadFileContentsAsString(path);
		if (txt.empty())
			return false;

		if (!ShaderPreprocessor::VerifyFullShader(txt))
			return false;

		String vertexBlock = "", fragBlock = "";
		if (!ShaderPreprocessor::ExtractVertexFrag(txt, vertexBlock, fragBlock))
			return false;

		ShaderPreprocessor::InjectVersionAndExtensions(vertexBlock);
		ShaderPreprocessor::InjectVersionAndExtensions(fragBlock);

		Vector<ShaderProperty*> vertexProperties = {}, fragProperties = {};
		ShaderPreprocessor::InjectMaterialIfRequired(vertexBlock, vertexProperties);
		ShaderPreprocessor::InjectMaterialIfRequired(fragBlock, fragProperties);

		if (!ShaderProperty::VerifySimilarity(vertexProperties, fragProperties))
		{
			LINA_ERR("LinaMaterial structs in vertex and fragment shaders are different!");
			return false;
		}

		m_properties = fragProperties;

		const ShaderType type	 = m_meta.shaderType;
		bool			 success = true;

		auto clearVariants = [this]() {
			for (auto& [sid, variant] : m_meta.variants)
			{
				for (auto& [stage, blob] : variant._outCompiledBlobs)
				{
					if (blob.ptr != nullptr)
						delete[] blob.ptr;
				}
				variant._outCompiledBlobs.clear();
			}
		};

		if (type == ShaderType::OpaqueSurface)
		{

			ShaderPreprocessor::InjectRenderPassInputs(vertexBlock, RenderPassDescriptorType::Deferred);
			ShaderPreprocessor::InjectRenderPassInputs(fragBlock, RenderPassDescriptorType::Deferred);

			// Deferred default
			{
				ShaderVariant& variant = m_meta.variants["DeferredDefault"_hs];
				variant				   = ShaderVariant{
								   .blendDisable	= true,
								   .depthTest		= true,
								   .depthWrite		= true,
								   .targets			= {{.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}},
								   .cullMode		= LinaGX::CullMode::Back,
								   .frontFace		= LinaGX::FrontFace::CW,
								   .indirectEnabled = true,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectVertexMain(vertex, ShaderType::OpaqueSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::OpaqueSurface);

				variant._compileData[LinaGX::ShaderStage::Vertex] = {
					.includePath = includePath.c_str(),
					.text		 = vertex,
				};

				variant._compileData[LinaGX::ShaderStage::Fragment] = {
					.includePath = includePath.c_str(),
					.text		 = fragment,
				};
			}

			// Deferred skinned
			{
				ShaderVariant& variant = m_meta.variants["DeferredSkinned"_hs];
				variant				   = ShaderVariant{
								   .blendDisable	= true,
								   .depthTest		= true,
								   .depthWrite		= true,
								   .targets			= {{.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}},
								   .cullMode		= LinaGX::CullMode::Back,
								   .frontFace		= LinaGX::FrontFace::CW,
								   .indirectEnabled = true,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectSkinnedVertexMain(vertex, ShaderType::OpaqueSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::OpaqueSurface);

				variant._compileData[LinaGX::ShaderStage::Vertex] = {
					.includePath = includePath.c_str(),
					.text		 = vertex,
				};

				variant._compileData[LinaGX::ShaderStage::Fragment] = {
					.includePath = includePath.c_str(),
					.text		 = fragment,
				};
			}
		}
		else if (type == ShaderType::TransparentSurface)
		{
			ShaderPreprocessor::InjectRenderPassInputs(vertexBlock, RenderPassDescriptorType::Forward);
			ShaderPreprocessor::InjectRenderPassInputs(fragBlock, RenderPassDescriptorType::Forward);
			// Forward default
			{
				ShaderVariant& variant = m_meta.variants["ForwardDefault"_hs];
				variant				   = ShaderVariant{
								   .blendDisable		= false,
								   .blendSrcFactor		= LinaGX::BlendFactor::SrcAlpha,
								   .blendDstFactor		= LinaGX::BlendFactor::OneMinusSrcAlpha,
								   .blendColorOp		= LinaGX::BlendOp::Add,
								   .blendSrcAlphaFactor = LinaGX::BlendFactor::One,
								   .blendDstAlphaFactor = LinaGX::BlendFactor::One,
								   .blendAlphaOp		= LinaGX::BlendOp::Add,
								   .depthTest			= true,
								   .depthWrite			= true,
								   .targets				= {{.format = DEFAULT_RT_FORMAT}},
								   .cullMode			= LinaGX::CullMode::None,
								   .frontFace			= LinaGX::FrontFace::CCW,
								   .indirectEnabled		= true,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectVertexMain(vertex, ShaderType::TransparentSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::TransparentSurface);

				variant._compileData[LinaGX::ShaderStage::Vertex] = {
					.includePath = includePath.c_str(),
					.text		 = vertex,
				};
				variant._compileData[LinaGX::ShaderStage::Fragment] = {
					.includePath = includePath.c_str(),
					.text		 = fragment,
				};
			}

			// Forward default
			{
				ShaderVariant& variant = m_meta.variants["ForwardSkinned"_hs];
				variant				   = ShaderVariant{
								   .blendDisable		= false,
								   .blendSrcFactor		= LinaGX::BlendFactor::SrcAlpha,
								   .blendDstFactor		= LinaGX::BlendFactor::OneMinusSrcAlpha,
								   .blendColorOp		= LinaGX::BlendOp::Add,
								   .blendSrcAlphaFactor = LinaGX::BlendFactor::One,
								   .blendDstAlphaFactor = LinaGX::BlendFactor::One,
								   .blendAlphaOp		= LinaGX::BlendOp::Add,
								   .depthTest			= true,
								   .depthWrite			= true,
								   .targets				= {{.format = DEFAULT_RT_FORMAT}},
								   .cullMode			= LinaGX::CullMode::None,
								   .frontFace			= LinaGX::FrontFace::CCW,
								   .indirectEnabled		= true,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectSkinnedVertexMain(vertex, ShaderType::TransparentSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::TransparentSurface);

				variant._compileData[LinaGX::ShaderStage::Vertex] = {
					.includePath = includePath.c_str(),
					.text		 = vertex,
				};

				variant._compileData[LinaGX::ShaderStage::Fragment] = {
					.includePath = includePath.c_str(),
					.text		 = fragment,
				};
			}
		}
		else if (type == ShaderType::Lighting)
		{
			ShaderVariant& variant = m_meta.variants["Default"_hs];
			variant				   = ShaderVariant{
							   .blendDisable	= false,
							   .depthTest		= false,
							   .depthWrite		= false,
							   .targets			= {{.format = DEFAULT_RT_FORMAT}},
							   .cullMode		= LinaGX::CullMode::None,
							   .frontFace		= LinaGX::FrontFace::CW,
							   .indirectEnabled = false,
			   };

			variant._compileData[LinaGX::ShaderStage::Vertex] = {
				.includePath = includePath.c_str(),
				.text		 = vertexBlock,
			};

			variant._compileData[LinaGX::ShaderStage::Fragment] = {
				.includePath = includePath.c_str(),
				.text		 = fragBlock,
			};
		}
		else if (type == ShaderType::PostProcess)
		{
		}
		else if (type == ShaderType::Sky)
		{
			ShaderPreprocessor::InjectRenderPassInputs(vertexBlock, RenderPassDescriptorType::Lighting);
			ShaderPreprocessor::InjectRenderPassInputs(fragBlock, RenderPassDescriptorType::Lighting);

			ShaderVariant& variant = m_meta.variants["Default"_hs];
			variant				   = ShaderVariant{
							   .blendDisable	  = true,
							   .depthTest		  = true,
							   .depthWrite		  = false,
							   .targets			  = {{.format = DEFAULT_RT_FORMAT}},
							   .depthOp			  = LinaGX::CompareOp::Equal,
							   .cullMode		  = LinaGX::CullMode::Back,
							   .frontFace		  = LinaGX::FrontFace::CW,
							   .depthBiasEnable	  = true,
							   .depthBiasConstant = 5.0f,
							   .indirectEnabled	  = false,
			   };

			String vertex	= vertexBlock;
			String fragment = fragBlock;

			ShaderPreprocessor::InjectVertexMain(vertex, ShaderType::Sky);
			ShaderPreprocessor::InjectFragMain(fragment, ShaderType::Sky);

			variant._compileData[LinaGX::ShaderStage::Vertex] = {
				.includePath = includePath.c_str(),
				.text		 = vertex,
			};

			variant._compileData[LinaGX::ShaderStage::Fragment] = {
				.includePath = includePath.c_str(),
				.text		 = fragment,
			};
		}
		else if (type == ShaderType::Custom)
		{
			for (auto& [sid, variant] : m_meta.variants)
			{
				variant._compileData[LinaGX::ShaderStage::Vertex] = {
					.includePath = includePath.c_str(),
					.text		 = vertexBlock,
				};

				variant._compileData[LinaGX::ShaderStage::Fragment] = {
					.includePath = includePath.c_str(),
					.text		 = fragBlock,
				};
			}
		}

		for (auto& [sid, variant] : m_meta.variants)
		{
			success = LinaGX::Instance::CompileShader(variant._compileData, variant._outCompiledBlobs, variant._outLayout);

			if (!success)
			{
				LINA_ERR("Failed compiling shader! {0}", m_name);
				break;
			}
		}

		if (!success)
		{
			clearVariants();
			return false;
		}

		return true;
	}

	void Shader::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_meta;
		stream << m_properties;
	}

	void Shader::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_meta;
		stream >> m_properties;
	}

	void Shader::GenerateHW()
	{

		/*
		   Create a descriptor set description from the reflection info, this will be used to create descritor sets for the materials using this shader.
			Create a pipeline layout, using global set description, description of the render pass we are using, and the material set description.
		 Materials will use this layout when binding descriptor sets for this shader.
		 */

		// if (m_layout.descriptorSetLayouts.size() > 2)
		// {
		// 	const auto& setLayout = m_layout.descriptorSetLayouts[2];
		//
		// 	m_materialSetInfo = m_layout.descriptorSetLayouts[2];
		// 	m_materialSetDesc = {};
		// 	for (const auto& b : setLayout.bindings)
		// 		m_materialSetDesc.bindings.push_back(GfxHelpers::GetBindingFromShaderBinding(b));
		//
		// 	m_materialSetDesc.allocationCount = 1;
		// }

		m_descriptorSets.push_back(new DescriptorSet());
		m_descriptorSets[0]->Create(m_materialSetDesc);

		// Create variants
		for (auto& [sid, variant] : m_meta.variants)
		{
			LinaGX::ColorBlendAttachment blend = LinaGX::ColorBlendAttachment{
				.blendEnabled		 = !variant.blendDisable,
				.srcColorBlendFactor = variant.blendSrcFactor,
				.dstColorBlendFactor = variant.blendDstFactor,
				.colorBlendOp		 = variant.blendColorOp,
				.srcAlphaBlendFactor = variant.blendSrcAlphaFactor,
				.dstAlphaBlendFactor = variant.blendDstAlphaFactor,
				.alphaBlendOp		 = variant.blendAlphaOp,
				.componentFlags		 = {LinaGX::ColorComponentFlags::R, LinaGX::ColorComponentFlags::G, LinaGX::ColorComponentFlags::B, LinaGX::ColorComponentFlags::A},
			};

			LINAGX_VEC<LinaGX::ShaderColorAttachment> colorAttachments;
			colorAttachments.resize(variant.targets.size());

			for (size_t i = 0; i < variant.targets.size(); i++)
			{
				colorAttachments[i] = {
					.format			 = variant.targets[i].format,
					.blendAttachment = blend,
				};
			}

			LinaGX::ShaderDepthStencilDesc depthStencilAtt = {
				.depthStencilAttachmentFormat = variant.depthFormat,
				.depthWrite					  = variant.depthWrite,
				.depthTest					  = variant.depthTest,
				.depthCompare				  = variant.depthOp,
			};

			variant._gpuHandle = Application::GetLGX()->CreateShader({
				.stages					 = variant._outCompiledBlobs,
				.colorAttachments		 = colorAttachments,
				.depthStencilDesc		 = depthStencilAtt,
				.layout					 = variant._outLayout,
				.polygonMode			 = LinaGX::PolygonMode::Fill,
				.cullMode				 = variant.cullMode,
				.frontFace				 = variant.frontFace,
				.topology				 = variant.topology,
				.depthBiasEnable		 = variant.depthBiasEnable,
				.depthBiasConstant		 = variant.depthBiasConstant,
				.depthBiasClamp			 = variant.depthBiasClamp,
				.depthBiasSlope			 = variant.depthBiasSlope,
				.drawIndirectEnabled	 = variant.indirectEnabled,
				.useCustomPipelineLayout = false,
				.debugName				 = m_name.c_str(),
			});

			variant._gpuHandleExists = true;

			for (auto& [stage, blob] : variant._outCompiledBlobs)
				delete[] blob.ptr;

			variant._outCompiledBlobs.clear();
		}
	}

	void Shader::DestroyHW()
	{
		for (auto& [sid, var] : m_meta.variants)
		{
			if (!var._gpuHandleExists)
				continue;

			var._gpuHandleExists = false;
			Application::GetLGX()->DestroyShader(var._gpuHandle);
		}

		for (const auto& d : m_descriptorSets)
		{
			d->Destroy();
			delete d;
		}
	}

	Vector<ShaderProperty*> Shader::CopyProperties()
	{
		Vector<ShaderProperty*> props;

		for (ShaderProperty* property : m_properties)
		{
			ShaderProperty* copy = new ShaderProperty();
			copy->name			 = property->name;
			copy->sid			 = property->sid;
			copy->type			 = property->type;
			copy->data			 = {new uint8[property->data.size()], property->data.size()};
			MEMCPY(copy->data.data(), property->data.data(), property->data.size());
			props.push_back(copy);
		}

		return props;
	}
} // namespace Lina
