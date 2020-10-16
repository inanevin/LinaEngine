/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SpriteRendererComponent
Timestamp: 10/1/2020 9:26:21 AM

*/
#pragma once

#ifndef SpriteRendererComponent_HPP
#define SpriteRendererComponent_HPP


#include "ECS/ECSComponent.hpp"


namespace LinaEngine::ECS
{
	struct SpriteRendererComponent : public ECSComponent
	{
		int materialID = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(materialID, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<SpriteRendererComponent>(entity, SpriteRendererComponent()); }
#endif
	};
}

#endif
