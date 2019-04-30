/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: MotionComponent
Timestamp: 4/28/2019 3:37:53 AM

*/

#pragma once

#ifndef MotionComponent_HPP
#define MotionComponent_HPP


namespace LinaEngine::ECS
{
	struct MotionComponent : public ECSComponent<MotionComponent>
	{

		Vector3F velocity = Vector3F(0.0f, 0.0f, 0.0f);
		Vector3F acceleration = Vector3F(0.0f, 0.0f, 0.0f);
	};
}


#endif