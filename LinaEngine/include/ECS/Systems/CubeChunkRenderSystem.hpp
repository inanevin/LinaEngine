/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: CubeChunkRenderSystem
Timestamp: 4/30/2019 12:16:36 AM

*/

#pragma once

#ifndef CubeChunkRenderSystem_HPP
#define CubeChunkRenderSystem_HPP

#include "ECS/ECSSystem.hpp"
#include "Rendering/GameRenderContext.hpp"
#include "ECS/Components/CubeChunkComponent.hpp"

using namespace LinaEngine::Graphics;


namespace LinaEngine::ECS
{
	class CubeChunkRenderSystem : BaseECSSystem
	{
	public:

		CubeChunkRenderSystem(GameRenderContext& contextIn, VertexArray& vertexArrayIn, Texture** texturesIn, size_t textureCountIn) : BaseECSSystem(), context(contextIn), vertexArray(vertexArrayIn), textures(texturesIn), textureCount(textureCountIn)
		{
			AddComponentType(CubeChunkComponent::ID);
		}

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

	private:

		GameRenderContext& context;
		VertexArray& vertexArray;
		Texture** textures;
		size_t textureCount;
	};
}


#endif