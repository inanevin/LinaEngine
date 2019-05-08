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
Timestamp: 5/8/2019 6:51:32 AM

*/

#include "LinaPch.hpp"
#include "Rendering/SkyboxContext.hpp"  
#include "..\..\include\Rendering\SkyboxContext.hpp"

namespace LinaEngine::Graphics
{
	void SkyboxContext::RenderSkybox(const Matrix& projection, const Matrix& view)
	{

		m_Shader->SetSampler(m_SamplerName, *m_Texture, *m_Sampler, 0, BindTextureMode::BINDTEXTURE_CUBEMAP);
		renderDevice->DrawSkybox(target->GetID(), m_Shader->GetID(), m_VAO, m_Texture->GetID(), *m_DrawParams, projection, view);
	

	}
}

