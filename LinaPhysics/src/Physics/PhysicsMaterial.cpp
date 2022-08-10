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

#include "Physics/PhysicsMaterial.hpp"
#include "Core/ResourceCommon.hpp"
#include "Log/Log.hpp"
#include "Core/ResourceStorage.hpp"
#include <fstream>

namespace Lina::Physics
{
    PhysicsMaterial* PhysicsMaterial::CreatePhysicsMaterial(const String& savePath, float staticFriction, float dynamicFriction, float restitution)
    {
        auto* storage = Resources::ResourceStorage::Get();

        if (storage->Exists<PhysicsMaterial>(savePath))
        {
            LINA_WARN("Physics Material already exists with the given path. {0}", savePath);
            return storage->GetResource<PhysicsMaterial>(savePath);
        }

        PhysicsMaterial* mat = new PhysicsMaterial();
        mat->SetSID(savePath);
        mat->m_staticFriction = staticFriction;
        mat->m_dynamicFriction = dynamicFriction;
        mat->m_restitution = restitution;
        storage->Add(static_cast<void*>(mat), GetTypeID<PhysicsMaterial>(), mat->m_sid);
        Resources::SaveArchiveToFile<PhysicsMaterial>(savePath, *mat);
        return mat;
    }
    void* PhysicsMaterial::LoadFromFile(const String& path)
    {
        LINA_TRACE("Physics Loader - File] -> Loading: {0}", path);
        *this = Resources::LoadArchiveFromFile<PhysicsMaterial>(path);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }

    void* PhysicsMaterial::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        LINA_TRACE("[Physics Loader - Memory] -> Loading: {0}", path);
        *this = Resources::LoadArchiveFromMemory<PhysicsMaterial>(path, data, dataSize);
        IResource::SetSID(path);
        return static_cast<void*>(this);
    }
} // namespace Lina::Physics
