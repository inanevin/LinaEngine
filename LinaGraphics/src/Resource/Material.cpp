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

#include "Resource/Material.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Core/ResourceManager.hpp"
namespace Lina::Graphics
{
    Material::~Material()
    {
    }

    Resources::Resource* Material::LoadFromMemory(const IStream& stream)
    {
       // *this = Resources::LoadArchiveFromMemory<Material>(path, data, dataSize);
       // IResource::SetSID(path);
       // FindShader();
        return this;
    }

    Resources::Resource* Material::LoadFromFile(const String& path)
    {
        Serialization::LoadFromFile<Material>(path, *this);
        FindShader();
        return this;
    }

    void Material::FindShader()
    {
        auto* manager = Resources::ResourceManager::Get();

        if (manager->Exists<Shader>(m_shader.sid))
            m_shader.value = manager->GetResource<Shader>(m_shader.sid);
        else
            Event::EventSystem::Get()->Connect<Event::EResourceLoaded, &Material::OnResourceLoaded>(this);
    }

    void Material::OnResourceLoaded(const Event::EResourceLoaded& ev)
    {
        // Assign our shader if it's not loaded yet.
        if (ev.tid == GetTypeID<Shader>())
        {
            auto* manager = Resources::ResourceManager::Get();
            if (ev.sid == m_shader.sid)
            {
                m_shader.value = manager->GetResource<Shader>(m_shader.sid);
                Event::EventSystem::Get()->Disconnect<Event::EResourceLoaded>(this);
            }
        }
    }

    void Material::SetShader(Shader* shader)
    {
        m_shader.sid   = shader->GetSID();
        m_shader.value = shader;
    }

} // namespace Lina::Graphics
