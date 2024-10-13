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

		virtual void PlayBegin()													  = 0;
		virtual void PlayEnd()														  = 0;
		virtual void PreTick(uint32 flags)											  = 0;
		virtual void Tick(float delta, uint32 flags)								  = 0;
		virtual void PostTick(float delta, uint32 flags)							  = 0;
		virtual void LoadFromStream(IStream& stream, const Vector<Entity*>& entities) = 0;
		virtual void SaveToStream(OStream& stream)									  = 0;
		virtual void Destroy(Entity* e)												  = 0;
		virtual void Destroy(Component* c)											  = 0;
		virtual void ForEach(Delegate<void(Component* c)>&& cb)						  = 0;
	};

	template <typename T> class ComponentCache : public ComponentCacheBase
	{
	public:
		ComponentCache(EntityWorld* world) : m_world(world)
		{
		}

		virtual ~ComponentCache()
		{
		}

		virtual void PlayBegin() override
		{
			m_componentBucket.View([](T* comp, uint32 index) -> bool {
				comp->OnEvent({.type = ComponentEventType::PlayBegin});
				return false;
			});
		}

		virtual void PlayEnd() override
		{
			m_componentBucket.View([](T* comp, uint32 index) -> bool {
				comp->OnEvent({.type = ComponentEventType::PlayEnd});
				return false;
			});
		}

		virtual void PreTick(uint32 flags) override
		{
			m_componentBucket.View([flags](T* comp, uint32 index) -> bool {
				if (comp->GetFlags().IsSet(flags))
					comp->OnEvent({.type = ComponentEventType::PreTick});
				return false;
			});
		}

		virtual void Tick(float delta, uint32 flags) override
		{
			m_componentBucket.View([delta, flags](T* comp, uint32 index) -> bool {
				if (comp->GetFlags().IsSet(flags))
					comp->OnEvent({.type = ComponentEventType::Tick, .data = delta});
				return false;
			});
		}

		virtual void PostTick(float delta, uint32 flags) override
		{
			m_componentBucket.View([delta, flags](T* comp, uint32 index) -> bool {
				if (comp->GetFlags().IsSet(flags))
					comp->OnEvent({.type = ComponentEventType::PostTick, .data = delta});
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
				m_componentBucket.Free(component);
		}

		virtual void Destroy(Component* c) override
		{
			m_componentBucket.Free(static_cast<T*>(c));
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

			MetaType& meta = ReflectionSystem::Get().Resolve<T>();

			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				meta.GetFunction<void(OStream&, void*)>("SaveToStream"_hs)(stream, comp);
				stream << comp->GetEntity()->GetTransientID();
				return false;
			});
		}

		virtual void LoadFromStream(IStream& stream, const Vector<Entity*>& entities) override
		{
			uint32 totalCount = 0;
			stream >> totalCount;

			MetaType& meta = ReflectionSystem::Get().Resolve<T>();

			for (uint32 i = 0; i < totalCount; i++)
			{
				T* comp = Create();
				meta.GetFunction<void(OStream&, void*)>("LoadFromStream"_hs)(stream, comp);
				uint32 entityID = 0;
				stream >> entityID;
				comp->m_entity = entities[entityID];
			}
		}

		virtual void ForEach(Delegate<void(Component* c)>&& cb) override
		{
			m_componentBucket.View([&](T* comp, uint32 index) -> bool {
				cb(comp);
				return false;
			});
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
