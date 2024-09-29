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

#include "CommonWorld.hpp"
#include "Entity.hpp"
#include "Common/SizeDefinitions.hpp"
#include "Common/Memory/AllocatorBucket.hpp"

namespace Lina
{
	class EntityWorld;

#define COMPONENT_POOL_SIZE 100

	class ComponentCacheBase
	{
	public:
		ComponentCacheBase()		  = default;
		virtual ~ComponentCacheBase() = default;

		virtual void PreTick()														  = 0;
		virtual void Tick(float delta)												  = 0;
		virtual void PostTick(float delta)											  = 0;
		virtual void LoadFromStream(IStream& stream, const Vector<Entity*>& entities) = 0;
		virtual void SaveToStream(OStream& stream)									  = 0;
		virtual void Destroy(Entity* e)												  = 0;
	};

	template <typename T> class ComponentCache : public ComponentCacheBase
	{
	public:
		ComponentCache(EntityWorld* world) : m_world(world)
		{
		}

		virtual ~ComponentCache()
		{
			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				comp->Destroy();
				return false;
			});
		}

		virtual void PreTick() override
		{
			m_componentBucket.View([](T* comp, uint32 index) -> bool {
				comp->PreTick();
				return false;
			});
		}

		virtual void Tick(float delta) override
		{
			m_componentBucket.View([delta](T* comp, uint32 index) -> bool {
				comp->Tick(delta);
				return false;
			});
		}

		virtual void PostTick(float delta) override
		{
			m_componentBucket.View([delta](T* comp, uint32 index) -> bool {
				comp->PostTick(delta);
				return false;
			});
		}

		inline T* Create()
		{
			return m_componentBucket.Allocate();
		}

		virtual void Destroy(Entity* e) override
		{
			T* component = nullptr;
			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				if (comp->m_entity == e)
				{
					component = comp;
					return true;
				}
				return false;
			});

			if (component != nullptr)
			{
				component->Destroy();
				m_componentBucket.Free(component);
			}
		}

		inline T* Get(Entity* e)
		{
			T* component = nullptr;
			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				if (comp->m_entity == e)
				{
					component = comp;
					return true;
				}
				return false;
			});

			return component;
		}

		virtual void SaveToStream(OStream& stream) override
		{
			uint32 totalCount = m_componentBucket.GetActiveItemCount();
			stream << totalCount;

			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				comp->SaveToStream(stream);
				stream << comp->GetEntity()->GetTransientID();
				return false;
			});
		}

		virtual void LoadFromStream(IStream& stream, const Vector<Entity*>& entities) override
		{
			uint32 totalCount = 0;
			stream >> totalCount;

			for (uint32 i = 0; i < totalCount; i++)
			{
				T* comp = Create();
				comp->LoadFromStream(stream);
				uint32 entityID = 0;
				stream >> entityID;
				comp->m_entity = entities[entityID];
			}
		}

		void View(Delegate<bool(T* comp, uint32 index)>&& callback)
		{
			m_componentBucket.View(std::move(callback));
		}

	private:
		EntityWorld*			m_world = nullptr;
		AllocatorBucket<T, 100> m_componentBucket;
	};

} // namespace Lina
