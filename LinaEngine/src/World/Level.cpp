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

#include "World/Level.hpp"

#include "Core/Application.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Registry.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resources/ResourceStorage.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include <stdio.h>

namespace Lina::World
{
    void Level::ExportLevel(const std::string& path, const std::string& name)
    {
        const std::string finalPath = path + "/" + name + ".linalevel";

        // Delete if exists.
        if (Utility::FileExists(finalPath))
            Utility::DeleteFileInPath(finalPath);

        ECS::Registry* registry = ECS::Registry::Get();
        {

            std::ofstream levelDataStream(finalPath, std::ios::binary);
            {
                cereal::PortableBinaryOutputArchive oarchive(levelDataStream);
                oarchive(m_levelData);
                registry->SerializeComponentsInRegistry(oarchive);
            }
        }
    }

    void Level::ImportLevel(const std::string& path, const std::string& name)
    {
        ECS::Registry* registry = ECS::Registry::Get();

        {
            std::ifstream levelDataStream(path + "/" + name + ".linalevel", std::ios::binary);
            {
                cereal::PortableBinaryInputArchive iarchive(levelDataStream);
                iarchive(m_levelData);
                registry->clear();
                registry->DeserializeComponentsInRegistry(iarchive);
            }
        }
    }

    bool Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
    {
        return true;
    }

    void Level::Uninstall()
    {

    }

} // namespace Lina::World
