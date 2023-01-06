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

#ifndef World_HPP
#define World_HPP

#include "Serialization/VectorSerialization.hpp"
#include "Entity.hpp"
#include "ComponentCache.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Data/FixedList.hpp"

namespace Lina
{
    namespace Graphics
    {
        class CameraComponent;
    }
} // namespace Lina
namespace Lina::World
{
    class Entity;
    class Component;

    // Actual game state
    class EntityWorld
    {
    public:
        EntityWorld()  = default;
        ~EntityWorld() = default;

    public:
        /// <summary>
        /// Returns the current installed level's world.
        /// </summary>
        /// <returns></returns>
        static inline EntityWorld* GetWorld()
        {
            return s_levelWorld;
        }

        Entity* GetEntity(uint32 id);
        Entity* GetEntity(const String& name);
        Entity* CreateEntity(const String& name);
        void    DestroyEntity(Entity* e);

        inline uint32 GetID()
        {
            return m_worldID;
        }

        inline void SetActiveCamera(Graphics::CameraComponent* cam)
        {
            m_activeCamera = cam;
        }

        inline Graphics::CameraComponent* GetActiveCamera()
        {
            return m_activeCamera;
        }

        template <typename T> ComponentCache<T>* Cache()
        {
            const TypeID       tid   = GetTypeID<T>();
            ComponentCache<T>* cache = nullptr;
            const auto&        it    = m_componentCaches.find(tid);
            if (it == m_componentCaches.end())
            {
                cache                  = new ComponentCache<T>();
                m_componentCaches[tid] = cache;
            }
            else
                cache = static_cast<ComponentCache<T>*>(it->second);

            return cache;
        }

        template <typename T> T** View(uint32* maxSize)
        {
            auto* cache = Cache<T>();
            *maxSize    = cache->m_nextID;
            return cache->m_components.data();
        }

        template <typename T> T* GetComponent(Entity* e)
        {
            return Cache<T>()->GetComponent(e);
        }

        template <typename T> T* AddComponent(Entity* e, const T& t)
        {
            T* comp = Cache<T>()->AddComponent(e, t);
            *comp   = t;
            return comp;
        }

        template <typename T> T* AddComponent(Entity* e)
        {
            return Cache<T>()->AddComponent(e);
        }

        template <typename T> void RemoveComponent(Entity* e)
        {
            Cache<T>()->DestroyComponent(e);

            if (Cache<T>()->GetComponent(e) == m_activeCamera)
                m_activeCamera = nullptr;
        }

        template <typename T> void Load(T& archive)
        {
            uint32 entitiesSize = 0;
            archive(m_nextID);
            archive(m_availableIDs);
            archive(entitiesSize);
            m_entities.resize(entitiesSize, nullptr);

            Vector<Entity> entities;
            archive(entities);

            if (!entities.empty())
            {
                if (!Memory::MemoryManager::Get()->PoolBlockExists<World::Entity>())
                    Memory::MemoryManager::Get()->CreatePoolBlock<World::Entity>(ENTITY_CHUNK_COUNT, "Entity");
            }

            for (int i = 0; i < entities.size(); i++)
            {
                const uint32 id = entities[i].m_id;
                Entity*      e  = Memory::MemoryManager::Get()->GetFromPoolBlock<World::Entity>();
                *e              = entities[i];
                e->m_id         = id;
                e->m_world      = this;
                m_entities[id]  = e;
            }

            for (auto e : m_entities)
            {
                if (e != nullptr)
                {
                    auto& childrenIDs = e->m_childrenID;

                    for (auto& childID : childrenIDs)
                        e->m_children.insert(m_entities[childID]);

                    if (e->m_parentID != ENTITY_NULL)
                        e->m_parent = m_entities[e->m_parentID];
                }
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
                    cache->LoadFromArchive(archive, m_entities.data());
                    m_componentCaches[tids[i]] = cache;
                }
                else
                    LINA_ERR("Could not create a cache for the type ID {0}, did your component change?", tids[i]);
            }
        }

        template <typename T> void Save(T& archive)
        {
            const uint32 entitiesSize = static_cast<uint32>(m_entities.size());
            archive(m_nextID);
            archive(m_availableIDs);
            archive(entitiesSize);

            Vector<Entity> entities;
            for (uint32 i = 0; i < m_nextID; i++)
            {
                Entity* e = m_entities[i];
                if (e != nullptr)
                    entities.push_back(*e);
            }

            archive(entities);
            entities.clear();

            Vector<TypeID> tids;
            for (auto& [tid, cache] : m_componentCaches)
                tids.push_back(tid);

            archive(tids);

            for (auto& [tid, cache] : m_componentCaches)
                cache->SaveToArchive(archive);
        }

    private:
        friend class Level;

        void Initialize();
        void Shutdown();
        void CopyFrom(EntityWorld& world);
        void DestroyWorld();
        void DestroyEntityData(Entity* e);

    private:
        static EntityWorld*                  s_levelWorld;
        static uint32                        s_worldCounter;
        bool                                 m_initialized = false;
        HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
        Vector<Entity*>                      m_entities;
        uint32                               m_nextID = 0;
        Queue<uint32>                        m_availableIDs;
        Graphics::CameraComponent*           m_activeCamera = nullptr;
        uint32                               m_worldID      = 0;
    };

} // namespace Lina::World

#endif
