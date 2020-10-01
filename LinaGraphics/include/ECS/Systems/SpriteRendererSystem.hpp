/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SpriteRendererSystem
Timestamp: 10/1/2020 9:27:40 AM

*/
#pragma once

#ifndef SpriteRendererSystem_HPP
#define SpriteRendererSystem_HPP

// Headers here.
#include "ECS/ECSSystem.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/IndexedModel.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class RenderEngine;
		class Material;
	}

}

namespace LinaEngine::ECS
{
	class SpriteRendererSystem : public BaseECSSystem
	{

		struct BatchModelData
		{
			LinaArray<Matrix> models;
			LinaArray<Matrix> inverseTransposeModels;
		};

	public:
		
		SpriteRendererSystem() {};
		~SpriteRendererSystem() {};
	
		void Construct(ECSRegistry& registry, Graphics::RenderEngine& renderEngineIn, RenderDevice& renderDeviceIn);
		virtual void UpdateComponents(float delta) override;

		void Render(Graphics::Material& material, const Matrix& transformIn);
		void Flush(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);

	private:
	
		Graphics::IndexedModel m_quadModel;
		Graphics::VertexArray m_spriteVertexArray;
		RenderDevice* m_RenderDevice = nullptr;
		Graphics::RenderEngine* m_RenderEngine = nullptr;
		std::map<Graphics::Material*, BatchModelData> m_renderBatch;
	};
}

#endif
