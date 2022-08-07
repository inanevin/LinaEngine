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

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include "Utility/StringId.hpp"
#include "Math/Color.hpp"
#include "ECS/Registry.hpp"
#include "Core/CommonReflection.hpp"
#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Data/HashSet.hpp"
#include "Data/Serialization/SetSerialization.hpp"
#include <cereal/access.hpp>

namespace Lina
{
    class Application;
} // namespace Lina

namespace Lina::World
{
    LINA_CLASS("Level Settings")
    class Level : public Resources::IResource
    {
    public:
        Level(){};
        virtual ~Level(){};
        Level(const Level&);

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;
        void          SaveToFile(const String& path);
        void          AddResourceReference(TypeID tid, const String& path);
        void          RemoveResourceReference(TypeID tid, const String& path);

        const HashMap<TypeID, HashSet<String>>& GetResources()
        {
            return m_usedResources;
        }

        LINA_PROPERTY("Ambient", "Color", "", "", "Sky")
        Color m_ambientColor = Color(0);

    private:
        void Install();
        void Uninstall();

    private:
        friend class LevelManager;
        friend class cereal::access;

        ECS::Registry                    m_registry;
        HashMap<TypeID, HashSet<String>> m_usedResources;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_ambientColor, m_usedResources);
        }
    };
} // namespace Lina::World

#endif
