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
#include "Core/ResourceStorage.hpp"

namespace Lina::Graphics
{
    Material::~Material()
    {
    }

    void* Material::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        *this = Resources::LoadArchiveFromMemory<Material>(path, data, dataSize);
        IResource::SetSID(path);
        FindShader();
        return static_cast<void*>(this);
    }

    void* Material::LoadFromFile(const String& path)
    {
        *this = Resources::LoadArchiveFromFile<Material>(path);
        IResource::SetSID(path);
        FindShader();
        return static_cast<void*>(this);
    }

    void Material::FindShader()
    {
        auto* storage = Resources::ResourceStorage::Get();

        if (storage->Exists(m_shader.tid, m_shader.sid))
            m_shader.value = storage->GetResource<Shader>(m_shader.sid);
        else
            Event::EventSystem::Get()->Connect<Event::EResourceLoaded, &Material::OnResourceLoaded>(this);
    }

    void Material::OnResourceLoaded(const Event::EResourceLoaded& ev)
    {
        // Assign our shader if it's not loaded yet.
        if (ev.tid == GetTypeID<Shader>())
        {
            auto* storage = Resources::ResourceStorage::Get();
            if (ev.sid == m_shader.sid)
            {
                m_shader.value = storage->GetResource<Shader>(m_shader.sid);
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
