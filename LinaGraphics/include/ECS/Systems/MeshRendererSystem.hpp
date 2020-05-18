/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: MeshRendererSystem
Timestamp: 4/27/2019 5:38:44 PM

*/

#pragma once

#ifndef RenderableMeshSystem_HPP
#define RenderableMeshSystem_HPP

#include "ECS/ECSSystem.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/VertexArray.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Material;

		struct BatchDrawData
		{
			Graphics::VertexArray* vertexArray;
			Graphics::Material* material;
		};

		struct BatchModelData
		{
			LinaArray<Matrix> models;
			LinaArray<Matrix> inverseTransposeModels;
		};
	}
}

namespace LinaEngine::ECS
{
	class MeshRendererSystem : public BaseECSSystem
	{

	public:

		typedef std::map<Graphics::BatchDrawData, Graphics::BatchModelData> OpaqueRenderBatch;
		typedef std::map<float, std::tuple<Graphics::BatchDrawData, Graphics::BatchModelData>> TransparentRenderBatch;

		MeshRendererSystem() {};

		FORCEINLINE void Construct(ECSRegistry& registry, Graphics::RenderEngine& renderEngineIn, RenderDevice& renderDeviceIn, Graphics::RenderTarget& renderTargetIn)
		{
			BaseECSSystem::Construct(registry);
			m_RenderEngine = &renderEngineIn;
			m_RenderDevice = &renderDeviceIn;
			m_RenderTarget = &renderTargetIn;
		}

		void RenderOpaque(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn);
		void RenderTransparent(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn, float priority);
		void FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);

		virtual void UpdateComponents(float delta) override;


	private:

		RenderDevice* m_RenderDevice = nullptr;
		Graphics::RenderTarget* m_RenderTarget = nullptr;
		Graphics::RenderEngine* m_RenderEngine = nullptr;

		// Map to see the list of same vertex array & textures to compress them into single draw call.
		OpaqueRenderBatch m_OpaqueRenderBatch;
		TransparentRenderBatch m_TransparentRenderBatch;
	};
}


#endif