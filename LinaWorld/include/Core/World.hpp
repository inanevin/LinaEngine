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

#include "Entity.hpp"
#include "ComponentCache.hpp"
#include "Data/Queue.hpp"
#include "Reflection/ReflectionSystem.hpp"

namespace Lina::World
{
#define MAX_ENTITY_COUNT 50000

    class Entity;
    class Component;

    // Actual game state
    class EntityWorld
    {
    public:
        EntityWorld() = default;
        ~EntityWorld();

        static EntityWorld* Get();

    public:

        Entity* GetEntity(uint32 id);
        void    CopyFrom(EntityWorld& world);
        void    DestroyWorld();
        Entity* CreateEntity(const String& name);
        void    DestroyEntity(Entity* e);

        template <typename T>
        ComponentCache<T>* Cache()
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

        template <typename T>
        HashMap<Entity*, T*>& View()
        {
            return Cache<T>()->m_map;
        }

        template <typename T>
        T* GetComponent(Entity* e)
        {
            return Cache<T>()->m_map[e];
        }

        template <typename T>
        bool HasComponent(Entity* e)
        {
            auto* cache = Cache<T>();
            return cache->m_map.find(e) != cache->m_map.end();
        }

        template <typename T>
        T* AddComponent(Entity* e, T& t)
        {
            if (HasComponent<T>(e))
                return GetComponent<T>(e);

            T* tptr              = new T(t);
            tptr->m_entity       = e;
            tptr->m_entityID     = e->m_id;
            Cache<T>()->m_map[e] = tptr;
            return tptr;
        }

        template <typename T>
        T* AddComponent(Entity* e)
        {
            if (HasComponent<T>(e))
                return GetComponent<T>(e);

            T* t                 = new T();
            t->m_entity          = e;
            t->m_entityID        = e->m_id;
            Cache<T>()->m_map[e] = t;
            return t;
        }

        template <typename T>
        void RemoveComponent(Entity* e)
        {
            if (!HasComponent<T>(e))
                return;

            auto* cache = Cache<T>();
            cache->RemoveComponent(e);
        }

        void SaveToArchive(cereal::PortableBinaryOutputArchive& archive);
        void LoadFromArchive(cereal::PortableBinaryInputArchive& archive);

    private:
        HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
        Entity*                              m_entities[MAX_ENTITY_COUNT] = {nullptr};
        uint32                               m_nextID                     = 0;
        Queue<uint32>                        m_availableIDs;
    };

} // namespace Lina::World

#endif
