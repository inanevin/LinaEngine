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
#include "RenderTarget.hpp"
#include "VertexArray.hpp"

namespace LinaEngine
{
	namespace ECS
	{
		class LighingSystem;
	}
}

namespace LinaEngine::Graphics
{
	class Material;

	class RenderContext
	{
	public:

		FORCEINLINE void Construct(RenderDevice& renderDeviceIn, RenderTarget& renderTargetIn, DrawParams& drawParamsIn, Texture& text, LinaEngine::ECS::LightingSystem& lightingSystem)
		{
			m_RenderDevice = &renderDeviceIn;
			m_Target = &renderTargetIn;
			m_DrawParams = &drawParamsIn;
			m_DefaultTexture = &text;
			m_LightingSystem = &lightingSystem;
		}

		FORCEINLINE void Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const Color& color, uint32 stencil)
		{
			m_RenderDevice->Clear(m_Target->GetID(), shouldClearColor, shouldClearDepth, shouldClearStencil, color, stencil);
		}

		FORCEINLINE void Clear(const Color& color, bool shouldClearDepth = false)
		{
			m_RenderDevice->Clear(m_Target->GetID(), true, shouldClearDepth, false, color, 0);
		}

		FORCEINLINE void Draw(uint32 vao, const DrawParams& drawParams, uint32 numInstances = 1, uint32 numElements = 1, bool drawArrays = false)
		{
			m_RenderDevice->Draw(m_Target->GetID(), vao, drawParams, numInstances, numElements, drawArrays);
		}

		FORCEINLINE void Draw(VertexArray& vertexArray, const DrawParams& drawParams, uint32 numInstances = 1, bool drawArrays = false)
		{
			m_RenderDevice->Draw(m_Target->GetID(), vertexArray.GetID(), drawParams, numInstances, vertexArray.GetIndexCount(), drawArrays);
		}

		void UpdateShaderData(Material* data);
		void RenderMesh(VertexArray& vertexArray, Material& material, const Matrix& transformIn);
		void Flush();

	private:

		RenderDevice* m_RenderDevice = nullptr;
		RenderTarget* m_Target = nullptr;
		DrawParams* m_DrawParams = nullptr;
		Texture* m_DefaultTexture = nullptr;
		LinaEngine::ECS::LightingSystem* m_LightingSystem = nullptr;

		// Map to see the list of same vertex array & textures to compress them into single draw call.
		std::map<std::pair<VertexArray*, Material*>, std::tuple<LinaArray<Matrix>, LinaArray<Matrix>, LinaArray<Matrix>>> m_MeshRenderBuffer;
	
	private:

	};
}


#endif