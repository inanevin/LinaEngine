/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SpriteRendererComponent
Timestamp: 5/9/2019 2:58:13 AM

*/

#pragma once

#ifndef SpriteRendererComponent_HPP
#define SpriteRendererComponent_HPP

#include "ECS/ECSComponent.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Shader.hpp"

using namespace LinaEngine::Graphics;


namespace LinaEngine::ECS
{
	struct SpriteRendererComponent : public ECSComponent<SpriteRendererComponent>
	{
		Texture* texture = nullptr;
		Shader* shader = nullptr;
		Color color;
	};
}


#endif