/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

	uint32 IndexedModel::CreateVertexArray(RenderDevice& renderDevice, BufferUsage bufferUsage) const
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

		return renderDevice.CreateVertexArray(vertexData, vertexElementSizes, vertexElementTypes, numVertexComponents, numInstanceComponents, numVertices, &m_indices[0], numIndices, bufferUsage);
	}
}

