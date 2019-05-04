/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: RenderContext
Timestamp: 4/27/2019 5:48:39 PM

*/

#pragma once

#ifndef RenderContext_HPP
#define RenderContext_HPP

#include "PackageManager/PAMRenderDevice.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"
#include "VertexArray.hpp"

namespace LinaEngine::Graphics
{
	class RenderContext
	{
	public:

		virtual ~RenderContext() {};

		FORCEINLINE void Construct(PAMRenderDevice& renderDeviceIn, RenderTarget& renderTargetIn)
		{
			renderDevice = &renderDeviceIn;
			target = &renderTargetIn;
		}

		FORCEINLINE void Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const Color& color, uint32 stencil)
		{
			renderDevice->Clear(target->GetID(), shouldClearColor, shouldClearDepth, shouldClearStencil, color, stencil);
		}

		FORCEINLINE void Clear(const Color& color, bool shouldClearDepth = false)
		{
			renderDevice->Clear(target->GetID(), true, shouldClearDepth, false, color, 0);
		}

		FORCEINLINE void Draw(Shader& shader, VertexArray& vertexArray, const DrawParams& drawParams, uint32 numInstances = 1)
		{
			renderDevice->Draw(target->GetID(), shader.GetID(), vertexArray.GetID(), drawParams, numInstances, vertexArray.GetIndexCount());
		}

		FORCEINLINE void Draw(Shader& shader, VertexArray& vertexArray, const DrawParams& drawParams, uint32 numInstances, uint32 numIndices)
		{
			renderDevice->Draw(target->GetID(), shader.GetID(), vertexArray.GetID(), drawParams, numInstances, numIndices);
		}

	protected:

		FORCEINLINE RenderContext() {}
	
	private:

		PAMRenderDevice* renderDevice;
		RenderTarget* target;

	};
}


#endif