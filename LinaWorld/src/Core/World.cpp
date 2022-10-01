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
#include "Serialization/QueueSerialization.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Memory/MemoryManager.hpp"

namespace Lina::World
{
#define ENTITY_VEC_SIZE_CHUNK 2000

    void EntityWorld::Initialize()
    {
        if (m_initialized)
            return;

        m_initialized = true;

        if (m_entities.empty())
        {
            m_entities.resize(ENTITY_VEC_SIZE_CHUNK, nullptr);

            if (!Memory::MemoryManager::Get()->PoolBlockExists<World::Entity>())
                Memory::MemoryManager::Get()->CreatePoolBlock<World::Entity>(ENTITY_CHUNK_COUNT, "Entity");
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
        for (uint32 i = 0; i < m_nextID; i++)
        {
            Entity* e = m_entities[i];
            if (e != nullptr)
            {
                const uint32 index = e->m_allocPoolIndex;
                e->~Entity();
                Memory::MemoryManager::Get()->FreeFromPoolBlock<World::Entity>(e, index);
            }
        }

        for (auto& [tid, cache] : m_componentCaches)
            cache->Destroy();

        m_entities.clear();
        m_componentCaches.clear();
        m_nextID       = 0;
        m_availableIDs = Queue<uint32>();
    }

    Entity* EntityWorld::GetEntity(uint32 id)
    {
        return m_entities[id];
    }

    Entity* EntityWorld::GetEntity(const String& name)
    {
        for (uint32 i = 0; i < m_nextID; i++)
        {
            Entity* e = m_entities[i];
            if (e != nullptr)
            {
                if (e->m_name.compare(name) == 0)
                    return e;
            }
        }

        return nullptr;
    }

    void EntityWorld::CopyFrom(EntityWorld& world)
    {
        DestroyWorld();
        m_entities.resize(world.m_entities.size(), nullptr);

        for (uint32 i = 0; i < world.m_nextID; i++)
        {
            if (world.m_entities[i] != nullptr)
            {
                Entity* e     = Memory::MemoryManager::Get()->GetFromPoolBlock<World::Entity>();
                *e            = *world.m_entities[i];
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
        Entity* e = Memory::MemoryManager::Get()->GetFromPoolBlock<World::Entity>();

        uint32 id = 0;
        if (!m_availableIDs.empty())
        {
            const uint32 popped = m_availableIDs.front();
            id                  = popped;
            m_availableIDs.pop();
        }
        else
        {
            id = m_nextID;
            m_nextID++;
        }

        if (id == ENTITY_MAX)
        {
            LINA_ERR("Reached maximum number of entities!");
            LINA_ASSERT(false, "");
            return nullptr;
        }

        if (m_entities.size() <= id)
            m_entities.resize(m_entities.size() + ENTITY_VEC_SIZE_CHUNK, nullptr);

        m_entities[id] = e;

        e->m_id   = id;
        e->m_name = name;
        return e;
    }

    void EntityWorld::DestroyEntity(Entity* e)
    {
        if (e->m_parentID != ENTITY_NULL)
            e->m_parent->RemoveChild(e);

        DestroyEntityData(e);
    }

    void EntityWorld::DestroyEntityData(Entity* e)
    {
        for (auto child : e->m_children)
            DestroyEntityData(child);

        for (auto& [tid, cache] : m_componentCaches)
            cache->OnEntityDestroyed(e);

        const uint32 id    = e->m_id;
        const uint32 index = e->m_allocPoolIndex;
        m_entities[id]     = nullptr;
        m_availableIDs.push(id);
        e->~Entity();
        Memory::MemoryManager::Get()->FreeFromPoolBlock<World::Entity>(e, index);
    }


} // namespace Lina::World
