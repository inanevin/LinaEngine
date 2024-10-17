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
#include "Core/World/CommonWorld.hpp"
#include "Core/World/ComponentCache.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"
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
	class EntityWorld : public Resource
	{
	public:
		struct GfxSettings
		{
			ResRef<Material> skyMaterial;
			ResRef<Material> lightingMaterial;
			ResRef<Model>	 skyModel;

			void SaveToStream(OStream& stream)
			{
				stream << skyMaterial << lightingMaterial << skyModel;
			}

			void LoadFromStream(IStream& stream, ResourceManagerV2* rm)
			{
				stream >> skyMaterial >> lightingMaterial >> skyModel;
				skyMaterial.raw		 = rm->GetResource<Material>(skyMaterial.id);
				lightingMaterial.raw = rm->GetResource<Material>(lightingMaterial.id);
				skyModel.raw		 = rm->GetResource<Model>(skyModel.id);
			}

			inline void SetSkyMaterial(Material* mat)
			{
				skyMaterial.raw = mat;
				skyMaterial.id	= mat->GetID();
			}

			inline void SetSkyModel(Model* m)
			{
				skyModel.raw = m;
				skyModel.id	 = m->GetID();
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
		virtual bool LoadFromFile(const String& path) override;
		void		 AddListener(EntityWorldListener* listener);
		void		 RemoveListener(EntityWorldListener* listener);
		Entity*		 AddModelToWorld(Model* model, const Vector<Material*>& materials);

		template <typename T> T* GetComponent(Entity* e)
		{
			return GetCache<T>()->Get();
		}

		template <typename T> T* AddComponent(Entity* e, const T& t)
		{
			auto* cache = GetCache<T>();
			T*	  comp	= cache->Get(e);
			if (comp != nullptr)
			{
				LINA_WARN("Can't add component as it already exists!");
				return comp;
			}
			comp  = cache->Create();
			*comp = t;
			OnCreateComponent(comp, e);
			return comp;
		}

		template <typename T> T* AddComponent(Entity* e)
		{
			auto* cache = GetCache<T>();
			T*	  comp	= cache->Get(e);

			if (comp != nullptr)
			{
				LINA_WARN("Can't add component as it already exists!");
				return comp;
			}

			comp = cache->Create();
			OnCreateComponent(comp, e);
			return comp;
		}

		template <typename T> void RemoveComponent(Entity* e)
		{
			ComponentCache<T>* cache = GetCache<T>();
			T*				   comp	 = cache->Get(e);

			if (comp == nullptr)
			{
				LINA_ERR("Can't remove component as it doesn't exist!");
				return;
			}

			OnDestroyComponent(comp, e);
			cache->Destroy(comp);
		}

		template <typename T> ComponentCache<T>* GetCache()
		{
			const TypeID tid = GetTypeID<T>();

			if (m_componentCaches.find(tid) == m_componentCaches.end())
				m_componentCaches[tid] = new ComponentCache<T>(this);

			ComponentCache<T>* cache = static_cast<ComponentCache<T>*>(m_componentCaches[tid]);
			return cache;
		}

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

		inline WorldInput& GetInput()
		{
			return m_worldInput;
		}

		inline EntityID ConsumeEntityGUID()
		{
			return m_entityGUIDCounter++;
		}

	private:
		void DestroyEntityData(Entity* e);
		void OnCreateComponent(Component* c, Entity* e);
		void OnDestroyComponent(Component* c, Entity* e);

	private:
		ALLOCATOR_BUCKET_MEM;

		AllocatorBucket<Entity, 1000>		 m_entityBucket;
		HashMap<TypeID, ComponentCacheBase*> m_componentCaches;
		EntityID							 m_entityGUIDCounter = 1;
		ResourceManagerV2					 m_resourceManagerV2;
		PhysicsWorld						 m_physicsWorld;
		Bitmask32							 m_flags = 0;
		Vector<EntityWorldListener*>		 m_listeners;
		GfxSettings							 m_gfxSettings;
		Screen								 m_screen = {};
		WorldInput							 m_worldInput;
	};

	LINA_RESOURCE_BEGIN(EntityWorld)
	LINA_CLASS_END(EntityWorld)

} // namespace Lina
