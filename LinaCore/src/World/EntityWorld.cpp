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

#include "Core/World/EntityWorld.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/Component.hpp"
#include "Core/Reflection/ReflectionSystem.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/System/System.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina
{
#define ENTITY_VEC_SIZE_CHUNK 2000

	EntityWorld::EntityWorld(const String& path, StringID sid)
		: Resource(path, sid, GetTypeID<EntityWorld>()), m_physicsWorld(this){

														 };

	EntityWorld::~EntityWorld()
	{
		m_resourceManagerV2.Shutdown();

		m_entityBucket.View([this](Entity* e, uint32 index) -> bool {
			if (e->GetParent() == nullptr)
				DestroyEntity(e);
			return false;
		});

		for (Pair<TypeID, ComponentCacheBase*> pair : m_componentCaches)
		{
			delete pair.second;
		}
	}

	Entity* EntityWorld::CreateEntity(const String& name)
	{
		const uint32 id = m_entityBucket.GetActiveItemCount();
		Entity*		 e	= m_entityBucket.Allocate();
		e->SetVisible(true);
		e->SetStatic(true);
		e->m_world = this;
		e->m_name  = name;
		return e;
	}

	void EntityWorld::DestroyEntity(Entity* e)
	{
		if (e->m_parent != nullptr)
			e->m_parent->RemoveChild(e);
		DestroyEntityData(e);
	}

	void EntityWorld::DestroyEntityData(Entity* e)
	{
		for (auto child : e->m_children)
			DestroyEntityData(child);

		for (auto& [tid, cache] : m_componentCaches)
		{
			cache->Destroy(e);
		}

		m_entityBucket.Free(e);
	}

	void EntityWorld::Simulate(float fixedDelta)
	{
		m_physicsWorld.Simulate();
	}

	void EntityWorld::PreTick()
	{
		m_resourceManagerV2.Poll();

		for (const auto& [tid, cache] : m_componentCaches)
		{
			cache->PreTick();
		}
	}

	void EntityWorld::Tick(float deltaTime)
	{
		for (const auto& [tid, cache] : m_componentCaches)
		{
			cache->Tick(deltaTime);
		}

		for (const auto& [tid, cache] : m_componentCaches)
		{
			cache->PostTick(deltaTime);
		}
	}

	void EntityWorld::WaitForSimulation()
	{
		m_physicsWorld.WaitForSimulation();
	}

	void EntityWorld::SaveToStream(OStream& stream) const
	{
		stream << VERSION;
		stream << m_id;

		const uint32 entitiesSize = m_entityBucket.GetActiveItemCount();
		stream << entitiesSize;

		m_entityBucket.View([&](Entity* e, uint32 index) -> bool {
			e->SaveToStream(stream);
			e->m_transientID = index;
			return false;
		});

		m_entityBucket.View([&](Entity* e, uint32 index) -> bool {
			if (e->GetParent() != nullptr)
				stream << index + 1;
			else
				stream << 0;
			return false;
		});

		const uint32 cacheSize = static_cast<uint32>(m_componentCaches.size());
		stream << cacheSize;

		for (auto& [tid, cache] : m_componentCaches)
		{
			stream << tid;
			cache->SaveToStream(stream);
		}
	}

	void EntityWorld::LoadFromFile(const char* path)
	{
		IStream stream = Serialization::LoadFromFile(path);
		if (stream.GetDataRaw() != nullptr)
			LoadFromStream(stream);
		stream.Destroy();
	}

	void EntityWorld::LoadFromStream(IStream& stream)
	{
		uint32 version = 0;
		stream >> version;
		stream >> m_id;

		m_entityBucket.Clear();
		m_componentCaches.clear();

		uint32 entitiesSize = 0;
		stream >> entitiesSize;

		Vector<Entity*> entities;
		entities.resize(entitiesSize);

		for (uint32 i = 0; i < entitiesSize; i++)
		{
			Entity* e = CreateEntity("");
			e->LoadFromStream(stream);
			entities[i] = e;
		}

		for (uint32 i = 0; i < entitiesSize; i++)
		{
			uint32 parentID = 0;
			stream >> parentID;

			if (parentID != 0)
			{
				const uint32 actualID = parentID - 1;
				entities[actualID]->AddChild(entities[i]);
			}
		}

		uint32 cachesSize = 0;
		stream >> cachesSize;

		for (uint32 i = 0; i < cachesSize; i++)
		{
			TypeID tid = 0;
			stream >> tid;

			MetaType&			type  = ReflectionSystem::Get().Resolve(tid);
			void*				ptr	  = type.GetFunction<void*(EntityWorld*, GameEventDispatcher*)>("CreateCompCache"_hs)(this, this);
			ComponentCacheBase* cache = static_cast<ComponentCacheBase*>(ptr);
			cache->LoadFromStream(stream, entities);
			m_componentCaches[tid] = cache;
		}
	}

	void EntityWorld::ProcessComponent(Component* c, Entity* e)
	{
		c->m_input			 = &GfxManager::GetLGX()->GetInput();
		c->m_world			 = this;
		c->m_resourceManager = &m_resourceManagerV2;
		c->m_entity			 = e;
		c->Create();
	}

	void EntityWorld::AddListener(EntityWorldListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void EntityWorld::RemoveListener(EntityWorldListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](EntityWorldListener* list) -> bool { return list == listener; }));
	}
} // namespace Lina
