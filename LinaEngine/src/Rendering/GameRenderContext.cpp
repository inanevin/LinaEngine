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
Timestamp: 4/27/2019 5:54:04 PM

*/

#include "LinaPch.hpp"
#include "Rendering/GameRenderContext.hpp"  
#include "Core/STLImport.hpp"



namespace LinaEngine::Graphics
{
	void GameRenderContext::Flush()
	{
		Texture* currentTexture = nullptr;

		for (LinaMap<LinaPair<VertexArray*, Texture*>, LinaArray<Matrix> >::iterator it = m_MeshRenderBuffer.begin(); it != m_MeshRenderBuffer.end(); ++it)
		{
			VertexArray* vertexArray = it->first.first;
			Texture* texture = it->first.second;
			Matrix* transforms = &it->second[0];
			size_t numTransforms = it->second.size();
		
		

			if (numTransforms == 0) continue;
			

			if (texture != currentTexture) 
				m_Shader->SetSampler(m_SamplerName, *texture, *m_Sampler, 0);

			


			for (uint32 i = 0; i < numTransforms; i++)
			{
				// Update transformMat attribute, the buffer w/ each transform.
				//vertexArray->UpdateBuffer(4, &it->second[i], 1 * sizeof(Matrix));

				// Update model attribute.
				//vertexArray->UpdateBuffer(5, &m_ModelMatrices[i], 1 * sizeof(Matrix));

				// Update inversed model attribute.
				//vertexArray->UpdateBuffer(6, &m_NormalMatrices[i], 1 * sizeof(Matrix));

				// Update view matrix in shader.
				
				//renderDevice->UpdateShaderUniformVector3F(m_Shader->GetID(), "_lightPos", Vector3F(10.0f, 8.0f, 10.0f));
				//renderDevice->UpdateShaderUniformVector3F(m_Shader->GetID(), "_lightColor", Vector3F(1.0f, 0.0f, 0.0f));


				vertexArray->UpdateBuffer(4, &it->second[i], 1 * sizeof(Matrix));
				//vertexArray->UpdateBuffer(5, &m_NormalMatrices[i], 1 * sizeof(Matrix));


				// Draw call.
				this->Draw(*m_Shader, *vertexArray, *m_DrawParams, 1);
				renderDevice->UpdateShaderUniformMatrix(m_Shader->GetID(), "model", it->second[i]);
				renderDevice->UpdateShaderUniformMatrix(m_Shader->GetID(), "view", m_ViewMatrix);
				renderDevice->UpdateShaderUniformMatrix(m_Shader->GetID(), "projection", m_Projection);
				renderDevice->UpdateShaderUniformMatrix(m_Shader->GetID(), "inverseTransposeNormal", (m_ViewMatrix * it->second[i]).toNormalMatrix());

			}


	
			// Clear the buffer, or do not if you want a trail of shadows lol.
			it->second.clear();

			
		}
	}
}

