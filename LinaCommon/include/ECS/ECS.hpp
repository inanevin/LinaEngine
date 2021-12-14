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

/*
Class: ECSSystem

Defines Registry wrapper, base ECS system class that defines functions for updating entity components
as well as an ECS Systems class responsible for iterating & calling update functions of containted systems.

Timestamp: 4/8/2019 5:28:34 PM
*/

#pragma once

#ifndef ECSSystem_HPP
#define ECSSystem_HPP

#include "Core/Common.hpp"
#include "Math/Transformation.hpp"

#define ENTT_USE_ATOMIC
#include <entt/config/config.h>
#include <entt/entity/snapshot.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/entity.hpp>


#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/set.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <map>
#include <set>

namespace Lina
{
	class Application;
}

namespace Lina::ECS
{

#define ECSNULL entt::null

	typedef entt::entity Entity;
	typedef entt::id_type TypeID;
	typedef entt::delegate<void(entt::snapshot&, cereal::PortableBinaryOutputArchive&)> ComponentSerializeFunction;
	typedef entt::delegate<void(entt::snapshot_loader&, cereal::PortableBinaryInputArchive&)> ComponentDeserializeFunction;

	template<typename T>
	TypeID GetTypeID()
	{
		return entt::type_hash<T>::value();
	}
	
	class Registry : public entt::registry
	{
	public:

		FORCEINLINE static Registry* Get() { return s_ecs; }

		// Registering a component enables serialization as well as duplication functionality in & out editor.
		template<typename T>
		void RegisterComponent(bool onlyClone = false)
		{
			TypeID id = GetTypeID<T>();
			if (!onlyClone)
			{
				m_serializeFunctions[id].first.connect<&Registry::SerializeComponent<T>>(this);
				m_serializeFunctions[id].second.connect<&Registry::DeserializeComponent<T>>(this);
			}
		
			m_cloneComponentFunctions[id] = std::bind(&Registry::CloneComponent<T>, this, std::placeholders::_1, std::placeholders::_2);
		}

		void SerializeComponentsInRegistry(cereal::PortableBinaryOutputArchive& archive);
		void DeserializeComponentsInRegistry(cereal::PortableBinaryInputArchive& archive);
		void AddChildToEntity(Entity parent, Entity child);
		void DestroyAllChildren(Entity parent);
		void RemoveChildFromEntity(Entity parent, Entity child);
		void RemoveFromParent(Entity child);
		void CloneEntity(Entity from, Entity to);
		void DestroyEntity(Entity entity, bool isRoot = true);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(Entity copy, bool attachParent = true);
		Entity GetEntity(const std::string& name);
		const std::set<Entity>& GetChildren(Entity parent);

	private:

		friend class Lina::Application;
		Registry() {};
		virtual ~Registry() {};
		void Initialize();
		void Shutdown();

	private:


		template<typename Type>
		void CloneComponent(Entity from, Entity to)
		{
			Type component = get<Type>(from);
			emplace<Type>(to, component);
		}

		template<typename Type>
		void SerializeComponent(entt::snapshot& snapshot, cereal::PortableBinaryOutputArchive& archive)
		{
			snapshot.component<Type>(archive);
		}

		template<typename Type>
		void DeserializeComponent(entt::snapshot_loader& loader, cereal::PortableBinaryInputArchive& archive)
		{
			loader.component<Type>(archive);
		}

		void OnEntityDataComponentAdded(entt::registry& reg, entt::entity ent);

	private:

		static Registry* s_ecs;
		std::unordered_map<TypeID, std::pair<ComponentSerializeFunction, ComponentDeserializeFunction>> m_serializeFunctions;
		std::map<TypeID, std::function<void(Entity, Entity)>> m_cloneComponentFunctions;

	};
	

	class BaseECSSystem
	{
	public:

		BaseECSSystem() {};

		virtual void Initialize();
		virtual void UpdateComponents(float delta) = 0;
		virtual void SystemActivation(bool active) { m_isActive = active; }

	protected:

		Registry* m_ecs = nullptr;
		bool m_isActive = false;

	};

	class ECSSystemList
	{
	public:

		bool AddSystem(BaseECSSystem& system)
		{
			m_systems.push_back(&system);
			return true;
		}

		void UpdateSystems(float delta)
		{
			for (auto s : m_systems)
				s->UpdateComponents(delta);
		}

		bool RemoveSystem(BaseECSSystem& system);

	private:

		std::vector<BaseECSSystem*> m_systems;

	};
}


#endif