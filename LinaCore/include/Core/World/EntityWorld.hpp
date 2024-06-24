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

		EntityWorld(const EntityWorld& other) = delete;

		EntityWorld(ResourceManager* rm, const String& path = "", StringID sid = 0, uint32 flags = 0);

		~EntityWorld()
		{
			DestroyWorld();
		}

		Entity*		 GetEntity(uint32 id);
		Entity*		 GetEntity(const String& name);
		Entity*		 GetEntityFromSID(StringID sid);
		Entity*		 CreateEntity(const String& name);
		void		 DestroyEntity(Entity* e);
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		void		 AddListener(EntityWorldListener* listener);
		void		 RemoveListener(EntityWorldListener* listener);
		void		 PreTick();
		void		 Tick(float deltaTime);

		inline void SetRenderSize(const Vector2ui& sz)
		{
			m_renderSize = sz;
		}

		inline Vector2ui GetRenderSize() const
		{
			return m_renderSize;
		}

		inline System* GetSystem()
		{
			return m_system;
		}

		inline Bitmask32 GetFlags()
		{
			return m_flags;
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
			ProcessComponent(comp, e);

			for (auto* l : m_listeners)
				l->OnComponentAdded(comp);

			return comp;
		}

		template <typename T> T* AddComponent(Entity* e)
		{
			T* ptr = Cache<T>()->AddComponent(e);
			ProcessComponent(ptr, e);
			for (auto* l : m_listeners)
				l->OnComponentAdded(ptr);
			return ptr;
		}

		template <typename T> void RemoveComponent(Entity* e)
		{
			T* comp = Cache<T>()->GetComponent(e);
			comp->Destroy();

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

		void ProcessComponent(Component* c, Entity* e);

	private:
		FRIEND_RESOURCE_CACHE();
		friend class WorldManager;

		void CopyFrom(EntityWorld& world);
		void DestroyWorld();
		void DestroyEntityData(Entity* e);
		void Simulate(float fixedDelta);

		void WaitForSimulation();

	private:
		System*								 m_system = nullptr;
		PhysicsWorld						 m_physicsWorld;
		MemoryAllocatorPool					 m_allocatorPool;
		HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
		IDList<Entity*>						 m_entities;
		CameraComponent*					 m_activeCamera = nullptr;
		SkyComponent*						 m_activeSky	= nullptr;
		Bitmask32							 m_flags		= 0;
		Vector<EntityWorldListener*>		 m_listeners;
		GfxSettings							 m_gfxSettings;
		Vector2ui							 m_renderSize;
	};

} // namespace Lina

#endif
