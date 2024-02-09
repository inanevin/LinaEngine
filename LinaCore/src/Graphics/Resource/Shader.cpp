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
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Utility/ShaderPreprocessor.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"

#include "Common/System/System.hpp"
#include "Common/Serialization/StringSerialization.hpp"
#include "Common/FileSystem//FileSystem.hpp"
#include "Common/Serialization/HashMapSerialization.hpp"

namespace Lina
{
	void Shader::Metadata::SaveToStream(OStream& out) const
	{
		HashMapSerialization::SaveToStream_OBJ(out, variants);
		out << static_cast<uint8>(renderPassDescriptorType);
	}

	void Shader::Metadata::LoadFromStream(IStream& in)
	{
		HashMapSerialization::LoadFromStream_OBJ(in, variants);

		uint8 rpType = 0;
		in >> rpType;
		renderPassDescriptorType = static_cast<RenderPassDescriptorType>(rpType);
	}

	Shader::Shader(ResourceManager* rm, const String& path, StringID sid) : Resource(rm, path, sid, GetTypeID<Shader>())
	{
		m_gfxManager = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_lgx		 = m_gfxManager->GetLGX();
	};

	Shader::~Shader()
	{
		m_lgx->DestroyPipelineLayout(m_pipelineLayout);

		for (const auto& [sid, var] : m_meta.variants)
			m_lgx->DestroyShader(var.gpuHandle);

		for (const auto& d : m_descriptorSets)
		{
			d->Destroy();
			delete d;
		}
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
		set->Create(m_lgx, m_materialSetDesc);
		m_descriptorSets.push_back(set);
	}

	void Shader::FreeDescriptorSet(DescriptorSet* set, uint32 index)
	{
		set->Free(index);

		// Shrink if not used anymore.
		if (set->IsEmpty())
		{
			linatl::remove_if(m_descriptorSets.begin(), m_descriptorSets.end(), [set](DescriptorSet* s) -> bool { return s == set; });
			set->Destroy();
			delete set;
		}
	}

	void Shader::LoadFromFile(const char* path)
	{
		auto													   gfxMan = m_resourceManager->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		LINAGX_MAP<LinaGX::ShaderStage, LinaGX::ShaderCompileData> data;
		LINAGX_MAP<LinaGX::ShaderStage, String>					   blocks;

		String		 txt		 = FileSystem::ReadFileContentsAsString(path);
		const String includePath = FileSystem::GetFilePath(path);

		HashMap<LinaGX::ShaderStage, String> outStages;

		const bool success = ShaderPreprocessor::Preprocess(txt, outStages, m_meta.renderPassDescriptorType);
		if (!success)
			return;

		for (const auto& [stg, text] : outStages)
		{
			LinaGX::ShaderCompileData compData;
			compData.includePath = includePath.c_str();
			compData.text		 = text.c_str();
			data[stg]			 = compData;
		}

		m_lgx->CompileShader(data, m_outCompiledBlobs, m_layout);

		if (m_meta.variants.empty())
			m_meta.variants["Default"_hs] = {};
	}

	void Shader::SaveToStream(OStream& stream) const
	{
		m_meta.SaveToStream(stream);

		const uint32 size = static_cast<uint32>(m_outCompiledBlobs.size());
		stream << size;

		for (const auto& [stage, blob] : m_outCompiledBlobs)
		{
			const uint8 stg = static_cast<uint8>(stage);
			stream << stg;
			stream << blob.size;
			stream.WriteEndianSafe(blob.ptr, blob.size);
		}

		SaveLinaGXShaderLayout(stream, m_layout);
	}

	void Shader::LoadFromStream(IStream& stream)
	{
		m_meta.LoadFromStream(stream);

		uint32 size = 0;
		stream >> size;

		for (uint32 i = 0; i < size; i++)
		{
			uint8 stg = 0;
			stream >> stg;
			LinaGX::DataBlob blob;
			stream >> blob.size;

			if (blob.size != 0)
			{
				blob.ptr = new uint8[blob.size];
				stream.ReadEndianSafe(blob.ptr, blob.size);
			}

			m_outCompiledBlobs[static_cast<LinaGX::ShaderStage>(stg)] = blob;
		}

		m_layout = {};
		LoadLinaGXShaderLayout(stream, m_layout);
	}

	void Shader::BatchLoaded()
	{
		/*
		   Create a descriptor set description from the reflection info, this will be used to create descritor sets for the materials using this shader.
			Create a pipeline layout, using global set description, description of the render pass we are using, and the material set description.
		 Materials will use this layout when binding descriptor sets for this shader.
		 */
		LinaGX::PipelineLayoutDesc plDesc = {.descriptorSetDescriptions = {GfxHelpers::GetSetDescPersistentGlobal(), GfxHelpers::GetSetDescPersistentRenderPass(m_meta.renderPassDescriptorType)}};
		if (m_layout.descriptorSetLayouts.size() > 2)
		{
			const auto& setLayout = m_layout.descriptorSetLayouts[2];

			m_materialSetInfo = m_layout.descriptorSetLayouts[2];
			m_materialSetDesc = {};
			for (const auto& b : setLayout.bindings)
				m_materialSetDesc.bindings.push_back(GfxHelpers::GetBindingFromShaderBinding(b));

			m_materialSetDesc.allocationCount = m_meta.descriptorSetAllocationCount;
			plDesc.descriptorSetDescriptions.push_back(m_materialSetDesc);
		}
		m_pipelineLayout = m_lgx->CreatePipelineLayout(plDesc);

		m_descriptorSets.push_back(new DescriptorSet());
		m_descriptorSets[0]->Create(m_lgx, m_materialSetDesc);

		// Create variants
		for (auto& [sid, variant] : m_meta.variants)
		{
			LinaGX::Format format = LinaGX::Format::B8G8R8A8_SRGB;

			if (variant.targetType == ShaderWriteTargetType::RenderTarget)
				format = LinaGX::Format::R32G32B32A32_SFLOAT;

			LinaGX::ColorBlendAttachment blend = LinaGX::ColorBlendAttachment{
				.blendEnabled		 = !variant.blendDisable,
				.srcColorBlendFactor = LinaGX::BlendFactor::SrcAlpha,
				.dstColorBlendFactor = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.colorBlendOp		 = LinaGX::BlendOp::Add,
				.srcAlphaBlendFactor = LinaGX::BlendFactor::One,
				.dstAlphaBlendFactor = LinaGX::BlendFactor::Zero,
				.alphaBlendOp		 = LinaGX::BlendOp::Add,
				.componentFlags		 = {LinaGX::ColorComponentFlags::R, LinaGX::ColorComponentFlags::G, LinaGX::ColorComponentFlags::B, LinaGX::ColorComponentFlags::A},
			};

			LINAGX_VEC<LinaGX::ShaderColorAttachment> colorAttachments;
			colorAttachments.resize(1);

			colorAttachments[0] = {
				.format			 = format,
				.blendAttachment = blend,
			};

			LinaGX::ShaderDepthStencilDesc depthStencilAtt = {
				.depthStencilAttachmentFormat = LinaGX::Format::D32_SFLOAT,
				.depthWrite					  = !variant.depthDisable,
				.depthTest					  = !variant.depthDisable,
				.depthCompare				  = LinaGX::CompareOp::Less,
			};

			variant.gpuHandle = m_lgx->CreateShader({
				.stages					 = m_outCompiledBlobs,
				.colorAttachments		 = colorAttachments,
				.depthStencilDesc		 = depthStencilAtt,
				.layout					 = m_layout,
				.polygonMode			 = LinaGX::PolygonMode::Fill,
				.cullMode				 = variant.cullMode,
				.frontFace				 = variant.frontFace,
				.topology				 = LinaGX::Topology::TriangleList,
				.useCustomPipelineLayout = true,
				.customPipelineLayout	 = m_pipelineLayout,
				.debugName				 = m_path.c_str(),
			});
		}

		for (auto& [stage, blob] : m_outCompiledBlobs)
		{
			delete[] blob.ptr;
		}
	}

	void Shader::Flush()
	{
	}

} // namespace Lina
