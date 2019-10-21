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
	void GameRenderContext::RenderMesh(VertexArray& vertexArray,  MeshMaterial& material, const Matrix& transformIn)
	{
		// Add the new matrix to the same pairs, each pair will be drawn once.
		auto pair = LinaMakePair(&vertexArray, &material);
		LinaGet<0>(m_MeshRenderBuffer[pair]).push_back(transformIn.transpose());
		LinaGet<1>(m_MeshRenderBuffer[pair]).push_back(transformIn.inverse());
	}

	void GameRenderContext::Flush()
	{
		Texture* currentTexture = nullptr;

		for (LinaMap<LinaPair<VertexArray*, MeshMaterial*>, LinaTuple<LinaArray<Matrix>, LinaArray<Matrix>, LinaArray<Matrix>>>::iterator it = m_MeshRenderBuffer.begin(); it != m_MeshRenderBuffer.end(); ++it)
		{
			auto modelArray = LinaGet<0>(it->second);
			auto inverseModelArray = LinaGet<1>(it->second);

			VertexArray* vertexArray = it->first.first;
			Texture* texture = it->first.second->texture;
			Matrix* models = &modelArray[0];
			Matrix* inverseTransposeModels = &inverseModelArray[0];
			size_t numTransforms = modelArray.size();

			if (numTransforms == 0) continue;

			if (texture != currentTexture)
				m_Shader->SetSampler(m_SamplerName, *texture, *m_Sampler, 0);

			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));

			UpdateShaderData(it->first.second);

			// Draw call.
			//this->Draw(*m_Shader, *vertexArray, *m_DrawParams, numTransforms);
			this->Draw(it->first.second->shaderID, *vertexArray, *m_DrawParams, numTransforms);


			// Clear the buffer, or do not if you want a trail of shadows lol.
			modelArray.clear();
			inverseModelArray.clear();

		}
	}
}

