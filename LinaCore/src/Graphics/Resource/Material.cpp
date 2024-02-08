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

	Material::Material(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : Resource(rm, isUserManaged, path, sid, GetTypeID<Material>())
	{
		m_gfxManager = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_lgx		 = m_gfxManager->GetLGX();
	}

	Material::~Material()
	{
		DestroyBindingData();
	}

	void Material::SetShader(StringID sid)
	{
		m_shader	= m_resourceManager->GetResource<Shader>(m_shaderSID);
		m_shaderSID = sid;
		LINA_ASSERT(m_shader != nullptr, "Shader is null!");
		DestroyBindingData();

		/*

		 1. Create descriptor set from shader description.
		 2. For every binding in shader, create either a buffer, texture or sampler for that binding and assign defaults.
		 3. UpdateBinding() for each binding -> update the newly created sets to point to those buffers & textures.
		 */

		const LinaGX::DescriptorSetDesc			desc = m_shader->GetMaterialSetDesc();
		const LinaGX::ShaderDescriptorSetLayout info = m_shader->GetMaterialSetInfo();
		m_bindingData.resize(desc.bindings.size());

		for (int32 f = 0; f < FRAMES_IN_FLIGHT; f++)
		{
			m_descriptorSets[f] = m_lgx->CreateDescriptorSet(desc);

			uint32 bindingIndex = 0;
			for (const auto& b : desc.bindings)
			{
				auto& matBindingData = m_bindingData[bindingIndex];
				if (b.type == LinaGX::DescriptorType::UBO || b.type == LinaGX::DescriptorType::SSBO)
				{
					const int32	 count	  = b.type == LinaGX::DescriptorType::UBO ? b.descriptorCount : 1;
					const uint32 bufferSz = static_cast<uint32>(info.bindings[bindingIndex].size);
					matBindingData.bufferData[f].buffers.resize(count);

					for (int32 i = 0; i < count; i++)
					{
						auto& buf = matBindingData.bufferData[f].buffers[i];
						buf.Create(m_lgx, b.type == LinaGX::DescriptorType::UBO ? LinaGX::ResourceTypeHint::TH_ConstantBuffer : LinaGX::ResourceTypeHint::TH_StorageBuffer, bufferSz, "Material Buffer", true);
						MEMSET(buf.GetMapped(), 0, bufferSz);
					}
				}
				else if (b.type == LinaGX::DescriptorType::SeparateImage || b.type == LinaGX::DescriptorType::CombinedImageSampler)
				{
					matBindingData.textures.resize(b.descriptorCount);
					for (int32 i = 0; i < b.descriptorCount; i++)
						matBindingData.textures[i] = DEFAULT_TEXTURE_SID;
				}
				else if (b.type == LinaGX::DescriptorType::SeparateSampler || b.type == LinaGX::DescriptorType::CombinedImageSampler)
				{
					matBindingData.samplers.resize(b.descriptorCount);
					for (int32 i = 0; i < b.descriptorCount; i++)
						matBindingData.samplers[i] = DEFAULT_SAMPLER_SID;
				}
				bindingIndex++;
			}
		}

		const int32 sz = static_cast<int32>(m_bindingData.size());
		for (int32 i = 0; i < sz; i++)
			UpdateBinding(i);
	}

	void Material::Bind(LinaGX::CommandStream* stream, uint32 frameIndex)
	{
		LinaGX::CMDBindDescriptorSets* bind = stream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bind->descriptorSetHandles			= stream->EmplaceAuxMemory<uint16>(m_descriptorSets[frameIndex]);
		bind->firstSet						= 2;
		bind->setCount						= 1;
	}

	void Material::SetBuffer(uint32 bindingIndex, uint32 descriptorIndex, size_t padding, uint8* data, size_t dataSize)
	{
		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& buf = m_bindingData[bindingIndex].bufferData[i].buffers[descriptorIndex];
			buf.BufferData(padding, data, dataSize);
		}
	}

	void Material::SetTexture(uint32 bindingIndex, uint32 descriptorIndex, StringID sid)
	{
		auto* rm						= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto& bData						= m_bindingData[bindingIndex];
		bData.textures[descriptorIndex] = sid;
		UpdateBinding(bindingIndex);
	}

	void Material::SetSampler(uint32 bindingIndex, uint32 descriptorIndex, StringID sid)
	{
		auto* rm						= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto& bData						= m_bindingData[bindingIndex];
		bData.samplers[descriptorIndex] = sid;
		UpdateBinding(bindingIndex);
	}

	void Material::SetCombinedImageSampler(uint32 bindingIndex, uint32 descriptorIndex, StringID texture, StringID sampler)
	{
		auto* rm						= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto& bData						= m_bindingData[bindingIndex];
		bData.textures[descriptorIndex] = texture;
		bData.samplers[descriptorIndex] = sampler;
		UpdateBinding(bindingIndex);
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
		const int32 sz = static_cast<int32>(m_bindingData.size());
		for (int32 i = 0; i < sz; i++)
			UpdateBinding(i);
	}

	void Material::UpdateBinding(uint32 bindingIndex)
	{
		auto*		rm		= m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		const auto& binding = m_bindingData[bindingIndex];

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			if (!binding.bufferData[i].buffers.empty())
			{
				LinaGX::DescriptorUpdateBufferDesc update = {
					.setHandle = m_descriptorSets[i],
					.binding   = bindingIndex,
				};

				for (const auto& buf : binding.bufferData[i].buffers)
					update.buffers.push_back(buf.GetGPUResource());

				m_lgx->DescriptorUpdateBuffer(update);
			}
			else
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSets[i],
					.binding   = bindingIndex,
				};

				for (const auto& txt : binding.textures)
					update.textures.push_back(rm->GetResource<Texture>(txt)->GetGPUHandle());

				for (const auto& smp : binding.samplers)
					update.samplers.push_back(rm->GetResource<TextureSampler>(smp)->GetGPUHandle());

				m_lgx->DescriptorUpdateImage(update);
			}
		}
	}

	void Material::DestroyBindingData()
	{
		if (!m_bindingData.empty())
		{
			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				m_lgx->DestroyDescriptorSet(m_descriptorSets[i]);

				for (auto& b : m_bindingData)
				{
					for (auto& buf : b.bufferData[i].buffers)
						buf.Destroy();
				}
			}

			m_bindingData.clear();
		}
	}

} // namespace Lina
