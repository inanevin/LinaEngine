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

Defines ECSRegistry wrapper, base ECS system class that defines functions for updating entity components
as well as an ECS Systems class responsible for iterating & calling update functions of containted systems.

Timestamp: 4/8/2019 5:28:34 PM
*/

#pragma once

#ifndef ECSSystem_HPP
#define ECSSystem_HPP

#include "Core/Common.hpp"
#include "entt/entity/registry.hpp"
#include "entt/entity/entity.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <map>

namespace LinaEngine::ECS
{

	typedef entt::entity ECSEntity;
	typedef entt::id_type ECSTypeID;

	template<typename T>
	ECSTypeID GetTypeID()
	{
		return entt::type_info<T>::id();
	}
	
	class ECSRegistry : public entt::registry
	{
	public:

		ECSRegistry() {  };
		virtual ~ECSRegistry() {};

		ECSEntity CreateEntity(const std::string& name);
		void SetEntityName(ECSEntity entity, const std::string& name);
		std::string& GetEntityName(ECSEntity e);

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_entityNameMap); 
		}

	private:
		std::map<entt::entity, std::string> m_entityNameMap;
	};
	

	class BaseECSSystem
	{
	public:

		BaseECSSystem() {};

		virtual void UpdateComponents(float delta) = 0;

	protected:

		virtual void Construct(ECSRegistry& reg) { m_ecs = &reg; };
		ECSRegistry* m_ecs = nullptr;

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