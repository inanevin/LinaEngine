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

	Material::Material(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : Resource(rm, isUserManaged, path, sid, GetTypeID<Material>())
	{
		m_gfxManager = rm->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_lgx		 = m_gfxManager->GetLGX();
	}

	Material::~Material()
	{
		if (m_data != nullptr)
			delete[] m_data;
	}

	void Material::SetShader(StringID sid)
	{
		m_shader = sid;

		if (m_data != nullptr)
			delete[] m_data;

		Shader* shader = m_resourceManager->GetResource<Shader>(m_shader);
		LINA_ASSERT(shader != nullptr, "Shader is null!");

		if (m_descriptorSetCreated)
		{
			m_descriptorSetCreated = false;
			m_lgx->DestroyDescriptorSet(m_descriptorSet);
		}

		const LinaGX::DescriptorSetDesc			desc = shader->GetMaterialSetDesc();
		const LinaGX::ShaderDescriptorSetLayout info = shader->GetMaterialSetInfo();

		m_descriptorSet = m_lgx->CreateDescriptorSet(desc);

		LinaGX::DescriptorType type;

		/*
			For every binding we have in our descriptor set layout, we will create resources or at least reserve it.
			For UBOs, we create buffers for each descriptor in the set. (info fetched from shader layout info)
			For SSBO, we create 1 buffer.
			Buffers' sizes are again fetched from shader layout info.
			For shaders and samplers, we don't create anything as they are expected to exist.
			But we update the descriptor set with default textures and samplers, until they are set externally.
		 */

		uint32 bindingIndex = 0;
		m_bindingData.resize(desc.bindings.size());
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
					matBindingData.buffers[i].Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, info.bindings[bindingIndex].size, "Material Buffer", true);
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
				matBindingData.buffers[0].Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, info.bindings[bindingIndex].size, false);
				update.buffers.push_back(matBindingData.buffers[0].GetGPUResource());
				m_lgx->DescriptorUpdateBuffer(update);
			}
			else if (b.type == LinaGX::DescriptorType::SeparateImage)
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				for (int32 i = 0; i < b.descriptorCount; i++)
					update.textures.push_back(DEFAULT_TEXTURE_SID);

				m_lgx->DescriptorUpdateImage(update);
			}
			else if (b.type == LinaGX::DescriptorType::SeparateSampler)
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				for (int32 i = 0; i < b.descriptorCount; i++)
					update.samplers.push_back(DEFAULT_SAMPLER_SID);

				m_lgx->DescriptorUpdateImage(update);
			}
			else if (b.type == LinaGX::DescriptorType::CombinedImageSampler)
			{
				LinaGX::DescriptorUpdateImageDesc update = {
					.setHandle = m_descriptorSet,
					.binding   = bindingIndex,
				};

				for (int32 i = 0; i < b.descriptorCount; i++)
				{
					update.textures.push_back(DEFAULT_TEXTURE_SID);
					update.samplers.push_back(DEFAULT_SAMPLER_SID);
				}

				m_lgx->DescriptorUpdateImage(update);
			}

			bindingIndex++;
		}

		if (shader)
		{
			// m_uboDefinition = shader->GetMaterialUBO();
			//
			// m_totalDataSize = 0;
			// for (const auto& mem : m_uboDefinition.members)
			// 	m_totalDataSize += mem.alignment;
			//
			// if (m_totalDataSize == 0)
			// 	return;
			//
			// m_data = new uint8[m_totalDataSize];
		}
	}

	void Material::GetDataBlob(uint8* ptr, size_t size)
	{
		ptr = new uint8[m_totalDataSize];
		MEMCPY(ptr, m_data, m_totalDataSize);
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
		stream << m_shader << m_totalDataSize;

		if (m_totalDataSize != 0)
			stream.WriteEndianSafe(m_data, m_totalDataSize);
	}

	void Material::LoadFromStream(IStream& stream)
	{
		stream >> m_shader >> m_totalDataSize;

		if (m_totalDataSize != 0)
		{
			m_data = new uint8[m_totalDataSize];
			stream.ReadEndianSafe(m_data, m_totalDataSize);
		}
	}

	void Material::BatchLoaded()
	{
		SetShader(m_shader);
	}

} // namespace Lina
