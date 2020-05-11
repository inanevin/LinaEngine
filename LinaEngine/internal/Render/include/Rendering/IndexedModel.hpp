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
Timestamp: 4/26/2019 12:11:36 AM

*/

#pragma once

#ifndef IndexedModel_HPP
#define IndexedModel_HPP

#include "Utility/Math/AABB.hpp"
#include "RenderingCommon.hpp"
#include "Core/LinaArray.hpp"
#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine::Graphics
{
	class IndexedModel
	{
	public:

		IndexedModel() : m_StartIndex((uint32)-1) {}

		// Creates a vertex array using render renderEngine.
		uint32 CreateVertexArray(RenderDevice& engine, BufferUsage bufferUsage) const;

		// Sets the element size array according to the desired size.
		void AllocateElement(uint32 elementSize);

		// Adds float data to the m_Elements array, 1 to 4 elems. TODO: Maybe template? Consider inline array push performance.
		void AddElement(uint32 elementIndex, float e0);
		void AddElement(uint32 elementIndex, float e0, float e1);
		void AddElement(uint32 elementIndex, float e0, float e1, float e2);
		void AddElement(uint32 elementIndex, float e0, float e1, float e2, float e3);

		// Adds index data to m_Indices array, 1 to 4 m_Indices. TODO: Maybe template? Consider inline array push performance.
		void AddIndices(uint32 i0);
		void AddIndices(uint32 i0, uint32 i1);
		void AddIndices(uint32 i0, uint32 i1, uint32 i2);
		void AddIndices(uint32 i0, uint32 i1, uint32 i2, uint32 i3);

		// Find the axis aligned bounding box for the current specific element array.
		FORCEINLINE AABB GetAABBForElementArray(uint32 index) 
		{
			return AABB(&m_Elements[index][0], m_Elements[index].size() / m_ElementSizes[index]);
		}

		// Sets the start index for instanced elements.
		FORCEINLINE void SetStartIndex(uint32 elementIndex) { m_StartIndex = elementIndex; }

		// Accessor for num m_Indices.
		FORCEINLINE uint32 GetIndexCount() const { return m_Indices.size(); }

	private:

		// Index & element data.
		LinaArray<uint32> m_Indices;
		LinaArray<uint32> m_ElementSizes;
		LinaArray<LinaArray<float>> m_Elements;

		// Start index for instanced elements.
		uint32 m_StartIndex;

	};
}


#endif