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

#include "ComponentCache.hpp"
#include "Serialization/ISerializable.hpp"
#include "Memory/MemoryAllocatorPool.hpp"

namespace Lina
{
    class Entity;
    class Component;
    class CameraComponent;
    class EntityWorld;

    template <typename T> class ComponentReference
    {
    public:
        ComponentReference() = default;

        T& Get()
        {
            return *m_ptr;
        }

    private:
        friend class EntityWorld;
        ComponentReference(T* ptr) : m_ptr(ptr){};
        T* m_ptr = nullptr;
    };

    // Actual game state
    class EntityWorld : public ISerializable
    {
    public:
        EntityWorld() : m_entities(IDList<Entity*>(ENTITY_POOL_SIZE, nullptr)), m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(Entity) * ENTITY_POOL_SIZE, sizeof(Entity), "EntityPool", "World"_hs))
        {
            m_id = s_worldCounter++;
        };

        ~EntityWorld()
        {
            DestroyWorld();
        }

    public:
        Entity*      GetEntity(uint32 id);
        Entity*      GetEntity(const String& name);
        Entity*      CreateEntity(const String& name);
        void         DestroyEntity(Entity* e);
        virtual void SaveToStream(OStream& stream) override;
        virtual void LoadFromStream(IStream& stream) override;

        inline uint32 GetID()
        {
            return m_id;
        }

        inline void SetActiveCamera(ComponentReference<CameraComponent>& cam)
        {
            m_activeCamera = cam.m_ptr;
        }

        inline ComponentReference<CameraComponent> GetActiveCamera()
        {
            return ComponentReference<CameraComponent>(m_activeCamera);
        }

        // template <typename T> T** View(uint32* maxSize)
        // {
        //     auto* cache = Cache<T>();
        //     *maxSize    = cache->m_components.GetNextFreeID();
        //     return cache->m_components.GetRaw();
        // }

        template <typename T> ComponentReference<T> GetComponent(Entity* e)
        {
            T* ptr = Cache<T>()->GetComponent(e);
            return ComponentReference<T>(ptr);
        }

        template <typename T> ComponentReference<T> AddComponent(Entity* e, const T& t)
        {
            T* comp = Cache<T>()->AddComponent(e, t);
            *comp   = t;
            return ComponentReference<T>(comp);
        }

        template <typename T> ComponentReference<T> AddComponent(Entity* e)
        {
            T* ptr = Cache<T>()->AddComponent(e);
            return ComponentReference<T>(ptr);
        }

        template <typename T> void RemoveComponent(Entity* e)
        {
            Cache<T>()->DestroyComponent(e);

            if (Cache<T>()->GetComponent(e) == m_activeCamera)
                m_activeCamera = nullptr;
        }

    private:
        template <typename T> ComponentCache<T>* Cache()
        {
            const TypeID       tid   = GetTypeID<T>();
            ComponentCache<T>* cache = static_cast<ComponentCache<T>*>(m_componentCaches[tid]);
            return cache;
        }

    private:
        void CopyFrom(EntityWorld& world);
        void DestroyWorld();
        void DestroyEntityData(Entity* e);

    private:
        static uint32                        s_worldCounter;
        MemoryAllocatorPool                  m_allocatorPool;
        HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
        IDList<Entity*>                      m_entities;
        CameraComponent*                     m_activeCamera = nullptr;
        uint32                               m_id           = 0;
    };

} // namespace Lina

#endif