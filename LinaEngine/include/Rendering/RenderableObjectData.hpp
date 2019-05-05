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
Timestamp: 5/4/2019 11:41:37 PM

*/

#pragma once

#ifndef RenderableObjectData_HPP
#define RenderableObjectData_HPP


#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/IndexedModel.hpp"
#include "Rendering/Material.hpp"

namespace LinaEngine::Graphics
{
	class RenderableObjectData
	{
	public:

		RenderableObjectData() {};
		~RenderableObjectData() 
		{
			for (uint32 i = 0; i < m_VertexArrays.size(); i++)
				delete m_VertexArrays[i];

			m_VertexArrays.clear();
			m_IndexedModelArray.clear();
			m_MaterialSpecArray.clear();
			m_MaterialIndexArray.clear();
		};


		FORCEINLINE LinaArray<VertexArray*>& GetVertexArrays()
		{
			return m_VertexArrays;
		}

		FORCEINLINE LinaArray<IndexedModel>& GetIndexedModels()
		{
			return m_IndexedModelArray;
		}

		FORCEINLINE LinaArray<MaterialSpec>& GetMaterialSpecs()
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
		LinaArray<MaterialSpec> m_MaterialSpecArray;
		LinaArray<uint32> m_MaterialIndexArray;


	};
}


#endif