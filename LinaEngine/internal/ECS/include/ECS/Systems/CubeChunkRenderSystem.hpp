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

namespace LinaEngine
{
	namespace Graphics
	{
		class GameRenderContext;
		class VertexArray;
		class Texture;
	}
}

namespace LinaEngine::ECS
{
	class CubeChunkRenderSystem : public BaseECSSystem
	{
	public:

		CubeChunkRenderSystem(LinaEngine::Graphics::GameRenderContext& contextIn, LinaEngine::Graphics::VertexArray& vertexArrayIn, LinaEngine::Graphics::Texture** texturesIn, size_t textureCountIn);

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

	private:

		LinaEngine::Graphics::GameRenderContext& context;
		LinaEngine::Graphics::VertexArray& vertexArray;
		LinaEngine::Graphics::Texture** textures;
		size_t textureCount;
	};
}


#endif