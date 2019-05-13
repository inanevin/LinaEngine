/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: SpriteRendererSystem
Timestamp: 5/9/2019 3:02:50 AM

*/

#pragma once

#ifndef SpriteRendererSystem_HPP
#define SpriteRendererSystem_HPP

#include "ECS/ECSSystem.hpp"
#include "Rendering/Sampler.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"


using namespace LinaEngine::Graphics;

namespace LinaEngine::ECS
{
	class SpriteRendererSystem : public BaseECSSystem
	{

	public:

		SpriteRendererSystem() : BaseECSSystem()
		{
			AddComponentType(TransformComponent::ID);
			AddComponentType(SpriteRendererComponent::ID);
		}

		FORCEINLINE void Construct(PAMRenderDevice& renderDeviceIn, Sampler& samplerIn, uint32 fbo, uint32 vao, DrawParams& drawParams, const Matrix& projection)
		{
			m_FBO = fbo;
			m_VAO = vao;
			m_DrawParams = &drawParams;
			m_RenderDevice = &renderDeviceIn;
			m_SpriteSampler = &samplerIn;
			m_ViewProjection = projection;
		}

		FORCEINLINE void SetProjectionMatrix(const Matrix& projection)
		{
			m_ViewProjection = projection;
		}

		virtual void UpdateComponents(float delta, BaseECSComponent** components);

	private:

		uint32 m_VAO;
		uint32 m_FBO;
		DrawParams* m_DrawParams;
		PAMRenderDevice* m_RenderDevice;
		Sampler* m_SpriteSampler;
		Matrix m_ViewProjection;
	};
}


#endif