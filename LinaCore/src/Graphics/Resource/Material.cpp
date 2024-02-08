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
#include "Common/System/System.hpp"
#include "Common/Serialization/StringSerialization.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{

	void Material::BindingData::SaveToStream(OStream& stream) const
	{
		stream << static_cast<int32>(buffers.size());
		stream << static_cast<int32>(textures.size());
		stream << static_cast<int32>(samplers.size());

		for (const auto& buf : buffers)
		{
			buf.SaveToStream(stream);
		}

		for (const auto& txt : textures)
			stream << txt;

		for (const auto& smp : samplers)
			stream << smp;
	}

	void Material::BindingData::LoadFromStream(LinaGX::Instance* lgx, IStream& stream)
	{
		int32 buffersSz = 0, texturesSz = 0, samplersSz = 0;
		stream >> buffersSz >> texturesSz >> samplersSz;

		buffers.resize(buffersSz);
		textures.resize(texturesSz);
		samplers.resize(samplersSz);

		for (int32 i = 0; i < buffersSz; i++)
		{
			buffers[i].LoadFromStream(lgx, stream);
		}

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
			For every binding we have in our descriptor set layout, we will create resources or at least reserve it.
			For UBOs, we create buffers for each descriptor in the set. (info fetched from shader layout info)
			For SSBO, we create 1 buffer.
			Buffers' sizes are again fetched from shader layout info.
			For shaders and samplers, we don't create anything as they are expected to exist.
			But we update the descriptor set with default textures and samplers, until they are set externally.
		 */
		const LinaGX::DescriptorSetDesc			desc = m_shader->GetMaterialSetDesc();
		const LinaGX::ShaderDescriptorSetLayout info = m_shader->GetMaterialSetInfo();
		m_bindingData.resize(desc.bindings.size());
		m_descriptorSet = m_lgx->CreateDescriptorSet(desc);

		uint32 bindingIndex = 0;
		for (const auto& b : desc.bindings)
		{
			auto& matBindingData = m_bindingData[bindingIndex];

			if (b.type == LinaGX::DescriptorType::UBO)
			{
				LinaGX::DescriptorUpdateBufferDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				matBindingData.buffers.resize(b.descriptorCount);
				for (int32 i = 0; i < b.descriptorCount; i++)
				{
					matBindingData.buffers[i].Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, static_cast<uint32>(info.bindings[bindingIndex].size), "Material UBO", true);
					update.buffers.push_back(matBindingData.buffers[i].GetGPUResource());
				}
				m_lgx->DescriptorUpdateBuffer(update);
			}
			else if (b.type == LinaGX::DescriptorType::SSBO)
			{
				LinaGX::DescriptorUpdateBufferDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				matBindingData.buffers.resize(1);
				matBindingData.buffers[0].Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, static_cast<uint32>(info.bindings[bindingIndex].size), "Material SSBO", true);
				update.buffers.push_back(matBindingData.buffers[0].GetGPUResource());
				m_lgx->DescriptorUpdateBuffer(update);
			}
			else if (b.type == LinaGX::DescriptorType::SeparateImage)
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				matBindingData.textures.resize(b.descriptorCount);

				for (int32 i = 0; i < b.descriptorCount; i++)
				{
					matBindingData.textures[i] = DEFAULT_TEXTURE_SID;
					update.textures.push_back(DEFAULT_TEXTURE_SID);
				}

				m_lgx->DescriptorUpdateImage(update);
			}
			else if (b.type == LinaGX::DescriptorType::SeparateSampler)
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				matBindingData.samplers.resize(b.descriptorCount);

				for (int32 i = 0; i < b.descriptorCount; i++)
				{
					matBindingData.samplers[i] = DEFAULT_SAMPLER_SID;
					update.samplers.push_back(DEFAULT_SAMPLER_SID);
				}

				m_lgx->DescriptorUpdateImage(update);
			}
			else if (b.type == LinaGX::DescriptorType::CombinedImageSampler)
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				matBindingData.textures.resize(b.descriptorCount);
				matBindingData.samplers.resize(b.descriptorCount);

				for (int32 i = 0; i < b.descriptorCount; i++)
				{
					matBindingData.textures[i] = DEFAULT_TEXTURE_SID;
					matBindingData.samplers[i] = DEFAULT_SAMPLER_SID;
					update.textures.push_back(DEFAULT_TEXTURE_SID);
					update.samplers.push_back(DEFAULT_SAMPLER_SID);
				}

				m_lgx->DescriptorUpdateImage(update);
			}

			bindingIndex++;
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
		SetShader(m_shaderSID);
	}

	void Material::DestroyBindingData()
	{
		if (!m_bindingData.empty())
		{
			m_lgx->DestroyDescriptorSet(m_descriptorSet);

			for (auto& b : m_bindingData)
			{
				for (auto& buf : b.buffers)
					buf.Destroy();
			}

			m_bindingData.clear();
		}
	}

} // namespace Lina
