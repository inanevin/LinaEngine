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

#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Pipeline/DescriptorSet.hpp"
#include "Common/System/System.hpp"
#include "Common/Serialization/StringSerialization.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{

	void Material::BindingData::SaveToStream(OStream& stream) const
	{
		stream << static_cast<int32>(bufferData[0].buffers.size());

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			for (const auto& buf : bufferData[i].buffers)
				buf.SaveToStream(stream);
		}

		stream << static_cast<int32>(textures.size());
		stream << static_cast<int32>(samplers.size());

		for (const auto& txt : textures)
			stream << txt;

		for (const auto& smp : samplers)
			stream << smp;
	}

	void Material::BindingData::LoadFromStream(LinaGX::Instance* lgx, IStream& stream)
	{
		int32 buffersSz = 0, texturesSz = 0, samplersSz = 0;
		stream >> buffersSz >> texturesSz >> samplersSz;

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			bufferData[i].buffers.resize(buffersSz);

			for (int32 j = 0; j < buffersSz; j++)
				bufferData[i].buffers[j].LoadFromStream(lgx, stream);
		}

		textures.resize(texturesSz);
		samplers.resize(samplersSz);

		for (int32 i = 0; i < texturesSz; i++)
			stream >> textures[i];

		for (int32 i = 0; i < samplersSz; i++)
			stream >> samplers[i];
	}

	Material::Material(ResourceManager* rm, const String& path, StringID sid) : Resource(rm, path, sid, GetTypeID<Material>())
	{
		m_gfxManager = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_lgx		 = m_gfxManager->GetLGX();
	}

	Material::~Material()
	{
		DestroyDescriptorSets();
		DestroyBindingData();
	}

	void Material::SetShader(StringID sid)
	{
		m_shaderSID = sid;
		m_shader	= m_resourceManager->GetResource<Shader>(m_shaderSID);

		if (m_shader == nullptr)
		{
			m_shaderSID = DEFAULT_SHADER_OBJECT_SID;
			m_shader	= m_resourceManager->GetResource<Shader>(m_shaderSID);
		}

		m_gfxManager->Join();
		CreateDescriptorSets();
		CreateBindingData();
	}

	void Material::Bind(LinaGX::CommandStream* stream, uint32 frameIndex, LinaGX::DescriptorSetsLayoutSource layoutSource, uint32 customShaderHandle)
	{
		LinaGX::CMDBindDescriptorSets* bind = stream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bind->descriptorSetHandles			= stream->EmplaceAuxMemory<uint16>(m_descriptorSetContainer[frameIndex].set->GetGPUHandle());
		bind->firstSet						= 2;
		bind->setCount						= 1;
		bind->allocationIndices				= stream->EmplaceAuxMemory<uint32>(m_descriptorSetContainer[frameIndex].allocIndex);
		bind->layoutSource					= layoutSource;

		if (layoutSource == LinaGX::DescriptorSetsLayoutSource::CustomLayout)
			bind->customLayout = m_shader->GetPipelineLayout();
		else if (layoutSource == LinaGX::DescriptorSetsLayoutSource::CustomShader)
			bind->customLayoutShader = customShaderHandle;
	}

	void Material::SetBuffer(uint32 bindingIndex, uint32 descriptorIndex, size_t padding, uint8* data, size_t dataSize, bool currentFrameOnly)
	{
		if (currentFrameOnly)
		{
			auto& buf = m_bindingData[bindingIndex].bufferData[m_gfxManager->GetCurrentFrameIndex()].buffers[descriptorIndex];
			buf.BufferData(padding, data, dataSize);
		}
		else
		{
			m_gfxManager->Join();
			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& buf = m_bindingData[bindingIndex].bufferData[i].buffers[descriptorIndex];
				buf.BufferData(padding, data, dataSize);
			}
		}
	}

	void Material::SetTexture(uint32 bindingIndex, uint32 descriptorIndex, uint32 gpuHandle, bool currentFrameOnly)
	{
		auto& bData						= m_bindingData[bindingIndex];
		bData.textures[descriptorIndex] = gpuHandle;

		if (currentFrameOnly)
			UpdateBinding(bindingIndex, m_gfxManager->GetCurrentFrameIndex());
		else
		{
			m_gfxManager->Join();

			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				UpdateBinding(bindingIndex, i);
		}
	}

	void Material::SetSampler(uint32 bindingIndex, uint32 descriptorIndex, uint32 gpuHandle, bool currentFrameOnly)
	{
		auto& bData						= m_bindingData[bindingIndex];
		bData.samplers[descriptorIndex] = gpuHandle;

		if (currentFrameOnly)
			UpdateBinding(bindingIndex, m_gfxManager->GetCurrentFrameIndex());
		else
		{
			m_gfxManager->Join();

			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				UpdateBinding(bindingIndex, i);
		}
	}

	void Material::SetCombinedImageSampler(uint32 bindingIndex, uint32 descriptorIndex, uint32 textureGPUHandle, uint32 samplerGPUHandle, bool currentFrameOnly)
	{
		auto* rm						= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto& bData						= m_bindingData[bindingIndex];
		bData.textures[descriptorIndex] = textureGPUHandle;
		bData.samplers[descriptorIndex] = samplerGPUHandle;

		if (currentFrameOnly)
			UpdateBinding(bindingIndex, m_gfxManager->GetCurrentFrameIndex());
		else
		{
			m_gfxManager->Join();

			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				UpdateBinding(bindingIndex, i);
		}
	}

	void Material::LoadFromFile(const char* path)
	{
		IStream stream = Serialization::LoadFromFile(path);

		if (stream.GetDataRaw() != nullptr)
			LoadFromStream(stream);

		stream.Destroy();
	}

	void Material::SaveToStream(OStream& stream) const
	{
		stream << m_shaderSID;
		stream << static_cast<int32>(m_bindingData.size());

		for (const auto& b : m_bindingData)
			b.SaveToStream(stream);
	}

	void Material::LoadFromStream(IStream& stream)
	{
		int32 bindingSz = 0;
		stream >> m_shaderSID >> bindingSz;
		m_bindingData.resize(bindingSz);

		for (int32 i = 0; i < bindingSz; i++)
			m_bindingData[i].LoadFromStream(m_lgx, stream);
	}

	void Material::BatchLoaded()
	{
		/*
		 If we are loaded, it means we have our shaderSID and m_bindings data loaded from somewhere.
		 If our shader is still valid (might be deleted), create descriptor sets using it & update bindings.
		 If it's not valid, set our shader to be default, create descriptor sets and re-create the bindings.
		 */

		auto* rm			= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto* defaultShader = rm->GetResource<Shader>(DEFAULT_SHADER_OBJECT_SID);

		m_shader = rm->GetResource<Shader>(m_shaderSID);

		if (m_shader != nullptr)
		{
			CreateDescriptorSets();

			const int32 sz = static_cast<int32>(m_bindingData.size());
			for (int32 i = 0; i < sz; i++)
			{
				for (int32 j = 0; j < FRAMES_IN_FLIGHT; j++)
					UpdateBinding(i, j);
			}
		}
		else
		{
			m_shaderSID = DEFAULT_SHADER_OBJECT_SID;
			m_shader	= rm->GetResource<Shader>(DEFAULT_SHADER_OBJECT_SID);

			CreateDescriptorSets();
			CreateBindingData();
		}
	}

	void Material::CreateDescriptorSets()
	{
		static int a = 0;
		DestroyDescriptorSets();
		for (int32 f = 0; f < FRAMES_IN_FLIGHT; f++)
			m_shader->AllocateDescriptorSet(m_descriptorSetContainer[f].set, m_descriptorSetContainer[f].allocIndex);
	}

	void Material::DestroyDescriptorSets()
	{
		if (m_descriptorSetContainer[0].set == nullptr)
			return;

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			m_shader->FreeDescriptorSet(m_descriptorSetContainer[i].set, m_descriptorSetContainer[i].allocIndex);
	}

	void Material::CreateBindingData()
	{
		DestroyBindingData();

		/*

		 1. Create descriptor set from shader description.
		 2. For every binding in shader, create either a buffer, texture or sampler for that binding and assign defaults.
		 3. UpdateBinding() for each binding -> update the newly created sets to point to those buffers & textures.
		 */

		const LinaGX::DescriptorSetDesc			desc = m_shader->GetMaterialSetDesc();
		const LinaGX::ShaderDescriptorSetLayout info = m_shader->GetMaterialSetInfo();
		m_bindingData.resize(desc.bindings.size());

		const uint32 defaultTextureGPUHandle = m_resourceManager->GetResource<Texture>(DEFAULT_TEXTURE_EMPTY_BLACK)->GetGPUHandle();
		const uint32 defaultSamplerGPUHandle = m_resourceManager->GetResource<TextureSampler>(DEFAULT_SAMPLER_SID)->GetGPUHandle();

		for (int32 f = 0; f < FRAMES_IN_FLIGHT; f++)
		{
			uint32 bindingIndex = 0;
			for (const auto& b : desc.bindings)
			{
				auto& matBindingData = m_bindingData[bindingIndex];
				if (b.type == LinaGX::DescriptorType::UBO || b.type == LinaGX::DescriptorType::SSBO)
				{
					const bool	 isUBO	  = b.type == LinaGX::DescriptorType::UBO;
					const int32	 count	  = b.type == LinaGX::DescriptorType::UBO ? b.descriptorCount : 1;
					const uint32 bufferSz = static_cast<uint32>(info.bindings[bindingIndex].size);
					matBindingData.bufferData[f].buffers.resize(count);

					for (int32 i = 0; i < count; i++)
					{
						auto& buf = matBindingData.bufferData[f].buffers[i];
						buf.Create(m_lgx, isUBO ? LinaGX::ResourceTypeHint::TH_ConstantBuffer : LinaGX::ResourceTypeHint::TH_StorageBuffer, bufferSz, m_path, true);
						buf.MemsetMapped(0);
					}
				}
				else if (b.type == LinaGX::DescriptorType::SeparateImage || b.type == LinaGX::DescriptorType::CombinedImageSampler)
				{
					matBindingData.textures.resize(b.descriptorCount);
					for (int32 i = 0; i < b.descriptorCount; i++)
						matBindingData.textures[i] = defaultTextureGPUHandle;
				}
				else if (b.type == LinaGX::DescriptorType::SeparateSampler || b.type == LinaGX::DescriptorType::CombinedImageSampler)
				{
					matBindingData.samplers.resize(b.descriptorCount);
					for (int32 i = 0; i < b.descriptorCount; i++)
						matBindingData.samplers[i] = defaultSamplerGPUHandle;
				}
				bindingIndex++;
			}
		}

		const int32 sz = static_cast<int32>(m_bindingData.size());
		for (int32 i = 0; i < sz; i++)
		{
			for (int32 j = 0; j < FRAMES_IN_FLIGHT; j++)
				UpdateBinding(i, j);
		}
	}

	void Material::DestroyBindingData()
	{
		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			for (auto& b : m_bindingData)
			{
				for (auto& buf : b.bufferData[i].buffers)
					buf.Destroy();
			}
		}

		m_bindingData.clear();
	}

	void Material::UpdateBinding(uint32 bindingIndex, uint32 frameIndex)
	{
		auto*		rm		= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		const auto& binding = m_bindingData[bindingIndex];

		if (!binding.bufferData[frameIndex].buffers.empty())
		{
			LinaGX::DescriptorUpdateBufferDesc update = {
				.setHandle			= m_descriptorSetContainer[frameIndex].set->GetGPUHandle(),
				.setAllocationIndex = m_descriptorSetContainer[frameIndex].allocIndex,
				.binding			= bindingIndex,
			};

			for (const auto& buf : binding.bufferData[frameIndex].buffers)
				update.buffers.push_back(buf.GetGPUResource());

			m_lgx->DescriptorUpdateBuffer(update);
		}
		else
		{
			LinaGX::DescriptorUpdateImageDesc update = {
				.setHandle			= m_descriptorSetContainer[frameIndex].set->GetGPUHandle(),
				.setAllocationIndex = m_descriptorSetContainer[frameIndex].allocIndex,
				.binding			= bindingIndex,
			};

			for (const auto& txt : binding.textures)
				update.textures.push_back(txt);

			for (const auto& smp : binding.samplers)
				update.samplers.push_back(smp);

			m_lgx->DescriptorUpdateImage(update);
		}
	}

} // namespace Lina
