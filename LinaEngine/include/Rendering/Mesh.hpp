/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderableObjectData
Timestamp: 5/6/2019 4:23:45 PM

*/

#pragma once

#ifndef MESH_HPP
#define MESH_HPP


#include "Rendering/Texture.hpp"
#include "Rendering/IndexedModel.hpp"
#include "Rendering/Material.hpp"

namespace LinaEngine::Graphics
{
	class VertexArray;

	class Mesh
	{

	public:

		Mesh() {};

		FORCEINLINE ~Mesh()
		{
			for (uint32 i = 0; i < m_VertexArrays.size(); i++)
				delete m_VertexArrays[i];

			m_VertexArrays.clear();
			m_IndexedModelArray.clear();
			m_MaterialSpecArray.clear();
			m_MaterialIndexArray.clear();
		};

		FORCEINLINE VertexArray* GetVertexArray(uint32 index)
		{

			if (index >= m_VertexArrays.size() || index < 0 || m_VertexArrays.size() == 0)
			{
				LINA_CORE_ERR("Index is bigger than the vertex array size or vertex array size is 0. Returning nullptr!");
				return nullptr;
			}

			return m_VertexArrays.at(index);
		}

		FORCEINLINE LinaArray<VertexArray*>& GetVertexArrays()
		{
			return m_VertexArrays;
		}

		FORCEINLINE LinaArray<IndexedModel>& GetIndexedModels()
		{
			return m_IndexedModelArray;
		}

		FORCEINLINE LinaArray<ModelMaterial>& GetMaterialSpecs()
		{
			return m_MaterialSpecArray;
		}

		FORCEINLINE LinaArray<uint32>& GetMaterialIndices()
		{
			return m_MaterialIndexArray;
		}

	
	private:

		LinaArray<VertexArray*> m_VertexArrays;
		LinaArray<IndexedModel> m_IndexedModelArray;
		LinaArray<ModelMaterial> m_MaterialSpecArray;
		LinaArray<uint32> m_MaterialIndexArray;

	};
}

#endif