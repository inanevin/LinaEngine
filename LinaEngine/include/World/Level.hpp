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

/*
Class: Level

Level class defines functions required for installing, loading and processing a level.
Derived classes contains custom logic for creating entity systems and processing the gameplay
logic.

Timestamp: 5/6/2019 5:10:23 PM
*/

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include "EventSystem/MainLoopEvents.hpp"
#include "Math/Color.hpp"
#include "Resources/ResourceHandle.hpp"

#include <cereal/archives/portable_binary.hpp>
#include <string>

namespace Lina
{
    namespace ECS
    {
        class Registry;
    }
} // namespace Lina

namespace Lina::World
{
    struct LevelData
    {
        Resources::ResourceHandle<Graphics::Material> m_skyboxMaterial;
        Color       m_ambientColor          = Color(0);

        template <class Archive> void serialize(Archive& archive)
        {
            archive(m_skyboxMaterial, m_ambientColor);
        }
    };

    class Level
    {
    public:
        Level() = default;
        virtual ~Level() = default;

        virtual bool Install(bool loadFromFile, const std::string& path, const std::string& levelName);
        virtual void Uninstall();
        virtual void Initialize(){};
        void         ImportLevel(const std::string& path, const std::string& name);
        void         ExportLevel(const std::string& path, const std::string& name);

        void       SetSkyboxMaterial();
        LevelData& GetLevelData()
        {
            return m_levelData;
        }

    private:
        void LoadLevelResources();

    private:
        LevelData m_levelData;
    };
} // namespace Lina::World

#endif
