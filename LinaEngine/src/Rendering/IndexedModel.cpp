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

#include "LinaPch.hpp"
#include "Rendering/IndexedModel.hpp"  

namespace LinaEngine::Graphics
{
	void IndexedModel::addElement1f(uint32 elementIndex, float e0)
	{
	
		LINA_CORE_ASSERT(elementIndex < elementSizes.size());
		elements[elementIndex].push_back(e0);
	}

	void IndexedModel::addElement2f(uint32 elementIndex, float e0, float e1)
	{
		LINA_CORE_ASSERT(elementIndex < elementSizes.size());
		elements[elementIndex].push_back(e0);
		elements[elementIndex].push_back(e1);
	}

	void IndexedModel::addElement3f(uint32 elementIndex, float e0, float e1, float e2)
	{
		LINA_CORE_ASSERT(elementIndex < elementSizes.size());
		elements[elementIndex].push_back(e0);
		elements[elementIndex].push_back(e1);
		elements[elementIndex].push_back(e2);
	}

	void IndexedModel::addElement4f(uint32 elementIndex, float e0, float e1, float e2, float e3)
	{
		LINA_CORE_ASSERT(elementIndex < elementSizes.size());
		elements[elementIndex].push_back(e0);
		elements[elementIndex].push_back(e1);
		elements[elementIndex].push_back(e2);
		elements[elementIndex].push_back(e3);
	}

	void IndexedModel::addIndices1i(uint32 i0)
	{
		indices.push_back(i0);
	}

	void IndexedModel::addIndices2i(uint32 i0, uint32 i1)
	{
		indices.push_back(i0);
		indices.push_back(i1);
	}

	void IndexedModel::addIndices3i(uint32 i0, uint32 i1, uint32 i2)
	{
		indices.push_back(i0);
		indices.push_back(i1);
		indices.push_back(i2);
	}

	void IndexedModel::addIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3)
	{
		indices.push_back(i0);
		indices.push_back(i1);
		indices.push_back(i2);
		indices.push_back(i3);
	}

	uint32 IndexedModel::getNumIndices() const
	{
		return indices.size();
	}

	void IndexedModel::allocateElement(uint32 elementSize)
	{
		elementSizes.push_back(elementSize);
		elements.push_back(LinaArray<float>());
	}

	void IndexedModel::setInstancedElementStartIndex(uint32 elementIndex)
	{
		instancedElementsStartIndex = elementIndex;
	}

	uint32 IndexedModel::createVertexArray(RenderEngine<PAMRenderEngine>& device, int bufferUsage) const
	{
		uint32 numVertexComponents = elementSizes.size();
		uint32 numInstanceComponents = instancedElementsStartIndex == ((uint32)-1) ?
			0 : (numVertexComponents - instancedElementsStartIndex);
		numVertexComponents -= numInstanceComponents;

		LinaArray<const float*> vertexDataArray;
		for (uint32 i = 0; i < numVertexComponents; i++) {
			vertexDataArray.push_back(&(elements[i][0]));
		}

		const float** vertexData = &vertexDataArray[0];
		const uint32* vertexElementSizes = &elementSizes[0];

		uint32 numVertices = elements[0].size() / vertexElementSizes[0];
		uint32 numIndices = indices.size();

		return device.CreateVertexArray(vertexData, vertexElementSizes, numVertexComponents, numInstanceComponents, numVertices, &indices[0], numIndices, bufferUsage);
	}
}

