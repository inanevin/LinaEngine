/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: FreeLookSystem
Timestamp: 5/2/2019 2:21:35 AM

*/

#pragma once

#ifndef FreeLookSystem_HPP
#define FreeLookSystem_HPP

#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "PackageManager/PAMInputEngine.hpp"


namespace LinaEngine::ECS
{
	class FreeLookSystem : public BaseECSSystem
	{
	public:

		FreeLookSystem(InputEngine<PAMInputEngine>& inputEngineIn) : BaseECSSystem(), inputEngine(inputEngineIn)
		{
			AddComponentType(TransformComponent::ID);
			AddComponentType(FreeLookComponent::ID);
		}

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

		FORCEINLINE void SetWindowCenter(const Vector2F& windowCenter) { m_WindowCenter = windowCenter;  }

	private:

		InputEngine<PAMInputEngine>& inputEngine;
		Vector2F m_WindowCenter;
		float horizontalKeyAmount;
		float verticalKeyAmount;
	};
}


#endif