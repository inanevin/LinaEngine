/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ECSMovementControlComponent
Timestamp: 4/9/2019 3:25:33 PM

*/

#pragma once

#ifndef ECSMovementControlComponent_HPP
#define ECSMovementControlComponent_HPP

#include "ECS/ECSComponent.hpp"
#include "Core/LinaArray.hpp"
#include "Utility/Math/Vector.hpp"

namespace LinaEngine
{
	namespace Input
	{
		class InputKeyAxisBinder;
	}
}

namespace LinaEngine::ECS
{
	struct MovementControl
	{
		MovementControl(const Vector3F movementIn, LinaEngine::Input::InputKeyAxisBinder* inputKeyAxisBinderIn) : movement(movementIn), inputKeyAxisBinder(inputKeyAxisBinderIn), lastInputAmount(0.0f) {};

		Vector3F movement = Vector3F::Zero;
		LinaEngine::Input::InputKeyAxisBinder* inputKeyAxisBinder;
		float lastInputAmount = 0.0f;
	};

	struct MovementControlComponent : public ECSComponent<MovementControlComponent>
	{
		LinaEngine::LinaArray<MovementControl> movementControls;
	};
}


#endif