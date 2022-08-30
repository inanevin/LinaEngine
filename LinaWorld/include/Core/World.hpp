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
#include "Serialization/Archive.hpp"
#include "Data/FixedList.hpp"

namespace Lina
{
    namespace Event
    {
        struct EStartGame;
        struct EEndGame;
        struct ETick;
        struct EPreTick;
        struct EPostTick;
        struct EPostPhysicsTick;
    } // namespace Event
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

        static EntityWorld* Get();

    public:
        Entity* GetEntity(uint32 id);
        Entity* GetEntity(const String& name);
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
        T** View(uint32* maxSize)
        {
            auto* cache = Cache<T>();
            *maxSize    = cache->m_nextID;
            return cache->m_components.data();
        }

        template <typename T>
        T* GetComponent(Entity* e)
        {
            return Cache<T>()->GetComponent(e);
        }

        template <typename T>
        T* AddComponent(Entity* e, const T& t)
        {
            T* comp = Cache<T>()->AddComponent(e, t);
            *comp   = t;
            return comp;
        }

        template <typename T>
        T* AddComponent(Entity* e)
        {
            return Cache<T>()->AddComponent(e);
        }

        template <typename T>
        void RemoveComponent(Entity* e)
        {
            Cache<T>()->DestroyComponent(e);
        }

        void SaveToArchive(Serialization::Archive<OStream>& archive);
        void LoadFromArchive(Serialization::Archive<IStream>& archive);

    private:
        void DestroyEntityData(Entity* e);
        friend class Level;

        void Initialize();
        void Shutdown();

    private:
        bool                                 m_initialized = false;
        HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
        Vector<Entity*>                      m_entities;
        uint32                               m_nextID = 0;
        Queue<uint32>                        m_availableIDs;
    };

} // namespace Lina::World

#endif
