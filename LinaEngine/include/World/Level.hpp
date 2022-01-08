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
#include "ECS/Registry.hpp"
#include "Resources/ResourceHandle.hpp"
#include "Rendering/Material.hpp"
#include "Core/CommonReflection.hpp"
#include <cereal/access.hpp>
#include <string>

namespace Lina
{
    class Application;
} // namespace Lina

namespace Lina::World
{
    LINA_CLASS("Level Settings")
    class Level
    {
    public:
        Level() = default;
        virtual ~Level()
        {
        }

        inline static Level* GetCurrent()
        {
            return s_currentLevel;
        }

        virtual void Install();

        void SaveToFile(const std::string& path);
        void InstallFromFile(const std::string& path);

        LINA_PROPERTY("Skybox", "Material")
        Resources::ResourceHandle<Graphics::Material> m_skyboxMaterial;

        LINA_PROPERTY("Ambient", "Color")
        Color m_ambientColor = Color(0);

    protected:
        ECS::Registry m_registry;

    private:
        void Uninstall();
        void SetupData();

    private:
        friend class Application;
        friend class cereal::access;
        static Level* s_currentLevel;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_skyboxMaterial, m_ambientColor);
        }
    };
} // namespace Lina::World

#endif
