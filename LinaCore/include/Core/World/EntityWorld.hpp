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
#include "Core/World/ComponentCache.hpp"
#include "Core/World/Screen.hpp"
#include "Core/World/Camera.hpp"
#include "Core/World/WorldInput.hpp"
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

	enum class PlayMode
	{
		None,
		Play,
		Other,
	};

	class EntityWorldListener
	{
	public:
		virtual void OnComponentAdded(Component* c){};
		virtual void OnComponentRemoved(Component* c){};
		virtual void OnGeneratedResources(Vector<Resource*>& resources){};
		virtual void OnWorldTick(float delta, PlayMode playmode){};
	};

	class EntityWorld : public Resource
	{
	public:
		struct SimulationSettings
		{
			uint32 targetUpdateTicks = 60;
		};

		struct GfxSettings
		{
			ResourceID skyMaterial;
			ResourceID lightingMaterial;
			ResourceID skyModel;

			void SaveToStream(OStream& stream) const
			{
				stream << skyMaterial << lightingMaterial << skyModel;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> skyMaterial >> lightingMaterial >> skyModel;
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
		Entity*		 AddModelToWorld(Model* model, const Vector<ResourceID>& materials);
		void		 CollectResourceNeeds(HashSet<ResourceID>& outResources);
		void		 LoadMissingResources(ProjectData* project, const HashSet<ResourceID>& extraResources);
		void		 VerifyResources();

		void Tick(float delta);
		void BeginPlay(PlayMode playmode);
		void EndPlay();
		void TickPlay(float deltaTime, PlayMode playmode);

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
			T*	  comp	= static_cast<T*>(cache->Get(e));

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
			T*				   comp	 = static_cast<T*>(cache->Get(e));

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

		inline Camera& GetWorldCamera()
		{
			return m_camera;
		}

		inline SimulationSettings& GetSimSettings()
		{
			return m_simulationSettings;
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
		Camera								 m_camera			  = {};
		SimulationSettings					 m_simulationSettings = {};

		float	 m_elapsedTime		  = 0.0f;
		float	 m_interpolationAlpha = 0.0f;
		PlayMode m_playMode			  = PlayMode::None;
	};

	LINA_RESOURCE_BEGIN(EntityWorld)
	LINA_CLASS_END(EntityWorld)

} // namespace Lina
