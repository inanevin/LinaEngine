/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: SkyboxContext
Timestamp: 5/8/2019 6:51:57 AM

*/

#pragma once

#ifndef SkyboxContext_HPP
#define SkyboxContext_HPP

#include "Rendering/RenderContext.hpp"

namespace LinaEngine::Graphics
{
	class SkyboxContext : public RenderContext
	{
	public:

		SkyboxContext() {};
		virtual ~SkyboxContext() { renderDevice->ReleaseVertexArray(m_VAO); };

		FORCEINLINE void Construct(PAMRenderDevice& deviceIn, RenderTarget& targetIn, DrawParams& drawParamsIn, Shader& shaderIn, Sampler& samplerIn, Texture& texture)
		{
			RenderContext::Construct(deviceIn, targetIn);
			m_DrawParams = &drawParamsIn;
			m_Shader = &shaderIn;
			m_VAO = deviceIn.CreateSkyboxVertexArray();
			m_Texture = &texture;
			m_Sampler = &samplerIn;
		}



		void RenderSkybox(const Matrix& projection, const Matrix& view);

	private:

		LinaString m_SamplerName;

		DrawParams* m_DrawParams;
		Shader* m_Shader;
		uint32 m_VAO;
		Texture* m_Texture;
		Sampler* m_Sampler;
	};
}


#endif