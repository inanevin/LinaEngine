/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: GameRenderContext
Timestamp: 4/27/2019 5:53:43 PM

*/

#pragma once

#ifndef GameRenderContext_HPP
#define GameRenderContext_HPP

#include "RenderContext.hpp"

namespace LinaEngine::Graphics
{
	class GameRenderContext : public RenderContext
	{
	public:
		GameRenderContext(PAMRenderDevice& deviceIn, RenderTarget& targetIn, DrawParams& drawParamsIn, Shader& shaderIn, Sampler& samplerIn, const Matrix& perspectiveIn)
			: RenderContext(deviceIn, targetIn), drawParams(drawParamsIn), shader(shaderIn), sampler(samplerIn), perspective(perspectiveIn) {};


		FORCEINLINE void RenderMesh(VertexArray& vertexArray, Texture& texture, const Matrix& transformIn)
		{
			// Add the new matrix to the same pairs, each pair will be drawn once.
			meshRenderBuffer[LinaMakePair(&vertexArray, &texture)].push_back(perspective * transformIn);
		}

		void Flush();

	private:

		DrawParams& drawParams;
		Shader& shader;
		Sampler& sampler;
		Matrix perspective;

		// Map to see the list of same vertex array & textures to compress them into single draw call.
		LinaMap<LinaPair<VertexArray*, Texture*>, LinaArray<Matrix> > meshRenderBuffer;

	};
}


#endif