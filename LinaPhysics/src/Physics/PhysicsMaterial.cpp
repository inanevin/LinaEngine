/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Log/Log.hpp"

#include <cereal/archives/portable_binary.hpp>
#include <fstream>

namespace Lina::Physics
{

    std::map<StringIDType, PhysicsMaterial> PhysicsMaterial::s_loadedMaterials;

    PhysicsMaterial& PhysicsMaterial::CreateMaterial(const std::string& name, float staticFriction, float dynamicFriction, float restitution)
    {
        StringIDType sid = StringID(name.c_str()).value();
        if (MaterialExists(sid))
        {
            LINA_WARN("Physics Material already exists with the given name. {0}", name);
            return s_loadedMaterials[sid];
        }

        PhysicsMaterial& mat = s_loadedMaterials[sid];
        mat.m_materialID     = sid;
        mat.m_path           = name;
        return s_loadedMaterials[sid];
    }

    PhysicsMaterial& PhysicsMaterial::LoadMaterialFromFile(const std::string& path)
    {
        StringIDType     sid = StringID(path.c_str()).value();
        PhysicsMaterial& mat = s_loadedMaterials[sid];
        LoadMaterialData(mat, path);
        mat.m_materialID = sid;
        mat.m_path       = path;
        return mat;
    }

    PhysicsMaterial& PhysicsMaterial::LoadMaterialFromMemory(const std::string& path, unsigned char* data, size_t dataSize)
    {
        StringIDType     sid = StringID(path.c_str()).value();
        PhysicsMaterial& mat = s_loadedMaterials[sid];
        mat.m_materialID     = sid;
        mat.m_path           = path;
        return mat;
    }

    PhysicsMaterial& PhysicsMaterial::GetMaterial(StringIDType id)
    {
        bool materialExists = MaterialExists(id);
        LINA_ASSERT(materialExists, "Physics Material does not exist!");
        return s_loadedMaterials[id];
    }
    PhysicsMaterial& PhysicsMaterial::GetMaterial(const std::string& path)
    {
        return GetMaterial(StringID(path.c_str()).value());
    }

    bool PhysicsMaterial::MaterialExists(StringIDType id)
    {
        if (id < 0)
            return false;
        return !(s_loadedMaterials.find(id) == s_loadedMaterials.end());
    }

    bool PhysicsMaterial::MaterialExists(const std::string& path)
    {
        return MaterialExists(StringID(path.c_str()).value());
    }

    void PhysicsMaterial::UnloadMaterialResource(StringIDType id)
    {
        if (!MaterialExists(id))
        {
            LINA_WARN("Physics Material not found! Aborting... ");
            return;
        }

        s_loadedMaterials.erase(id);
    }

    void PhysicsMaterial::LoadMaterialData(PhysicsMaterial& mat, const std::string& path)
    {
        std::ifstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(mat);
        }
    }

    void PhysicsMaterial::SaveMaterialData(const PhysicsMaterial& mat, const std::string& path)
    {
        std::ofstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryOutputArchive oarchive(stream);
            oarchive(mat);
        }
    }

    void PhysicsMaterial::UnloadAll()
    {
        s_loadedMaterials.clear();
    }
} // namespace Lina::Physics
