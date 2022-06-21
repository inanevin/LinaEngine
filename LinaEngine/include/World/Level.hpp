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
        void          AddResourceReference(TypeID tid, StringIDType sid);

        const HashMap<TypeID, HashSet<StringIDType>>& GetResources()
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

        ECS::Registry                          m_registry;
        HashMap<TypeID, HashSet<StringIDType>> m_usedResources;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_ambientColor, m_usedResources);
        }
    };
} // namespace Lina::World

#endif
