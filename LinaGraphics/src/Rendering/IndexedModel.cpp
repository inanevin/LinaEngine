/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: IndexedModel
Timestamp: 4/26/2019 12:12:01 AM

*/

#include <windows.h>
#include "Rendering/IndexedModel.hpp"  
#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine::Graphics
{
	void IndexedModel::AddElement(uint32 elementIndex, float e0)
	{
	
		LINA_CORE_ASSERT(elementIndex < m_elementSizes.size());
		m_elements[elementIndex].push_back(e0);
	}

	void IndexedModel::AddElement(uint32 elementIndex, float e0, float e1)
	{
		LINA_CORE_ASSERT(elementIndex < m_elementSizes.size());
		m_elements[elementIndex].push_back(e0);
		m_elements[elementIndex].push_back(e1);
	}

	void IndexedModel::AddElement(uint32 elementIndex, float e0, float e1, float e2)
	{
		LINA_CORE_ASSERT(elementIndex < m_elementSizes.size());
		m_elements[elementIndex].push_back(e0);
		m_elements[elementIndex].push_back(e1);
		m_elements[elementIndex].push_back(e2);
	}

	void IndexedModel::AddElement(uint32 elementIndex, float e0, float e1, float e2, float e3)
	{
		LINA_CORE_ASSERT(elementIndex < m_elementSizes.size());
		m_elements[elementIndex].push_back(e0);
		m_elements[elementIndex].push_back(e1);
		m_elements[elementIndex].push_back(e2);
		m_elements[elementIndex].push_back(e3);
	}

	void IndexedModel::AddIndices(uint32 i0)
	{
		m_indices.push_back(i0);
	}

	void IndexedModel::AddIndices(uint32 i0, uint32 i1)
	{
		m_indices.push_back(i0);
		m_indices.push_back(i1);
	}

	void IndexedModel::AddIndices(uint32 i0, uint32 i1, uint32 i2)
	{
		m_indices.push_back(i0);
		m_indices.push_back(i1);
		m_indices.push_back(i2);
	}

	void IndexedModel::AddIndices(uint32 i0, uint32 i1, uint32 i2, uint32 i3)
	{
		m_indices.push_back(i0);
		m_indices.push_back(i1);
		m_indices.push_back(i2);
		m_indices.push_back(i3);
	}

	void IndexedModel::AllocateElement(uint32 elementSize, bool isFloat)
	{
		m_elementSizes.push_back(elementSize);
		m_elementTypes.push_back(isFloat ? 1 : 0);
		m_elements.push_back(std::vector<float>());
	}

	uint32 IndexedModel::CreateVertexArray(RenderDevice& engine, BufferUsage bufferUsage) const
	{
		// Find the vertex component size using start index of instanced components.
		uint32 numVertexComponents = m_elementSizes.size();
		uint32 numInstanceComponents = m_startIndex == ((uint32)-1) ? 0 : (numVertexComponents - m_startIndex);
		numVertexComponents -= numInstanceComponents;

		// Create a new array to add the instanced data.
		std::vector<const float*> vertexDataArray;

		for (uint32 i = 0; i < numVertexComponents; i++) 
			vertexDataArray.push_back(&(m_elements[i][0]));

		
		const float** vertexData = &vertexDataArray[0];
		const uint32* vertexElementSizes = &m_elementSizes[0];
		const uint32* vertexElementTypes = &m_elementTypes[0];
		// Find vertex & index counts to send into render renderEngine.
		uint32 numVertices = m_elements[0].size() / vertexElementSizes[0];
		uint32 numIndices = m_indices.size();

		return engine.CreateVertexArray(vertexData, vertexElementSizes, vertexElementTypes, numVertexComponents, numInstanceComponents, numVertices, &m_indices[0], numIndices, bufferUsage);
	}
}

