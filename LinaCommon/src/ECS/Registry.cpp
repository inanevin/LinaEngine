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

#include "ECS/Registry.hpp"

#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/EntityEvents.hpp"
#include "Log/Log.hpp"

namespace Lina::ECS
{
    Registry* Registry::s_ecs = nullptr;

    void Registry::Initialize()
    {
        LINA_TRACE("[Initialization] -> ECS Registry ({0})", typeid(*this).name());
        on_construct<EntityDataComponent>().connect<&Registry::OnEntityDataComponentAdded>(this);
    }

    void Registry::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> ECS Registry ({0})", typeid(*this).name());
        on_construct<EntityDataComponent>().disconnect(this);
    }

    void Registry::SerializeComponentsInRegistry(cereal::PortableBinaryOutputArchive& archive)
    {

        auto& snapshot = entt::snapshot{*this};
        snapshot.entities(archive);

        //for (auto& func : m_serializeFunctions)
        //{
        //    func.second.first(snapshot, archive);
        //}
    }

    void Registry::DeserializeComponentsInRegistry(cereal::PortableBinaryInputArchive& archive)
    {
        auto& loader = entt::snapshot_loader{*this};
        loader.entities(archive);
        // for (auto& func : m_serializeFunctions)
        // {
        //     func.second.second(loader, archive);
        // }
    }

    void Registry::OnEntityDataComponentAdded(entt::registry& reg, entt::entity ent)
    {
        auto& data = reg.get<EntityDataComponent>(ent);
        data.m_ecs = this;
    }

    void Registry::AddChildToEntity(Entity parent, Entity child)
    {

        if (parent == child)
            return;

        EntityDataComponent& childData  = get<EntityDataComponent>(child);
        EntityDataComponent& parentData = get<EntityDataComponent>(parent);

        const Vector3    childGlobalPos   = childData.GetLocation();
        const Quaternion childGlobalRot   = childData.GetRotation();
        const Vector3    childGlobalScale = childData.GetScale();
        if (parentData.m_parent == child || childData.m_parent == parent)
            return;

        if (childData.m_parent != entt::null)
        {
            RemoveChildFromEntity(childData.m_parent, child);
        }

        parentData.m_children.emplace(child);
        childData.m_parent = parent;

        // Adding a child to an entity does not change any transformation
        // Due to this, child's local values will be according to the previous parent if exists.
        // So we update the global transformation, which makes sure local transformations are set according to
        // the current parent.
        childData.SetLocation(childGlobalPos);
        childData.SetRotation(childGlobalRot);
        childData.SetScale(childGlobalScale);
    }

    void Registry::DestroyAllChildren(Entity parent)
    {
        EntityDataComponent* data = try_get<EntityDataComponent>(parent);

        if (data == nullptr)
            return;

        int                        counter  = 0;
        std::set<Entity>           children = data->m_children;
        std::set<Entity>::iterator it;
        for (it = children.begin(); it != children.end(); ++it)
        {
            DestroyEntity(*it);
            counter++;
        }
        data->m_children.clear();
    }

    void Registry::RemoveChildFromEntity(Entity parent, Entity child)
    {
        std::set<Entity>& children = get<EntityDataComponent>(parent).m_children;
        if (children.find(child) != children.end())
        {
            children.erase(child);
        }

        get<EntityDataComponent>(child).m_parent = entt::null;
    }

    void Registry::RemoveFromParent(Entity child)
    {
        Entity parent = get<EntityDataComponent>(child).m_parent;

        if (parent != entt::null)
            RemoveChildFromEntity(parent, child);
    }

    void Registry::CloneEntity(Entity from, Entity to)
    {
        for (auto& pool : storage())
        {
            // if (pool.second.contains(from))
            //     m_cloneComponentFunctions[pool.first](from, to);
        }
    }

    const std::set<Entity>& Registry::GetChildren(Entity parent)
    {
        return get<EntityDataComponent>(parent).m_children;
    }

    Entity Registry::CreateEntity(const std::string& name)
    {
        entt::entity ent = create();
        emplace<EntityDataComponent>(ent, EntityDataComponent(true, true, name));
        emplace<PhysicsComponent>(ent, PhysicsComponent());
        return ent;
    }

    Entity Registry::CreateEntity(Entity source, bool attachParent)
    {
        EntityDataComponent sourceData = get<EntityDataComponent>(source);

        // Build the entity.
        Entity copy = create();

        // Copy entity components to newly created one
        CloneEntity(source, copy);

        // EntityDataComponent& copyData = get<EntityDataComponent>(copy);
        get<EntityDataComponent>(copy).m_parent = entt::null;
        get<EntityDataComponent>(copy).m_children.clear();

        for (Entity child : sourceData.m_children)
        {
            Entity               copyChild     = CreateEntity(child, false);
            EntityDataComponent& copyChildData = get<EntityDataComponent>(copyChild);
            copyChildData.m_parent             = copy;
            get<EntityDataComponent>(copy).m_children.emplace(copyChild);
        }

        if (attachParent && sourceData.m_parent != entt::null)
            AddChildToEntity(sourceData.m_parent, copy);

        return copy;
    }

    Entity Registry::GetEntity(const std::string& name)
    {
        auto singleView = view<EntityDataComponent>();

        for (auto entity : singleView)
        {
            if (singleView.get<EntityDataComponent>(entity).m_name.compare(name) == 0)
                return entity;
        }

        LINA_WARN("Entity with the name {0} could not be found, returning null entity.", name);
        return entt::null;
    }

    void Registry::DestroyEntity(Entity entity, bool isRoot)
    {
        std::set<Entity> toErase;
        for (Entity child : get<EntityDataComponent>(entity).m_children)
        {
            toErase.emplace(child);
            DestroyEntity(child, false);
        }

        for (Entity child : toErase)
            get<EntityDataComponent>(entity).m_children.erase(child);

        if (isRoot)
            RemoveFromParent(entity);

        destroy(entity);
    }

    void Registry::SetEntityEnabled(Entity entity, bool isEnabled)
    {
        auto& data = get<EntityDataComponent>(entity);
        if (data.m_isEnabled == isEnabled)
            return;

        data.m_isEnabled = isEnabled;
        Event::EventSystem::Get()->Trigger<Event::EEntityEnabledChanged>(Event::EEntityEnabledChanged{entity, isEnabled});

        for (auto child : data.m_children)
        {
            auto& childData = get<EntityDataComponent>(child);

            if (!isEnabled)
            {
                childData.m_wasPreviouslyEnabled = childData.m_isEnabled;
                SetEntityEnabled(child, false);
            }
            else
            {
                if (childData.m_wasPreviouslyEnabled)
                    SetEntityEnabled(child, true);
            }
        }
    }

} // namespace Lina::ECS
