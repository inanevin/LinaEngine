/*
Class: ECSSystem

Defines Registry wrapper, base ECS system class that defines functions for updating entity components
as well as an ECS Systems class responsible for iterating & calling update functions of containted systems.

Timestamp: 4/8/2019 5:28:34 PM
*/

#pragma once

#ifndef Registry_HPP
#define Registry_HPP

#include "Core/CommonECS.hpp"
#include "Data/Map.hpp"
#include <cereal/access.hpp>
#include <entt/config/config.h>
#include <entt/entity/snapshot.hpp>

namespace Lina
{
    namespace World
    {
        class Level;
    }
} // namespace Lina

namespace Lina::ECS
{

    class Registry : public entt::registry
    {
    public:
        Registry()          = default;
        virtual ~Registry() = default;

        inline static Registry* Get()
        {
            return s_ecs;
        }

        void               SerializeComponentsInRegistry(cereal::PortableBinaryOutputArchive& archive);
        void               DeserializeComponentsInRegistry(cereal::PortableBinaryInputArchive& archive);
        void               AddChildToEntity(Entity parent, Entity child);
        void               DestroyAllChildren(Entity parent);
        void               RemoveChildFromEntity(Entity parent, Entity child);
        void               RemoveFromParent(Entity child);
        void               CloneEntity(Entity from, Entity to);
        void               DestroyEntity(Entity entity, bool isRoot = true);
        void               SetEntityEnabled(Entity entity, bool isEnabled);
        Entity             CreateEntity(const String& name);
        Entity             CreateEntity(Entity copy, bool attachParent = true);
        Entity             GetEntity(const String& name);
        const Set<Entity>& GetChildren(Entity parent);

    private:
        friend class World::Level;
        static Registry* s_ecs;
    };
} // namespace Lina::ECS

#endif
