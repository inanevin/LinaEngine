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

#include "CommonWorld.hpp"
#include "Entity.hpp"
#include "Common/SizeDefinitions.hpp"
#include "Common/Data/IDList.hpp"
#include "Common/Serialization/HashMapSerialization.hpp"
#include "Common/Memory/MemoryAllocatorPool.hpp"
#include "Common/Event/GameEventDispatcher.hpp"

namespace Lina
{
	class EntityWorld;

#define COMPONENT_POOL_SIZE 100

	class ComponentCacheBase
	{
	public:
		ComponentCacheBase()		  = default;
		virtual ~ComponentCacheBase() = default;

		virtual void				PreTick()						= 0;
		virtual void				Tick(float delta)				= 0;
		virtual void				PostTick(float delta)			= 0;
		virtual void				LoadFromStream(IStream& stream) = 0;
		virtual void				SaveToStream(OStream& stream)	= 0;
		virtual void				OnEntityDestroyed(Entity* e)	= 0;
		virtual ComponentCacheBase* CopyCreate()					= 0;
		Entity**					m_entities						= nullptr;
	};

	template <typename T> class ComponentCache : public ComponentCacheBase
	{
	public:
		ComponentCache(EntityWorld* world, GameEventDispatcher* eventDispatcher)
			: m_world(world), m_components(IDList<T*>(COMPONENT_POOL_SIZE, nullptr)), m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(T) * COMPONENT_POOL_SIZE, sizeof(T), "World"_hs))
		{
			m_eventDispatcher = eventDispatcher;
		}

		virtual ~ComponentCache()
		{
			Destroy();
		}

		virtual void PreTick() override
		{
			for (T* comp : m_components)
			{
				if (comp != nullptr)
					comp->PreTick();
			}
		}

		virtual void Tick(float delta) override
		{
			for (T* comp : m_components)
			{
				if (comp != nullptr)
					comp->Tick(delta);
			}
		}

		virtual void PostTick(float delta) override
		{
			for (T* comp : m_components)
			{
				if (comp != nullptr)
					comp->PostTick(delta);
			}
		}
		// When copying the world.
		virtual ComponentCacheBase* CopyCreate() override
		{
			ComponentCache<T>* newCache = new ComponentCache<T>(m_world, m_eventDispatcher);

			int i = 0;
			for (auto comp : m_components)
			{
				if (comp != nullptr)
				{
					T* newComp = new (newCache->m_allocatorPool.Allocate(sizeof(T))) T();
					*newComp   = *comp;
					newCache->m_components.AddItem(newComp, i);
				}
				i++;
			}

			return newCache;
		}

		// Mutators
		inline T* AddComponent(Entity* e)
		{
			T* comp = new (m_allocatorPool.Allocate(sizeof(T))) T();
			m_components.AddItem(comp);
			return comp;
		}

		inline T* GetComponent(Entity* e)
		{
			for (auto* comp : m_components)
			{
				if (comp != nullptr && comp->m_entityID == e->GetID())
					return comp;
			}
			return nullptr;
		}

		inline void DestroyComponent(Entity* e)
		{
			uint32 index = 0;
			for (auto* comp : m_components)
			{
				if (comp != nullptr && comp->m_entityID == e->GetID())
				{
					comp->Destroy();
					comp->~T();
					m_components.RemoveItem(index);
					m_allocatorPool.Free(comp);
					break;
				}
				index++;
			}
		}

		inline void GetAllComponents(Vector<T*>& comps)
		{
			comps.reserve(static_cast<size_t>(m_components.GetNextFreeID()));
			for (auto c : m_components)
			{
				if (c != nullptr)
					comps.push_back(c);
			}
		}

		virtual void OnEntityDestroyed(Entity* e) override
		{
			DestroyComponent(e);
		}

		virtual void SaveToStream(OStream& stream) override
		{
			m_components.SaveToStream(stream);

			HashMap<uint32, uint32> compEntityIDs;

			int i = 0;
			for (auto* comp : m_components)
			{
				if (comp != nullptr)
					compEntityIDs[i] = comp->m_entityID;

				i++;
			}

			HashMapSerialization::SaveToStream_PT(stream, compEntityIDs);

			for (auto [compID, entityID] : compEntityIDs)
			{
				auto comp = m_components.GetItem(compID);
				stream << comp->m_entityID;
				comp->SaveToStream(stream);
			}
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			m_components.LoadFromStream(stream);

			HashMap<uint32, uint32> compEntityIDs;
			HashMapSerialization::LoadFromStream_PT(stream, compEntityIDs);

			for (auto [compID, entityID] : compEntityIDs)
			{
				T* comp = new (m_allocatorPool.Allocate(sizeof(T))) T();
				stream >> comp->m_entityID;
				comp->LoadFromStream(stream);
				comp->m_entity = m_entities[comp->m_entityID];
				m_components.AddItem(comp, compID);
			}
		}

	private:
		virtual void Destroy()
		{
			for (auto* comp : m_components)
			{
				if (comp != nullptr)
				{
					comp->Destroy();
					comp->~T();
					m_allocatorPool.Free(comp);
				}
			}

			m_components.Clear();
		}

	private:
		EntityWorld*		 m_world		   = nullptr;
		GameEventDispatcher* m_eventDispatcher = nullptr;
		MemoryAllocatorPool	 m_allocatorPool;
		IDList<T*>			 m_components;
	};

} // namespace Lina

#endif
