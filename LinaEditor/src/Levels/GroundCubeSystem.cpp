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

Class: GroundCubeSystem
Timestamp: 6/4/2020 1:44:04 AM

*/

#include "Levels/GroundCubeSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "Levels/GroundCubeComponent.hpp"
#include "Utility/Math/Math.hpp"

namespace LinaEngine::ECS
{
	static float totalTime;
	static float time = 5;
	void GroundCubeSystem::UpdateComponents(float delta)
	{
		auto& view = m_Registry->view<TransformComponent, GroundCubeComponent>();

		totalTime += delta;
	

		for (auto& entity : view)
		{
			TransformComponent& tc = view.get<TransformComponent>(entity);
			GroundCubeComponent& gc = view.get<GroundCubeComponent>(entity);

			if (totalTime > gc.delay)
			{
				gc.time += delta;
				if (gc.time > time)
					gc.time = time;

				float currentT = Math::Remap(gc.time, 0.0f, time, 0.0f, 1.0f);
				tc.transform.m_location = Vector3::Lerp(gc.initialPos, gc.targetPos, currentT);
			}

			
		}
	}
}