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

#ifndef ComponentCache_HPP
#define ComponentCache_HPP

#include "WorldCommon.hpp"
#include "Entity.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Data/Queue.hpp"
#include "Data/HashMap.hpp"
#include "Serialization/HashMapSerialization.hpp"
#include "Serialization/QueueSerialization.hpp"
#include "Serialization/Archive.hpp"

#include <functional>

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
    typedef std::function<void()>                                     CacheDestroyFunction;
    typedef std::function<void*()>                                    CacheCreateFunction;

#define COMPONENT_VEC_SIZE_CHUNK 2000

    class Component;
    class ComponentCacheBase
    {
    protected:
        friend class EntityWorld;

        virtual ComponentCacheBase* CopyCreate()                                                                     = 0;
        virtual void                OnEntityDestroyed(Entity* e)                                                     = 0;
        virtual void                Destroy()                                                                        = 0;
        virtual void                SaveToArchive(Serialization::Archive<OStream>& oarchive)                     = 0;
        virtual void                LoadFromArchive(Serialization::Archive<IStream>& iarchive, Entity** entities) = 0;
    };

    template <typename T>
    class ComponentCache : public ComponentCacheBase
    {
    public:
        ComponentCache()
        {
            if (!Memory::MemoryManager::Get()->PoolBlockExists<T>())
            {
                const String& title        = Reflection::Resolve(GetTypeID<T>()).GetProperty("Title"_hs);
                const String& chunkSizeStr = Reflection::Resolve(GetTypeID<T>()).GetProperty("MemChunkSize"_hs);
                const uint32  chunkSize    = static_cast<uint32>(std::stoi(chunkSizeStr.c_str()));
                Memory::MemoryManager::Get()->CreatePoolBlock<T>(chunkSize, title);
            }

            if (m_components.empty())
                m_components.resize(COMPONENT_VEC_SIZE_CHUNK, nullptr);
        }

    protected:
        virtual ComponentCacheBase* CopyCreate() override
        {
            ComponentCache<T>* newCache = new ComponentCache<T>();

            for (uint32 i = 0; i < m_nextID; i++)
            {
                T* comp = m_components[i];
                if (comp != nullptr)
                {
                    T* newComp = Memory::MemoryManager::Get()->GetFromPoolBlock<T>();
                    *newComp   = *comp;
                    newComp->OnComponentCreated();
                    newCache->m_components[i] = newComp;
                }
            }

            return newCache;
        }

        inline T* AddComponent(Entity* e)
        {
            T* comp          = Memory::MemoryManager::Get()->GetFromPoolBlock<T>();
            comp->m_entityID = e->GetID();
            comp->m_entity   = e;
            comp->OnComponentCreated();

            uint32 id = 0;

            if (!m_availableIDs.empty())
            {
                id = m_availableIDs.front();
                m_availableIDs.pop();
            }
            else
                id = m_nextID++;

            if (m_components.size() <= id)
                m_components.resize(m_components.size() + COMPONENT_VEC_SIZE_CHUNK, nullptr);

            m_components[id] = comp;
            return comp;
        }

        inline T* GetComponent(Entity* e)
        {
            for (uint32 i = 0; i < m_nextID; i++)
            {
                if (m_components[i] != nullptr)
                {
                    if (m_components[i]->m_entityID == e->GetID())
                        return m_components[i];
                }
            }

            return nullptr;
        }

        inline void DestroyComponent(Entity* e)
        {
            for (uint32 i = 0; i < m_nextID; i++)
            {
                if (m_components[i] != nullptr)
                {
                    if (m_components[i]->m_entityID == e->GetID())
                    {
                        T*           comp  = m_components[i];
                        const uint32 index = comp->m_allocPoolIndex;
                        comp->OnComponentDestroyed();
                        comp->~T();
                        Memory::MemoryManager::Get()->FreeFromPoolBlock<T>(comp, index);
                        m_availableIDs.push(i);
                        m_components[i] = nullptr;
                        break;
                    }
                }
            }
        }

        virtual void OnEntityDestroyed(Entity* e) override
        {
            DestroyComponent(e);
        }

        virtual void Destroy() override
        {
            for (uint32 i = 0; i < m_nextID; i++)
            {
                if (m_components[i] != nullptr)
                {
                    T*           comp  = m_components[i];
                    const uint32 index = comp->m_allocPoolIndex;
                    comp->OnComponentDestroyed();
                    comp->~T();
                    Memory::MemoryManager::Get()->FreeFromPoolBlock<T>(comp, index);
                    m_components[i] = nullptr;
                }
            }

            m_availableIDs = Queue<uint32>();
            m_nextID       = 0;
            m_components.clear();
        }

        virtual void SaveToArchive(Serialization::Archive<OStream>& oarchive) override
        {
            const uint32 compsSize = static_cast<uint32>(m_components.size());
            oarchive(m_availableIDs);
            oarchive(m_nextID);
            oarchive(compsSize);

            HashMap<uint32, uint32> compEntityIDs;

            for (uint32 i = 0; i < m_nextID; i++)
            {
                if (m_components[i] != nullptr)
                    compEntityIDs[i] = m_components[i]->m_entityID;
            }

            oarchive(compEntityIDs);

            for (auto [compID, entityID] : compEntityIDs)
            {
                m_components[compID]->SaveToArchive(oarchive);
            }
        }

        virtual void LoadFromArchive(Serialization::Archive<IStream>& iarchive, Entity** entities) override
        {
            uint32 compsSize = 0;
            iarchive(m_availableIDs);
            iarchive(m_nextID);
            iarchive(compsSize);
            m_components.clear();
            m_components.resize(compsSize, nullptr);

            HashMap<uint32, uint32> compEntityIDs;
            iarchive(compEntityIDs);

            for (auto [compID, entityID] : compEntityIDs)
            {
                T* comp = Memory::MemoryManager::Get()->GetFromPoolBlock<T>();
                comp->LoadFromArchive(iarchive);
                comp->m_entityID     = entityID;
                comp->m_entity       = entities[entityID];
                comp->OnComponentCreated();
                m_components[compID] = comp;
            }
        }

    private:
        friend class EntityWorld;

        Vector<T*>    m_components;
        uint32        m_nextID = 0;
        Queue<uint32> m_availableIDs;
    };

} // namespace Lina::World

#endif
