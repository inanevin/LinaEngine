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

#include "Core/Resources/Resource.hpp"
#include "ComponentCache.hpp"
#include "Common/Memory/MemoryAllocatorPool.hpp"
#include "Common/ObjectWrapper.hpp"
#include "Core/Physics/PhysicsWorld.hpp"
#include "Common/Event/GameEventDispatcher.hpp"
#include "Core/World/CommonWorld.hpp"
#include "Core/Graphics/Resource/Material.hpp"

namespace Lina
{
	class Entity;
	class Component;
	class CameraComponent;
	class SkyComponent;
	class EntityWorld;
	class WorldRenderer;

	class EntityWorldListener
	{
	public:
		virtual void OnComponentAdded(Component* c){};
		virtual void OnComponentRemoved(Component* c){};
	};
	// Actual game state
	class EntityWorld : public Resource, public GameEventDispatcher
	{
	public:
		struct GfxSettings
		{
			ResRef<Material> skyMaterial;

			void SaveToStream(OStream& stream) const;
			void LoadFromStream(IStream& stream);
		};

		Entity*		 GetEntity(uint32 id);
		Entity*		 GetEntity(const String& name);
		Entity*		 GetEntityFromSID(StringID sid);
		Entity*		 CreateEntity(const String& name);
		void		 DestroyEntity(Entity* e);
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		void		 AddListener(EntityWorldListener* listener);
		void		 RemoveListener(EntityWorldListener* listener);
		Vector2ui	 GetRenderSize() const;

		inline Bitmask32 GetFlags()
		{
			return m_flags;
		}

		inline uint32 GetID()
		{
			return m_id;
		}

		inline void SetActiveCamera(CameraComponent* cam)
		{
			m_activeCamera = cam;
		}

		inline CameraComponent* GetActiveCamera() const
		{
			return m_activeCamera;
		}

		inline void SetActiveSky(SkyComponent* sky)
		{
			m_activeSky = sky;
		}

		inline SkyComponent* GetActiveSky() const
		{
			return m_activeSky;
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

		void GetAllRootEntities(Vector<Entity*>& entities)
		{
			entities.reserve(m_entities.GetNextFreeID());

			for (auto e : m_entities)
			{
				if (e != nullptr && e->GetParent() == nullptr)
					entities.push_back(e);
			}
		}

		template <typename T> void GetAllComponents(Vector<T*>& comps)
		{
			auto* cache = Cache<T>();
			cache->GetAllComponents(comps);
		}

		template <typename T> T* GetComponent(Entity* e)
		{
			T* ptr = Cache<T>()->GetComponent(e);
			return ptr;
		}

		template <typename T> T* AddComponent(Entity* e, const T& t)
		{
			T* comp = Cache<T>()->AddComponent(e, t);
			*comp	= t;
			ProcessComponent(comp);

			for (auto* l : m_listeners)
				l->OnComponentAdded(comp);

			return comp;
		}

		template <typename T> T* AddComponent(Entity* e)
		{
			T* ptr = Cache<T>()->AddComponent(e);
			ProcessComponent(ptr);
			for (auto* l : m_listeners)
				l->OnComponentAdded(ptr);
			return ptr;
		}

		template <typename T> void RemoveComponent(Entity* e)
		{
			T* comp = Cache<T>()->GetComponent(e);

			for (auto* l : m_listeners)
				l->OnComponentRemoved(comp);

			if (comp == m_activeCamera)
				m_activeCamera = nullptr;

			if (comp == m_activeSky)
				m_activeSky = nullptr;

			Cache<T>()->DestroyComponent(e);
		}

		inline const GfxSettings& GetGfxSettings() const
		{
			return m_gfxSettings;
		}

		inline void SetSkyMaterial(Material* mat)
		{
			m_gfxSettings.skyMaterial.raw = mat;
			m_gfxSettings.skyMaterial.sid = mat->GetSID();
		}

	protected:
		virtual void LoadFromFile(const char* path) override;

	private:
		template <typename T> ComponentCache<T>* Cache()
		{
			const TypeID tid = GetTypeID<T>();

			if (m_componentCaches.find(tid) == m_componentCaches.end())
				m_componentCaches[tid] = new ComponentCache<T>(this, this);

			ComponentCache<T>* cache = static_cast<ComponentCache<T>*>(m_componentCaches[tid]);
			cache->m_entities		 = m_entities.GetRaw();
			return cache;
		}

		void ProcessComponent(Component* c);

	private:
		FRIEND_RESOURCE_CACHE();
		friend class WorldManager;

		EntityWorld(const EntityWorld& other) = delete;

		EntityWorld(ResourceManager* rm = nullptr, const String& path = "", StringID sid = 0)
			: Resource(rm, path, sid, GetTypeID<EntityWorld>()), m_physicsWorld(this), m_entities(IDList<Entity*>(ENTITY_POOL_SIZE, nullptr)),
			  m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(Entity) * ENTITY_POOL_SIZE, sizeof(Entity), "World"_hs))
		{
			m_id = s_worldCounter++;
		};

		~EntityWorld()
		{
			DestroyWorld();
		}

		void CopyFrom(EntityWorld& world);
		void DestroyWorld();
		void DestroyEntityData(Entity* e);
		void Simulate(float fixedDelta);
		void Tick(float deltaTime);
		void WaitForSimulation();

		inline void SetRenderer(WorldRenderer* rend)
		{
			m_renderer = rend;
		}

		inline WorldRenderer* GetRenderer() const
		{
			return m_renderer;
		}

	private:
		static uint32 s_worldCounter;

		System*								 m_system = nullptr;
		PhysicsWorld						 m_physicsWorld;
		MemoryAllocatorPool					 m_allocatorPool;
		HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
		IDList<Entity*>						 m_entities;
		CameraComponent*					 m_activeCamera = nullptr;
		SkyComponent*						 m_activeSky	= nullptr;
		uint32								 m_id			= 0;
		Bitmask32							 m_flags		= 0;
		Vector<EntityWorldListener*>		 m_listeners;
		WorldRenderer*						 m_renderer = nullptr;
		GfxSettings							 m_gfxSettings;
	};

} // namespace Lina

#endif
