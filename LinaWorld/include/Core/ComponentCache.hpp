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
#include "Data/Serialization/HashMapSerialization.hpp"
#include "Data/Serialization/QueueSerialization.hpp"

#include <functional>
#include <cereal/archives/portable_binary.hpp>

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
    typedef std::function<void(cereal::PortableBinaryOutputArchive&)> CacheSaveFunction;
    typedef std::function<void(cereal::PortableBinaryInputArchive&)>  CacheLoadFunction;

#define MAX_COMP_COUNT 5

    class Component;
    class ComponentCacheBase
    {
    protected:
        friend class EntityWorld;

        virtual ComponentCacheBase* CopyCreate()                                                                     = 0;
        virtual void                OnEntityDestroyed(Entity* e)                                                     = 0;
        virtual void                Destroy()                                                                        = 0;
        virtual void                SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive)                     = 0;
        virtual void                LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive, Entity** entities) = 0;
    };

    template <typename T>
    class ComponentCache : public ComponentCacheBase
    {
    protected:
        virtual ComponentCacheBase* CopyCreate() override
        {
            ComponentCache<T>* newCache = new ComponentCache<T>();

            for (uint32 i = 0; i < m_nextID; i++)
            {
                if (m_components[i] != nullptr)
                {
                    T* newComp = new T(*m_components[i]);
                    newComp->OnComponentCreated();
                    newCache->m_components[i] = newComp;
                }
            }

            return newCache;
        }

        inline T* AddComponent(Entity* e)
        {
            T* comp          = new T();
            comp->m_entityID = e->GetID();
            comp->m_entity   = e;
            comp->OnComponentCreated();

            uint32 id = 0;

            if (!m_availableIDs.empty())
            {
                id = m_availableIDs.back();
                m_availableIDs.pop();
            }
            else
                id = m_nextID++;

            m_components[id] = comp;
            return comp;
        }

        inline T* AddComponent(Entity* e, const T& t)
        {
            T* comp          = new T(t);
            comp->m_entityID = e->GetID();
            comp->m_entity   = e;
            comp->OnComponentCreated();

            uint32 id = 0;

            if (!m_availableIDs.empty())
            {
                id = m_availableIDs.back();
                m_availableIDs.pop();
            }
            else
                id = m_nextID++;

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
                        T* comp = m_components[i];
                        comp->OnComponentDestroyed();
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
                    delete m_components[i];
                    m_components[i] = nullptr;
                }
            }

            m_availableIDs = Queue<uint32>();
            m_nextID       = 0;
        }

        virtual void SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive) override
        {
            oarchive(m_availableIDs);
            oarchive(m_nextID);
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

        virtual void LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive, Entity** entities) override
        {
            iarchive(m_availableIDs);
            iarchive(m_nextID);

            HashMap<uint32, uint32> compEntityIDs;
            iarchive(compEntityIDs);

            for (auto [compID, entityID] : compEntityIDs)
            {
                T* comp = new T();
                comp->LoadFromArchive(iarchive);
                comp->OnComponentCreated();
                comp->m_entityID     = entityID;
                comp->m_entity       = entities[entityID];
                m_components[compID] = comp;
            }
        }

    private:
        friend class EntityWorld;

        T*            m_components[MAX_COMP_COUNT] = {nullptr};
        uint32        m_nextID                     = 0;
        Queue<uint32> m_availableIDs;
    };

} // namespace Lina::World

#endif
