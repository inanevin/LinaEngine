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
Timestamp: 4/30/2019 12:18:00 AM

*/

#include "ECS/Systems/CubeChunkRenderSystem.hpp"
#include "ECS/Components/CubeChunkComponent.hpp"
#include "Utility/Math/Transformation.hpp"

namespace LinaEngine::ECS
{
	CubeChunkRenderSystem::CubeChunkRenderSystem(LinaEngine::Graphics::GameRenderContext& contextIn, LinaEngine::Graphics::VertexArray& vertexArrayIn, LinaEngine::Graphics::Texture** texturesIn, size_t textureCountIn) : BaseECSSystem(), context(contextIn), vertexArray(vertexArrayIn), textures(texturesIn), textureCount(textureCountIn)
	{

	}
	void CubeChunkRenderSystem::UpdateComponents(float delta, BaseECSComponent** components)
	{
		CubeChunkComponent* component = (CubeChunkComponent*)components[0];

		Transformation transform(Vector3F(component->position[0], component->position[1], component->position[2]));
		//context.RenderMesh(vertexArray, *(textures[component->textureIndex]), transform.ToMatrix());
	}
}

