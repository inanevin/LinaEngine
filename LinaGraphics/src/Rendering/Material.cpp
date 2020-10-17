/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Material
Timestamp: 5/20/2020 9:09:53 PM

*/

#include "Rendering/Material.hpp"

namespace LinaEngine::Graphics
{
	void Material::SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode)
	{
		if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
		{
			m_sampler2Ds[textureName].m_boundTexture = texture;
			m_sampler2Ds[textureName].m_bindMode = bindMode;
			m_sampler2Ds[textureName].m_isActive = texture == nullptr ? false : true;
		}
		else
		{
			LINA_CORE_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
			return;
		}
	}
	void Material::RemoveTexture(const std::string& textureName)
	{
		if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
		{
			m_sampler2Ds[textureName].m_boundTexture = nullptr;
			m_sampler2Ds[textureName].m_isActive = false;
		}
		else
		{
			LINA_CORE_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
			return;
		}
	}
	Texture* Material::GetTexture(const std::string& name)
	{
		if (!(m_sampler2Ds.find(name) == m_sampler2Ds.end()))
			return m_sampler2Ds[name].m_boundTexture;
		else
		{
			LINA_CORE_WARN("This material doesn't support texture slot with the name {0}, returning null pointer", name);
			return nullptr;
		}
	}
}