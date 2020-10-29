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
#include "ECS/Components/TransformComponent.hpp"

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

	void ECSRegistry::Refresh()
	{

		auto singleView = view<ECSEntityData>();

		for (ECSEntity entity : singleView)
		{
			AddEntityChildTransforms(entity);
		}
	}

	void ECSRegistry::AddChildToEntity(ECSEntity parent, ECSEntity child)
	{
		if (parent == child) return;

		ECSEntityData& childData = get<ECSEntityData>(child);
		ECSEntityData& parentData = get<ECSEntityData>(parent);

		if (parentData.m_parent == child || childData.m_parent == parent) return;

		Transformation* childTransform = &get<TransformComponent>(child).transform;
		Transformation* parentTransform = &get<TransformComponent>(parent).transform;

		if (childData.m_parent != entt::null)
		{
			RemoveChildFromEntity(childData.m_parent, child);
			childTransform->RemoveFromParent();
		}

		parentTransform->AddChild(childTransform);
		parentData.m_children.emplace(child);
		childData.m_parent = parent;
	}

	void ECSRegistry::RemoveChildFromEntity(ECSEntity parent, ECSEntity child)
	{
		std::set<ECSEntity>& children = get<ECSEntityData>(parent).m_children;
		if (children.find(child) != children.end())
		{
			children.erase(child);

			// Remove transformation.
			get<TransformComponent>(parent).transform.RemoveChild(&get<TransformComponent>(child).transform);
		}

		get<ECSEntityData>(child).m_parent = entt::null;

	}

	void ECSRegistry::RemoveFromParent(ECSEntity child)
	{
		ECSEntity parent = get<ECSEntityData>(child).m_parent;

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

	const std::set<ECSEntity> ECSRegistry::GetChildren(ECSEntity parent)
	{
		return get<ECSEntityData>(parent).m_children;
	}

	ECSEntity ECSRegistry::CreateEntity(const std::string& name)
	{
		entt::entity ent = create();
		emplace<ECSEntityData>(ent, ECSEntityData{ false, false, name });
		emplace<TransformComponent>(ent, TransformComponent());
		Refresh();
		return ent;
	}

	ECSEntity ECSRegistry::CreateEntity(ECSEntity source, bool attachParent)
	{
		ECSEntityData sourceData = get<ECSEntityData>(source);

		// Create the entity.
		ECSEntity copy = create();

		// Copy entity components to newly created one
		CloneEntity(source, copy);

		//ECSEntityData& copyData = get<ECSEntityData>(copy);
		get<ECSEntityData>(copy).m_parent = entt::null;
		get<ECSEntityData>(copy).m_children.clear();
		Refresh();


		for (ECSEntity child : sourceData.m_children)
		{
			ECSEntity copyChild = CreateEntity(child, false);
			ECSEntityData& copyChildData = get<ECSEntityData>(copyChild);
			copyChildData.m_parent = copy;
			get<ECSEntityData>(copy).m_children.emplace(copyChild);
		}

		if (attachParent && sourceData.m_parent != entt::null)
			AddChildToEntity(sourceData.m_parent, copy);

		Refresh();

		return copy;

	}

	ECSEntity ECSRegistry::GetEntity(const std::string& name)
	{
		auto singleView = view<ECSEntityData>();

		for (auto entity : singleView)
		{
			if (singleView.get<ECSEntityData>(entity).m_name.compare(name) == 0)
				return entity;
		}

		LINA_CORE_WARN("Entity with the name {0} could not be found, returning null entity.", name);
		return entt::null;
	}

	void ECSRegistry::DestroyEntity(ECSEntity entity, bool isRoot)
	{
		std::set<ECSEntity> toErase;
		for (ECSEntity child : get<ECSEntityData>(entity).m_children)
		{
			toErase.emplace(child);
			DestroyEntity(child, false);
		}

		for (ECSEntity child : toErase)
			get<ECSEntityData>(entity).m_children.erase(child);

		if (isRoot)
			RemoveFromParent(entity);

		destroy(entity);
	}

	void ECSRegistry::AddEntityChildTransforms(ECSEntity entity)
	{
		ECSEntityData data = get<ECSEntityData>(entity);
		get<TransformComponent>(entity).transform.m_children.clear();
		get<TransformComponent>(entity).transform.m_parent = data.m_parent == entt::null ? nullptr : &get<TransformComponent>(data.m_parent).transform;

		for (ECSEntity child : data.m_children)
		{
			get<TransformComponent>(entity).transform.AddChild(&get<TransformComponent>(child).transform);
			AddEntityChildTransforms(child);
		}
	}

}

