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
#include "Core/World/EntityWorldListener.hpp"
#include "Core/World/Screen.hpp"
#include "Core/World/Camera.hpp"
#include "Core/World/WorldInput.hpp"
#include "Common/Memory/MemoryAllocatorPool.hpp"
#include "Common/ObjectWrapper.hpp"
#include "Common/Data/Deque.hpp"
#include "Core/World/CommonWorld.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"

namespace Lina
{
	class Model;
	class Entity;
	class Component;
	class ProjectData;
	class ResourceManagerV2;
	class PhysicsWorld;
	class WorldRenderer;
	class EntityTemplate;
	class ProjectData;

	class EntityWorld : public Resource
	{
	public:
		struct SimulationSettings
		{
			uint32 targetUpdateTicks = 60;
		};

		static constexpr uint32 VERSION = 0;

		EntityWorld(const EntityWorld& other) = delete;
		EntityWorld(ResourceID id, const String& name);
		~EntityWorld();

		void Initialize(ResourceManagerV2* rm, LinaGX::Window* window, ProjectData* projectData);
		void Tick(float delta);
		void SetPlayMode(PlayMode playmode);

		// Resource
		virtual void	   SaveToStream(OStream& stream) const override;
		virtual void	   LoadFromStream(IStream& stream) override;
		virtual bool	   LoadFromFile(const String& path) override;
		void			   CollectResourceNeeds(HashSet<ResourceID>& outResources) const;
		HashSet<Resource*> LoadMissingResources(ResourceManagerV2& rm, ProjectData* project, const HashSet<ResourceID>& extraResources);

		// Entity
		Entity* CreateEntity(EntityID id, const String& name = "");
		Entity* GetEntity(EntityID guid);
		void	DestroyEntity(Entity* e);
		Entity* FindEntity(const String& name);
		Entity* SpawnTemplate(EntityTemplate* tmp);

		// Components
		void	   GetComponents(Entity* e, Vector<Component*>& outComponents) const;
		Component* GetComponent(Entity* e, TypeID tid) const;
		Component* AddComponent(Entity* e, TypeID tid);
		void	   RefreshComponentReferences(const Vector<Entity*>& entities);
		void	   RefreshAllComponentReferences();

		// Misc
		void AddListener(EntityWorldListener* listener);
		void RemoveListener(EntityWorldListener* listener);

		template <typename T> T* GetComponent(Entity* e)
		{
			return static_cast<T*>(GetCache<T>()->Get(e));
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

			auto it = linatl::find_if(m_componentCaches.begin(), m_componentCaches.end(), [tid](const ComponentCachePair& pair) -> bool { return tid == pair.tid; });

			if (it == m_componentCaches.end())
			{
				ComponentCache<T>* cache = new ComponentCache<T>();
				m_componentCaches.push_back({tid, cache});
				return cache;
			}
			return static_cast<ComponentCache<T>*>(it->cache);
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

		inline WorldGfxSettings& GetGfxSettings()
		{
			return m_gfxSettings;
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

		inline ResourceManagerV2* GetResourceManager()
		{
			return m_rm;
		}

		inline PhysicsWorld* GetPhysicsWorld()
		{
			return m_physicsWorld;
		}

		inline WorldPhysicsSettings& GetPhysicsSettings()
		{
			return m_physicsSettings;
		}

		inline void SetWorldRenderer(WorldRenderer* wr)
		{
			m_worldRenderer = wr;
		}

		inline WorldRenderer* GetWorldRenderer() const
		{
			return m_worldRenderer;
		}

		inline PlayMode GetPlayMode() const
		{
			return m_playMode;
		}

		inline LinaVG::Drawer& GetLVGDrawer()
		{
			return m_lvgDrawer;
		}

		inline const Vector<ResourceDef>& GetNeededResourceDefinitions() const
		{
			return m_neededResourceDefinitions;
		}

	private:
		void				BeginPlay();
		void				EndPlay();
		void				DestroyEntityData(Entity* e);
		void				OnCreateComponent(Component* c, Entity* e);
		void				OnDestroyComponent(Component* c, Entity* e);
		void				DestroyComponentCaches();
		ComponentCacheBase* GetCache(TypeID tid);
		ComponentCacheBase* GetCache(TypeID tid) const;

	private:
		struct ComponentCachePair
		{
			TypeID				tid	  = 0;
			ComponentCacheBase* cache = nullptr;
		};

	private:
		ALLOCATOR_BUCKET_MEM;
		AllocatorBucket<Entity, 1000> m_entityBucket;
		Vector<ComponentCachePair>	  m_componentCaches;
		EntityID					  m_entityGUIDCounter = 1;
		PhysicsWorld*				  m_physicsWorld	  = nullptr;
		Bitmask32					  m_flags			  = 0;
		Vector<EntityWorldListener*>  m_listeners;
		WorldGfxSettings			  m_gfxSettings;
		Screen						  m_screen = {};
		WorldInput					  m_worldInput;
		Camera						  m_camera			   = {};
		SimulationSettings			  m_simulationSettings = {};
		ResourceManagerV2*			  m_rm				   = nullptr;

		float				 m_elapsedTime		  = 0.0f;
		float				 m_interpolationAlpha = 0.0f;
		PlayMode			 m_playMode			  = PlayMode::None;
		Vector<ResourceDef>	 m_neededResourceDefinitions;
		WorldPhysicsSettings m_physicsSettings = {};
		WorldRenderer*		 m_worldRenderer   = nullptr;
		LinaVG::Drawer		 m_lvgDrawer	   = {};
		ProjectData*		 m_projectData	   = nullptr;
	};

	LINA_RESOURCE_BEGIN(EntityWorld)
	LINA_CLASS_END(EntityWorld)

} // namespace Lina
