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

#include "World/Core/EntityWorld.hpp"
#include "World/Core/Entity.hpp"
#include "World/Core/Component.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Serialization/VectorSerialization.hpp"

namespace Lina
{
#define ENTITY_VEC_SIZE_CHUNK 2000

	uint32 EntityWorld::s_worldCounter = 0;

	void EntityWorld::DestroyWorld()
	{
		for (auto* e : m_entities)
		{
			if (e != nullptr)
			{
				e->~Entity();
				m_allocatorPool.Free(e);
			}
		}

		for (auto& [tid, cache] : m_componentCaches)
			delete cache;

		m_entities.Clear();
		m_componentCaches.clear();
	}

	Entity* EntityWorld::GetEntity(uint32 id)
	{
		return m_entities.GetItem(id);
	}

	Entity* EntityWorld::GetEntity(const String& name)
	{
		for (auto* e : m_entities)
		{
			if (e != nullptr && e->GetName().compare(name) == 0)
				return e;
		}

		auto it = linatl::find_if(m_entities.begin(), m_entities.end(), [&name](Entity* e) { return e != nullptr && e->GetName().compare(name) == 0; });
		return *it;
	}

	Entity* EntityWorld::GetEntityFromSID(StringID sid)
	{
		auto it = linatl::find_if(m_entities.begin(), m_entities.end(), [sid](Entity* e) { return e != nullptr && e->GetSID() == sid; });
		return *it;
	}

	void EntityWorld::CopyFrom(EntityWorld& world)
	{
		DestroyWorld();
		m_entities.Clear();

		for (auto e : world.m_entities)
		{
			if (e != nullptr)
			{
				Entity* newE = new (m_allocatorPool.Allocate(sizeof(Entity))) Entity();
				*newE		 = *e;
				m_entities.AddItem(newE, newE->m_id);
			}
		}

		for (auto& [tid, cache] : world.m_componentCaches)
		{
			ComponentCacheBase* c  = cache->CopyCreate();
			m_componentCaches[tid] = c;
		}
	}

	Entity* EntityWorld::CreateEntity(const String& name)
	{
		Entity* e = new (m_allocatorPool.Allocate(sizeof(Entity))) Entity();
		e->SetVisible(true);
		e->SetStatic(true);
		e->m_world = this;
		e->m_name  = name;
		e->m_sid   = TO_SID(name);
		e->m_id	   = m_entities.AddItem(e);

		if (m_entities.GetItems().size() > ENTITY_MAX)
		{
			LINA_ERR("Reached maximum number of entities!");
			LINA_ASSERT(false, "");
			return nullptr;
		}

		return e;
	}

	void EntityWorld::DestroyEntity(Entity* e)
	{
		if (e->m_parentID != ENTITY_NULL)
			e->m_parent->RemoveChild(e);

		DestroyEntityData(e);
	}

	void EntityWorld::DestroyEntityData(Entity* e)
	{
		for (auto child : e->m_children)
			DestroyEntityData(child);

		for (auto& [tid, cache] : m_componentCaches)
			cache->OnEntityDestroyed(e);

		const uint32 id = e->m_id;
		m_entities.RemoveItem(id);
		e->~Entity();
		m_allocatorPool.Free(e);
	}

	void EntityWorld::Tick(float delta)
	{
		Event data;
		data.fParams[0] = delta;
		DispatchEvent(EVG_Tick, data);
		DispatchEvent(EVG_PostTick, data);
		m_physicsWorld.Tick(delta);
	}

	void EntityWorld::WaitForSimulation()
	{
		m_physicsWorld.WaitForSimulation();
	}

	void EntityWorld::SyncData(float alpha)
	{
		m_physicsWorld.SyncData(alpha);
	}

	void EntityWorld::SaveToStream(OStream& stream)
	{
		m_entities.SaveToStream(stream);

		Vector<Entity*> entityObjects;
		for (auto e : m_entities)
		{
			if (e != nullptr)
				entityObjects.push_back(e);
		}

		const uint32 entitiesSize = static_cast<uint32>(entityObjects.size());
		stream << entitiesSize;

		for (auto e : entityObjects)
			e->SaveToStream(stream);
		entityObjects.clear();

		Vector<TypeID> tids;
		for (auto& [tid, cache] : m_componentCaches)
			tids.push_back(tid);

		VectorSerialization::SaveToStream_PT(stream, tids);

		for (auto& [tid, cache] : m_componentCaches)
			cache->SaveToStream(stream);
	}

	void EntityWorld::LoadFromStream(IStream& stream)
	{
		// Load id list.
		m_entities.LoadFromStream(stream);

		// Load entityObjects.
		uint32 entitiesSize = 0;
		stream >> entitiesSize;
		for (uint32 i = 0; i < entitiesSize; i++)
		{
			Entity* e = new (m_allocatorPool.Allocate(sizeof(Entity))) Entity();
			e->LoadFromStream(stream);
			e->m_world = this;
			m_entities.AddItem(e, e->m_id);
		}

		// Load parent-child hierarchy
		for (auto e : m_entities)
		{
			if (e != nullptr)
			{
				auto& childrenIDs = e->m_childrenID;

				for (auto& childID : childrenIDs)
					e->m_children.insert(m_entities.GetItem(childID));

				if (e->m_parentID != ENTITY_NULL)
					e->m_parent = m_entities.GetItem(e->m_parentID);
			}
		}

		Vector<TypeID> tids;
		VectorSerialization::LoadFromStream_PT(stream, tids);

		for (uint32 i = 0; i < tids.size(); i++)
		{
			MetaType&			type  = ReflectionSystem::Get().Resolve(tids[i]);
			void*				ptr	  = type.GetFunction<void*(EntityWorld*, IGameEventDispatcher*)>("CreateCompCache"_hs)(this, this);
			ComponentCacheBase* cache = static_cast<ComponentCacheBase*>(ptr);
			cache->m_entities		  = m_entities.GetRaw();
			cache->LoadFromStream(stream);
			m_componentCaches[tids[i]] = cache;
		}
	}

} // namespace Lina
