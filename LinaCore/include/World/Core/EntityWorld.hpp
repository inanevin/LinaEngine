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
#include "Memory/MemoryAllocatorPool.hpp"
#include "Core/ObjectWrapper.hpp"
#include "Physics/Core/PhysicsWorld.hpp"
#include "Event/GameEventDispatcher.hpp"

namespace Lina
{
	class Entity;
	class Component;
	class CameraComponent;
	class EntityWorld;

	// Actual game state
	class EntityWorld : public GameEventDispatcher
	{
	public:
		EntityWorld()
			: m_physicsWorld(this), m_entities(IDList<Entity*>(ENTITY_POOL_SIZE, nullptr)),
			  m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(Entity) * ENTITY_POOL_SIZE, sizeof(Entity), "EntityPool", "World"_hs))
		{
			m_id = s_worldCounter++;
		};

		~EntityWorld()
		{
			DestroyWorld();
		}

	public:
		Entity* GetEntity(uint32 id);
		Entity* GetEntity(const String& name);
		Entity* GetEntityFromSID(StringID sid);
		Entity* GetEntityFromID(uint32 id);
		Entity* CreateEntity(const String& name);
		void	DestroyEntity(Entity* e);
		void	SaveToStream(OStream& stream);
		void	LoadFromStream(IStream& stream);

		inline uint32 GetID()
		{
			return m_id;
		}

		inline void SetActiveCamera(ObjectWrapper<CameraComponent>& cam)
		{
			m_activeCamera = cam;
		}

		inline ObjectWrapper<CameraComponent> GetActiveCamera()
		{
			return m_activeCamera;
		}

		void GetAllEntities(Vector<Entity*>& entities)
		{
			entities.reserve(m_entities.GetNextFreeID());

			for (auto e : m_entities)
			{
				if (e != nullptr)
					entities.push_back(e);
			}
		}

		template <typename T> void GetAllComponents(Vector<ObjectWrapper<T>>& comps)
		{
			auto* cache = Cache<T>();

			Vector<T*> ptrs = cache->GetAllComponents();

			comps.reserve(ptrs.size());

			for (auto p : ptrs)
				comps.push_back(ObjectWrapper<T>(p));
		}

		template <typename T> ObjectWrapper<T> GetComponent(Entity* e)
		{
			T* ptr = Cache<T>()->GetComponent(e);
			return ObjectWrapper<T>(ptr);
		}

		template <typename T> ObjectWrapper<T> AddComponent(Entity* e, const T& t)
		{
			T* comp = Cache<T>()->AddComponent(e, t);
			*comp	= t;
			return ObjectWrapper<T>(comp);
		}

		template <typename T> ObjectWrapper<T> AddComponent(Entity* e)
		{
			T* ptr = Cache<T>()->AddComponent(e);
			return ObjectWrapper<T>(ptr);
		}

		template <typename T> void RemoveComponent(Entity* e)
		{
			Cache<T>()->DestroyComponent(e);

			if (Cache<T>()->GetComponent(e) == m_activeCamera)
				m_activeCamera.Reset();
		}

	private:
		template <typename T> ComponentCache<T>* Cache()
		{
			const TypeID tid = GetTypeID<T>();

			if (m_componentCaches.find(tid) == m_componentCaches.end())
				m_componentCaches[tid] = new ComponentCache<T>(this, this);

			ComponentCache<T>* cache = static_cast<ComponentCache<T>*>(m_componentCaches[tid]);
			return cache;
		}

	private:
		friend class LevelManager;

		void CopyFrom(EntityWorld& world);
		void DestroyWorld();
		void DestroyEntityData(Entity* e);
		void Simulate(float fixedDelta);
		void Tick(float deltaTime);
		void WaitForSimulation();

	private:
		static uint32 s_worldCounter;

		PhysicsWorld						 m_physicsWorld;
		MemoryAllocatorPool					 m_allocatorPool;
		HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
		IDList<Entity*>						 m_entities;
		ObjectWrapper<CameraComponent>		 m_activeCamera;
		uint32								 m_id = 0;
	};

} // namespace Lina

#endif
