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

#include "Core/World.hpp"
#include "Core/LevelManager.hpp"
#include "Core/Level.hpp"
#include "Core/Entity.hpp"
#include "Core/Component.hpp"
#include "Data/Serialization/QueueSerialization.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Memory/Memory.hpp"

namespace Lina::World
{
    void EntityWorld::Initialize()
    {

        if (m_initialized)
            return;
        m_initialized = true;

        int stackCount = Math::FloorToInt(static_cast<float>(m_nextID) / static_cast<float>(ENTITY_STACK_SIZE)) + 1;

        for (int i = 0; i < stackCount; i++)
        {
            EntityStack stack;
            stack.ptr = new Entity[ENTITY_STACK_SIZE];
            m_stacks.push_back(stack);
        }
    }

    void EntityWorld::Shutdown()
    {
        DestroyWorld();
    }

    EntityWorld* EntityWorld::Get()
    {
        auto* currentLevel = LevelManager::Get()->GetCurrentLevel();

        if (currentLevel != nullptr)
            return &currentLevel->GetWorld();

        return nullptr;
    }

    void EntityWorld::DestroyWorld()
    {
        for (auto& stack : m_stacks)
            delete[] stack.ptr;
       // for (uint32 i = 0; i < m_nextID; i++)
       // {
       //     if (m_entities[i] != nullptr)
       //         delete m_entities[i];
       // 
       //     m_entities[i] = nullptr;
       // }
       // 
       // for (auto& [tid, cache] : m_componentCaches)
       //     cache->Destroy();
       // 
       // m_componentCaches.clear();
       // m_nextID       = 0;
       // m_availableIDs = Queue<uint32>();
    }

    Entity* EntityWorld::GetEntity(uint32 id)
    {
        return m_entities[id];
    }

    Entity* EntityWorld::GetEntity(const String& name)
    {
        for (uint32 i = 0; i < m_nextID; i++)
        {
            if (m_entities[i] != nullptr)
            {
                if (m_entities[i]->m_name.compare(name) == 0)
                    return m_entities[i];
            }
        }

        return nullptr;
    }

    void EntityWorld::CopyFrom(EntityWorld& world)
    {
        DestroyWorld();
        for (uint32 i = 0; i < world.m_nextID; i++)
        {
            if (world.m_entities[i] != nullptr)
            {
                Entity* e     = new Entity(*world.m_entities[i]);
                m_entities[i] = e;
            }
        }

        m_nextID       = world.m_nextID;
        m_availableIDs = world.m_availableIDs;

        for (auto& [tid, cache] : world.m_componentCaches)
        {
            ComponentCacheBase* c  = cache->CopyCreate();
            m_componentCaches[tid] = c;
        }
    }

    Entity* EntityWorld::CreateEntity(const String& name)
    {
        Entity* e  = new Entity();
        e->m_world = this;
        e->m_name  = name;

        uint32 id = 0;

        if (!m_availableIDs.empty())
        {
            id = m_availableIDs.back();
            m_availableIDs.pop();
        }
        else
            id = m_nextID++;

        m_entities[id] = e;
        e->m_id        = id;

        return e;
    }

    void EntityWorld::DestroyEntity(Entity* e)
    {
        if (e->m_parent != ENTITY_NULL)
            m_entities[e->m_parent]->RemoveChild(e);

        DestroyEntityData(e);
    }

    void EntityWorld::DestroyEntityData(Entity* e)
    {
        for (auto child : e->m_children)
            DestroyEntityData(m_entities[child]);

        for (auto& [tid, cache] : m_componentCaches)
            cache->OnEntityDestroyed(e);

        const uint32 id = e->m_id;
        delete m_entities[id];
        m_availableIDs.push(id);
        m_entities[id] = nullptr;
    }

    void EntityWorld::SaveToArchive(cereal::PortableBinaryOutputArchive& archive)
    {
        archive(m_nextID);
        archive(m_availableIDs);

        Vector<Entity> entities;
        for (uint32 i = 0; i < m_nextID; i++)
        {
            if (m_entities[i] != nullptr)
                entities.push_back(*m_entities[i]);
        }

        archive(entities);

        Vector<TypeID> tids;
        for (auto& [tid, cache] : m_componentCaches)
            tids.push_back(tid);

        archive(tids);

        for (auto& [tid, cache] : m_componentCaches)
            cache->SaveToArchive(archive);
    }

    void EntityWorld::LoadFromArchive(cereal::PortableBinaryInputArchive& archive)
    {
        Initialize();
        archive(m_nextID);
        archive(m_availableIDs);

        Vector<Entity> entities;
        archive(entities);

        for (int i = 0; i < entities.size(); i++)
        {
            Entity* e           = new Entity(entities[i]);
            e->m_world          = this;
            m_entities[e->m_id] = e;
        }

        entities.clear();

        Vector<TypeID> tids;
        archive(tids);

        for (uint32 i = 0; i < tids.size(); i++)
        {
            const Reflection::MetaType& type = Reflection::Resolve(tids[i]);
            if (type.createCompCacheFunc)
            {
                ComponentCacheBase* cache = static_cast<ComponentCacheBase*>(type.createCompCacheFunc());
                cache->LoadFromArchive(archive, m_entities);
                m_componentCaches[tids[i]] = cache;
            }
            else
                LINA_ERR("Could not create a cache for the type ID {0}, did your component change?", tids[i]);
        }
    }

} // namespace Lina::World
