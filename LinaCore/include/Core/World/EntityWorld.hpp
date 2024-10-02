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

#include "Core/Resources/Resource.hpp"
#include "ComponentCache.hpp"
#include "Screen.hpp"
#include "WorldInput.hpp"
#include "Common/Memory/MemoryAllocatorPool.hpp"
#include "Common/ObjectWrapper.hpp"
#include "Core/Physics/PhysicsWorld.hpp"
#include "Common/Event/GameEventDispatcher.hpp"
#include "Core/World/CommonWorld.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Resources/ResourceManager.hpp"

namespace Lina
{
	class Model;
	class Entity;
	class Component;
	class CameraComponent;
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
			ResRef<Material> lightingMaterial;

			void SaveToStream(OStream& stream)
			{
				stream << skyMaterial << lightingMaterial;
			}

			void LoadFromStream(IStream& stream, ResourceManagerV2* rm)
			{
				stream >> skyMaterial >> lightingMaterial;
				skyMaterial.raw		 = rm->GetResource<Material>(skyMaterial.id);
				lightingMaterial.raw = rm->GetResource<Material>(lightingMaterial.id);
			}

			inline void SetSkyMaterial(Material* mat)
			{
				skyMaterial.raw = mat;
				skyMaterial.id	= mat->GetID();
			}

			inline void SetLightingMaterial(Material* mat)
			{
				lightingMaterial.raw = mat;
				lightingMaterial.id	 = mat->GetID();
			}
		};

		static constexpr uint32 VERSION = 0;

		EntityWorld(const EntityWorld& other) = delete;
		EntityWorld(ResourceID id, const String& name);
		~EntityWorld();

		Entity*		 CreateEntity(const String& name);
		void		 DestroyEntity(Entity* e);
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void LoadFromFile(const String& path) override;
		void		 AddListener(EntityWorldListener* listener);
		void		 RemoveListener(EntityWorldListener* listener);
		void		 PreTick();
		void		 Tick(float deltaTime);
		Entity*		 AddModelToWorld(Model* model, const Vector<Material*>& materials);

		inline uint32 GetActiveEntityCount() const
		{
			return m_entityBucket.GetActiveItemCount();
		}

		inline void ViewEntities(Delegate<bool(Entity* e, uint32 index)>&& callback)
		{
			m_entityBucket.View(std::move(callback));
		}

		inline Bitmask32& GetFlags()
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

		template <typename T> T* GetComponent(Entity* e)
		{
			return GetCache<T>()->Get();
		}

		template <typename T> T* AddComponent(Entity* e, const T& t)
		{
			T* comp = GetCache<T>()->Create();
			*comp	= t;
			ProcessComponent(comp, e);
			for (auto* l : m_listeners)
				l->OnComponentAdded(comp);

			return comp;
		}

		template <typename T> T* AddComponent(Entity* e)
		{
			T* ptr = GetCache<T>()->Create();
			ProcessComponent(ptr, e);
			for (auto* l : m_listeners)
				l->OnComponentAdded(ptr);
			return ptr;
		}

		template <typename T> void RemoveComponent(Entity* e)
		{
			ComponentCache<T>* cache = GetCache<T>();
			T*				   comp	 = cache->Get(e);

			for (auto* l : m_listeners)
				l->OnComponentRemoved(comp);

			if (comp == m_activeCamera)
				m_activeCamera = nullptr;

			cache->Destroy(e);
		}

		template <typename T> ComponentCache<T>* GetCache()
		{
			const TypeID tid = GetTypeID<T>();

			if (m_componentCaches.find(tid) == m_componentCaches.end())
				m_componentCaches[tid] = new ComponentCache<T>(this);

			ComponentCache<T>* cache = static_cast<ComponentCache<T>*>(m_componentCaches[tid]);
			return cache;
		}

		inline GfxSettings& GetGfxSettings()
		{
			return m_gfxSettings;
		}

		inline ResourceManagerV2& GetResourceManagerV2()
		{
			return m_resourceManagerV2;
		}

		inline Screen& GetScreen()
		{
			return m_screen;
		}

	private:
		void ProcessComponent(Component* c, Entity* e);

	private:
		friend class WorldManager;

		void DestroyEntityData(Entity* e);
		void Simulate(float fixedDelta);

		void WaitForSimulation();

	private:
		AllocatorBucket<Entity, 1000> m_entityBucket;

		ResourceManagerV2					 m_resourceManagerV2;
		PhysicsWorld						 m_physicsWorld;
		HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
		CameraComponent*					 m_activeCamera = nullptr;
		Bitmask32							 m_flags		= 0;
		Vector<EntityWorldListener*>		 m_listeners;
		GfxSettings							 m_gfxSettings;
		Screen								 m_screen	  = {};
		WorldInput							 m_worldInput = nullptr;
	};

} // namespace Lina
