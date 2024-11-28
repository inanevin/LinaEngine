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
#include <LinaGX/Utility/SPIRVUtility.hpp>

namespace Lina
{
	Shader::~Shader()
	{
		m_propertyDefinitions.clear();

		for (ShaderVariant& variant : m_meta.variants)
		{
			for (LinaGX::ShaderCompileData& data : variant._compileData)
				delete[] data.outBlob.ptr;
		}
	}

	void Shader::Bind(LinaGX::CommandStream* stream, uint32 gpuHandle)
	{
		LinaGX::CMDBindPipeline* bind = stream->AddCommand<LinaGX::CMDBindPipeline>();
		bind->shader				  = gpuHandle;
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

		m_shaderType = ShaderPreprocessor::GetShaderType(txt);

		String vertexBlock = "", fragBlock = "";
		if (!ShaderPreprocessor::ExtractVertexFrag(txt, vertexBlock, fragBlock))
			return false;

		ShaderPreprocessor::InjectVersionAndExtensions(vertexBlock);
		ShaderPreprocessor::InjectVersionAndExtensions(fragBlock);

		auto injectMaterials = [&]() -> bool {
			Vector<ShaderPropertyDefinition> vertexProperties = {}, fragProperties = {};
			if (!ShaderPreprocessor::InjectMaterialIfRequired(vertexBlock, vertexProperties))
				return false;

			if (!ShaderPreprocessor::InjectMaterialIfRequired(fragBlock, fragProperties))
				return false;

			if (!ShaderPropertyDefinition::VerifySimilarity(vertexProperties, fragProperties))
			{
				LINA_ERR("LinaMaterial structs in vertex and fragment shaders are different!");
				return false;
			}
			m_propertyDefinitions = fragProperties.empty() ? vertexProperties : fragProperties;
			return true;
		};

		const ShaderType type	 = m_shaderType;
		bool			 success = true;

		auto clearVariants = [this]() {
			for (ShaderVariant& variant : m_meta.variants)
			{
				for (LinaGX::ShaderCompileData& data : variant._compileData)
				{
					if (data.outBlob.ptr != nullptr)
						delete[] data.outBlob.ptr;
				}
				variant._compileData.clear();
			}
		};

		if (type != ShaderType::Custom)
			clearVariants();

		if (type == ShaderType::OpaqueSurface)
		{
			if (!injectMaterials())
				return false;

			ShaderPreprocessor::InjectRenderPassInputs(vertexBlock, RenderPassType::Deferred);
			ShaderPreprocessor::InjectRenderPassInputs(fragBlock, RenderPassType::Deferred);

			// Deferred default
			{
				m_meta.variants.push_back({});
				ShaderVariant& variant = m_meta.variants.back();
				variant				   = ShaderVariant{
								   .id			 = "DeferredDefault"_hs,
								   .blendDisable = true,
								   .depthTest	 = true,
								   .depthWrite	 = true,
								   .targets		 = {{.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}},
								   .cullMode	 = LinaGX::CullMode::Back,
								   .frontFace	 = LinaGX::FrontFace::CW,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectVertexMain(vertex, ShaderType::OpaqueSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::OpaqueSurface);

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Vertex,
					.text		 = vertex,
					.includePath = includePath.c_str(),
				});

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Fragment,
					.text		 = fragment,
					.includePath = includePath.c_str(),
				});
			}

			// Deferred skinned
			{
				m_meta.variants.push_back({});
				ShaderVariant& variant = m_meta.variants.back();
				variant				   = ShaderVariant{
								   .id			 = "DeferredSkinned"_hs,
								   .blendDisable = true,
								   .depthTest	 = true,
								   .depthWrite	 = true,
								   .targets		 = {{.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}},
								   .cullMode	 = LinaGX::CullMode::Back,
								   .frontFace	 = LinaGX::FrontFace::CW,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectSkinnedVertexMain(vertex, ShaderType::OpaqueSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::OpaqueSurface);

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Vertex,
					.text		 = vertex,
					.includePath = includePath.c_str(),
				});

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Fragment,
					.text		 = fragment,
					.includePath = includePath.c_str(),
				});
			}
		}
		else if (type == ShaderType::TransparentSurface)
		{
			if (!injectMaterials())
				return false;

			ShaderPreprocessor::InjectRenderPassInputs(vertexBlock, RenderPassType::Forward);
			ShaderPreprocessor::InjectRenderPassInputs(fragBlock, RenderPassType::Forward);
			// Forward default
			{
				m_meta.variants.push_back({});
				ShaderVariant& variant = m_meta.variants.back();
				variant				   = ShaderVariant{
								   .id					= "ForwardDefault"_hs,
								   .blendDisable		= false,
								   .blendSrcFactor		= LinaGX::BlendFactor::SrcAlpha,
								   .blendDstFactor		= LinaGX::BlendFactor::OneMinusSrcAlpha,
								   .blendColorOp		= LinaGX::BlendOp::Add,
								   .blendSrcAlphaFactor = LinaGX::BlendFactor::One,
								   .blendDstAlphaFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
								   .blendAlphaOp		= LinaGX::BlendOp::Add,
								   .depthTest			= true,
								   .depthWrite			= true,
								   .targets				= {{.format = DEFAULT_RT_FORMAT}},
								   .cullMode			= LinaGX::CullMode::Back,
								   .frontFace			= LinaGX::FrontFace::CW,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectVertexMain(vertex, ShaderType::TransparentSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::TransparentSurface);

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Vertex,
					.text		 = vertex,
					.includePath = includePath.c_str(),
				});

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Fragment,
					.text		 = fragment,
					.includePath = includePath.c_str(),
				});
			}

			// Forward default
			{
				m_meta.variants.push_back({});
				ShaderVariant& variant = m_meta.variants.back();
				variant				   = ShaderVariant{
								   .id					= "ForwardSkinned"_hs,
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
								   .cullMode			= LinaGX::CullMode::Back,
								   .frontFace			= LinaGX::FrontFace::CW,
				   };

				String vertex	= vertexBlock;
				String fragment = fragBlock;

				ShaderPreprocessor::InjectSkinnedVertexMain(vertex, ShaderType::TransparentSurface);
				ShaderPreprocessor::InjectFragMain(fragment, ShaderType::TransparentSurface);

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Vertex,
					.text		 = vertex,
					.includePath = includePath.c_str(),
				});

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Fragment,
					.text		 = fragment,
					.includePath = includePath.c_str(),
				});
			}
		}
		else if (type == ShaderType::Lighting)
		{
			if (!injectMaterials())
				return false;

			m_meta.variants.push_back({});
			ShaderVariant& variant = m_meta.variants.back();
			variant				   = ShaderVariant{
							   .id			 = "Default"_hs,
							   .blendDisable = false,
							   .depthTest	 = false,
							   .depthWrite	 = false,
							   .targets		 = {{.format = DEFAULT_RT_FORMAT}},
							   .cullMode	 = LinaGX::CullMode::None,
							   .frontFace	 = LinaGX::FrontFace::CW,
			   };

			variant._compileData.push_back({
				.stage		 = LinaGX::ShaderStage::Vertex,
				.text		 = vertexBlock,
				.includePath = includePath.c_str(),
			});

			variant._compileData.push_back({
				.stage		 = LinaGX::ShaderStage::Fragment,
				.text		 = fragBlock,
				.includePath = includePath.c_str(),
			});
		}
		else if (type == ShaderType::PostProcess)
		{
			if (!injectMaterials())
				return false;
		}
		else if (type == ShaderType::Sky)
		{
			if (!injectMaterials())
				return false;

			ShaderPreprocessor::InjectRenderPassInputs(vertexBlock, RenderPassType::Lighting);
			ShaderPreprocessor::InjectRenderPassInputs(fragBlock, RenderPassType::Lighting);

			m_meta.variants.push_back({});
			ShaderVariant& variant = m_meta.variants.back();

			variant = ShaderVariant{
				.id				   = "Default"_hs,
				.blendDisable	   = true,
				.depthTest		   = true,
				.depthWrite		   = false,
				.targets		   = {{.format = DEFAULT_RT_FORMAT}},
				.depthOp		   = LinaGX::CompareOp::Equal,
				.cullMode		   = LinaGX::CullMode::Back,
				.frontFace		   = LinaGX::FrontFace::CW,
				.depthBiasEnable   = true,
				.depthBiasConstant = 5.0f,
			};

			String vertex	= vertexBlock;
			String fragment = fragBlock;

			ShaderPreprocessor::InjectVertexMain(vertex, ShaderType::Sky);
			ShaderPreprocessor::InjectFragMain(fragment, ShaderType::Sky);

			variant._compileData.push_back({
				.stage		 = LinaGX::ShaderStage::Vertex,
				.text		 = vertex,
				.includePath = includePath.c_str(),
			});

			variant._compileData.push_back({
				.stage		 = LinaGX::ShaderStage::Fragment,
				.text		 = fragment,
				.includePath = includePath.c_str(),
			});
		}
		else if (type == ShaderType::Custom)
		{
			for (ShaderVariant& variant : m_meta.variants)
			{
				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Vertex,
					.text		 = vertexBlock,
					.includePath = includePath.c_str(),
				});

				variant._compileData.push_back({
					.stage		 = LinaGX::ShaderStage::Fragment,
					.text		 = fragBlock,
					.includePath = includePath.c_str(),
				});
			}
		}

		for (ShaderVariant& variant : m_meta.variants)
		{
			for (LinaGX::ShaderCompileData& data : variant._compileData)
			{
				String fullText = "";
				LinaGX::SPIRVUtility::GetShaderTextWithIncludes(fullText, data.text, data.includePath);
				data.text = fullText;
			}

			success = LinaGX::Instance::CompileShader(variant._compileData, variant._outLayout);

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
		stream << m_propertyDefinitions;
		stream << m_shaderType;
	}

	void Shader::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_meta;
		stream >> m_propertyDefinitions;
		stream >> m_shaderType;
	}

	void Shader::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");

		m_gpuHandles.clear();

		// Create variants
		for (ShaderVariant& variant : m_meta.variants)
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

			const uint32 handle = Application::GetLGX()->CreateShader({
				.stages					 = variant._compileData,
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
				.useCustomPipelineLayout = false,
				.debugName				 = m_name.c_str(),
			});

			m_gpuHandles.push_back({variant.id, handle});

			for (LinaGX::ShaderCompileData& data : variant._compileData)
				delete[] data.outBlob.ptr;

			variant._compileData.clear();
		}

		m_hwValid = true;
	}

	void Shader::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;

		for (Pair<StringID, uint32> pair : m_gpuHandles)
			Application::GetLGX()->DestroyShader(pair.second);
	}

} // namespace Lina
