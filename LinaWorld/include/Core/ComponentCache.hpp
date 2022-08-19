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

#include "Entity.hpp"
#include "Core/SizeDefinitions.hpp"
#include <functional>
#include <cereal/archives/portable_binary.hpp>

namespace Lina::World
{

    typedef std::function<void()>                                     CacheDestroyFunction;
    typedef std::function<void*()>                                    CacheCreateFunction;
    typedef std::function<void(cereal::PortableBinaryOutputArchive&)> CacheSaveFunction;
    typedef std::function<void(cereal::PortableBinaryInputArchive&)>  CacheLoadFunction;

    class ComponentCacheBase
    {
    public:
        virtual ComponentCacheBase* CopyCreate()                                                                     = 0;
        virtual void                DestroyComponent(Entity* e)                                                      = 0;
        virtual bool                ContainsEntity(Entity* e)                                                        = 0;
        virtual void                Destroy()                                                                        = 0;
        virtual void                SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive)                     = 0;
        virtual void                LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive, Entity** entities) = 0;
    };

    template <typename T>
    class ComponentCache : public ComponentCacheBase
    {
    public:
        virtual ComponentCacheBase* CopyCreate() override
        {
            ComponentCache<T>* newCache = new ComponentCache<T>();

            for (auto [entity, comp] : m_map)
                newCache->m_map[entity] = new T(*comp);

            return newCache;
        }
        virtual void DestroyComponent(Entity* e) override
        {
            delete m_map[e];
            m_map.erase(m_map.find(e));
        }

        virtual bool ContainsEntity(Entity* e) override
        {
            return m_map.find(e) != m_map.end();
        }

        virtual void Destroy() override
        {
            for (auto& pair : m_map)
                delete pair.second;

            m_map.clear();
        }

        virtual void SaveToArchive(cereal::PortableBinaryOutputArchive& oarchive) override
        {
            Vector<uint32> entityIDs;

            for (auto [e, c] : m_map)
                entityIDs.push_back(e->GetID());

            oarchive(entityIDs);

            for (auto [e, c] : m_map)
                c->SaveToArchive(oarchive);
        }

        virtual void LoadFromArchive(cereal::PortableBinaryInputArchive& iarchive, Entity** entities) override
        {
            Vector<uint32> entityIDs;
            iarchive(entityIDs);

            for (uint32 i = 0; i < entityIDs.size(); i++)
            {
                T* comp = new T();
                comp->LoadFromArchive(iarchive);
                m_map[entities[entityIDs[i]]] = comp;
            }
        }

        void RemoveComponent(Entity* e)
        {
            delete m_map[e];
            m_map.erase(m_map.find(e));
        }

    private:
        friend class EntityWorld;
        HashMap<Entity*, T*> m_map;
    };

} // namespace Lina::World

#endif
