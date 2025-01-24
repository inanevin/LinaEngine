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
#include "Common/Data/Functional.hpp"
#include "Common/Memory/AllocatorBucket.hpp"

namespace Lina
{
	class EntityWorld;
	class Component;

#define COMPONENT_POOL_SIZE 100

	class ComponentCacheBase
	{
	public:
		ComponentCacheBase()		  = default;
		virtual ~ComponentCacheBase() = default;

		virtual void	   LoadFromStream(IStream& stream, const Vector<Entity*>& entities) = 0;
		virtual void	   SaveToStream(OStream& stream)									= 0;
		virtual Component* Get(Entity* e) const												= 0;
		virtual void	   Destroy(Entity* e)												= 0;
		virtual void	   Destroy(Component* c)											= 0;
		virtual void	   ForEach(Delegate<void(Component* c)>&& cb)						= 0;
		virtual Component* CreateRaw()														= 0;
	};

	template <typename T> class ComponentCache : public ComponentCacheBase
	{
	public:
		ComponentCache()
		{
		}

		virtual ~ComponentCache()
		{
		}

		inline T* Create()
		{
			return m_componentBucket.Allocate();
		}

		virtual Component* CreateRaw()
		{
			T* t = m_componentBucket.Allocate();
			return t;
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
				m_componentBucket.Free(component);
		}

		virtual T* Get(Entity* e) const override
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

		virtual void Destroy(Component* c) override
		{
			m_componentBucket.Free(static_cast<T*>(c));
		}

		virtual void SaveToStream(OStream& stream) override
		{
			uint32 totalCount = m_componentBucket.GetActiveItemCount();
			stream << totalCount;

			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				comp->SaveToStream(stream);
				stream << comp->GetEntity()->GetGUID();
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
				EntityID entityID = 0;
				stream >> entityID;

				auto it = linatl::find_if(entities.begin(), entities.end(), [entityID](Entity* e) -> bool { return e->GetGUID() == entityID; });
				if (it == entities.end())
				{
					LINA_ERR("Could not found component entity!");
				}
				else
					comp->m_entity = *it;
			}
		}

		inline AllocatorBucket<T, COMPONENT_POOL_SIZE>& GetBucket()
		{
			return m_componentBucket;
		}

		virtual void ForEach(Delegate<void(Component* c)>&& cb) override
		{
			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				cb(comp);
				return false;
			});
		}

	private:
		AllocatorBucket<T, COMPONENT_POOL_SIZE> m_componentBucket;
	};

} // namespace Lina
