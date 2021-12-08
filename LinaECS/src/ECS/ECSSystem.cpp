/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "ECS/ECSSystem.hpp"  
#include "Utility/Log.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "..\..\include\ECS\Components\EntityDataComponent.hpp"

namespace LinaEngine::ECS
{
	bool ECSSystemList::RemoveSystem(BaseECSSystem& system)
	{
		for (unsigned int i = 0; i < m_systems.size(); i++)
		{
			// If the addr of the target system matches any system, erase it from the array.
			if (&system == m_systems[i])
			{
				m_systems.erase(m_systems.begin() + i);
				return true;
			}
		}

		return false;
	}

	ECSRegistry::~ECSRegistry()
	{
		on_construct<EntityDataComponent>().disconnect(this);
	}

	void ECSRegistry::Initialize()
	{
		on_construct<EntityDataComponent>().connect<&ECSRegistry::OnEntityDataComponentAdded>(this);
	}

	void ECSRegistry::OnEntityDataComponentAdded(entt::registry& reg, entt::entity ent)
	{
		auto& data = reg.get<EntityDataComponent>(ent);
		data.m_ecs = this;
	}

	void ECSRegistry::AddChildToEntity(ECSEntity parent, ECSEntity child)
	{
		if (parent == child) return;

		EntityDataComponent& childData = get<EntityDataComponent>(child);
		EntityDataComponent& parentData = get<EntityDataComponent>(parent);

		if (parentData.m_parent == child || childData.m_parent == parent) return;

		if (childData.m_parent != entt::null)
		{
			RemoveChildFromEntity(childData.m_parent, child);
		}

		parentData.m_children.emplace(child);
		childData.m_parent = parent;
	}

	void ECSRegistry::DestroyAllChildren(ECSEntity parent)
	{
		EntityDataComponent* data = try_get<EntityDataComponent>(parent);
		LINA_CORE_TRACE("Destroy All Children called");

		if (data == nullptr) return;

		int counter = 0;
		std::set<ECSEntity> children = data->m_children;
		std::set<ECSEntity>::iterator it;
		for (it = children.begin(); it != children.end(); ++it)
		{
			LINA_CORE_TRACE("Destroy Entity Called {0}", counter);
			DestroyEntity(*it);
			counter++;
		}
		data->m_children.clear();
		LINA_CORE_TRACE("Child count {0}", get<EntityDataComponent>(parent).m_children.size());
	}

	void ECSRegistry::RemoveChildFromEntity(ECSEntity parent, ECSEntity child)
	{
		std::set<ECSEntity>& children = get<EntityDataComponent>(parent).m_children;
		if (children.find(child) != children.end())
		{
			children.erase(child);
		}

		get<EntityDataComponent>(child).m_parent = entt::null;

	}

	void ECSRegistry::RemoveFromParent(ECSEntity child)
	{
		ECSEntity parent = get<EntityDataComponent>(child).m_parent;

		if (parent != entt::null)
			RemoveChildFromEntity(parent, child);

	}

	void ECSRegistry::CloneEntity(ECSEntity from, ECSEntity to)
	{
		visit(from, [this, from, to](const auto component)
			{
				m_cloneComponentFunctions[component](from, to);
			});
	}

	const std::set<ECSEntity>& ECSRegistry::GetChildren(ECSEntity parent)
	{
		return get<EntityDataComponent>(parent).m_children;
	}

	ECSEntity ECSRegistry::CreateEntity(const std::string& name)
	{
		entt::entity ent = create();
		emplace<EntityDataComponent>(ent, EntityDataComponent( false, true, true, name ));
		return ent;
	}

	ECSEntity ECSRegistry::CreateEntity(ECSEntity source, bool attachParent)
	{
		EntityDataComponent sourceData = get<EntityDataComponent>(source);

		// Build the entity.
		ECSEntity copy = create();

		// Copy entity components to newly created one
		CloneEntity(source, copy);

		//EntityDataComponent& copyData = get<EntityDataComponent>(copy);
		get<EntityDataComponent>(copy).m_parent = entt::null;
		get<EntityDataComponent>(copy).m_children.clear();

		for (ECSEntity child : sourceData.m_children)
		{
			ECSEntity copyChild = CreateEntity(child, false);
			EntityDataComponent& copyChildData = get<EntityDataComponent>(copyChild);
			copyChildData.m_parent = copy;
			get<EntityDataComponent>(copy).m_children.emplace(copyChild);
		}

		if (attachParent && sourceData.m_parent != entt::null)
			AddChildToEntity(sourceData.m_parent, copy);

		return copy;
	}

	ECSEntity ECSRegistry::GetEntity(const std::string& name)
	{
		auto singleView = view<EntityDataComponent>();

		for (auto entity : singleView)
		{
			if (singleView.get<EntityDataComponent>(entity).m_name.compare(name) == 0)
				return entity;
		}

		LINA_CORE_WARN("Entity with the name {0} could not be found, returning null entity.", name);
		return entt::null;
	}

	void ECSRegistry::DestroyEntity(ECSEntity entity, bool isRoot)
	{
		std::set<ECSEntity> toErase;
		for (ECSEntity child : get<EntityDataComponent>(entity).m_children)
		{
			toErase.emplace(child);
			DestroyEntity(child, false);
		}

		for (ECSEntity child : toErase)
			get<EntityDataComponent>(entity).m_children.erase(child);

		if (isRoot)
			RemoveFromParent(entity);

		destroy(entity);
	}

}

