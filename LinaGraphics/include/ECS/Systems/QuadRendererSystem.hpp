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

Class: QuadRendererSystem
Timestamp: 5/17/2020 2:32:59 AM

*/
#pragma once

#ifndef QuadRendererSystem_HPP
#define QuadRendererSystem_HPP

// Headers here.
#include "ECS/ECSSystem.hpp"
#include "PackageManager/PAMRenderDevice.hpp"

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
	class QuadRendererSystem : public BaseECSSystem
	{
		
	public:

		QuadRendererSystem() {};
		

		FORCEINLINE void Construct(ECSRegistry& registry, RenderDevice& renderDeviceIn, Graphics::RenderEngine& renderEngineIn, uint32 quadVAO, uint32 fbo)
		{
			BaseECSSystem::Construct(registry);
			m_RenderDevice = &renderDeviceIn;
			m_RenderEngine = &renderEngineIn;
			m_QuadVAO = quadVAO;
			m_FBO = fbo;
		}

		virtual void UpdateComponents(float delta) override;
		void Flush(Graphics::DrawParams& drawParams);

	private:

		Graphics::RenderEngine* m_RenderEngine;
		RenderDevice* m_RenderDevice;
		uint32 m_QuadVAO;
		uint32 m_FBO;
		std::map<float, std::tuple<Matrix, Graphics::Material*>> m_QuadsToRender;
	};
}

#endif
