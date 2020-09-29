/*
Author: Inan Evin - Thanks to the lectures & contributions of Benny Bobaganoosh, thebennybox.
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ECSSystem
Timestamp: 4/8/2019 5:28:34 PM

*/

#pragma once

#ifndef ECSSystem_HPP
#define ECSSystem_HPP


#include "Core/Common.hpp"
#include "entt/entity/registry.hpp"
#include "entt/entity/entity.hpp"
#include <map>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>

namespace LinaEngine::ECS
{

	typedef entt::entity ECSEntity;

	
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
			archive(m_EntityNameMap); // serialize things by passing them to the archive
		}

	private:
		std::map<entt::entity, std::string> m_EntityNameMap;
	};
	

	class BaseECSSystem
	{
	public:

		BaseECSSystem() {};

		virtual void UpdateComponents(float delta) = 0;

	protected:

		virtual void Construct(ECSRegistry& reg) { m_Registry = &reg; };
		ECSRegistry* m_Registry;

	};

	class ECSSystemList
	{
	public:

		/* Adds a system */
		FORCEINLINE bool AddSystem(BaseECSSystem& system)
		{
			systems.push_back(&system);
			return true;
		}

		FORCEINLINE void UpdateSystems(float delta)
		{
			for (auto s : systems)
				s->UpdateComponents(delta);
		}

		/* Remove a system */
		bool RemoveSystem(BaseECSSystem& system);

	private:

		/* Array of EntityComponentSystem systems */
		std::vector<BaseECSSystem*> systems;
	};
}


#endif