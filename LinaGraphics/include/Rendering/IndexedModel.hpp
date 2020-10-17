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

/*
Class: IndexedModel

Holds vertex, index, joint etc. data for model files.

Timestamp: 4/26/2019 12:11:36 AM
*/

#pragma once

#ifndef IndexedModel_HPP
#define IndexedModel_HPP

#include "Core/SizeDefinitions.hpp"
#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine::Graphics
{
	class IndexedModel
	{
	public:

		IndexedModel() : m_startIndex((uint32)-1) {}

		// Creates a vertex array using render renderEngine.
		uint32 CreateVertexArray(RenderDevice& engine, BufferUsage bufferUsage) const;

		// Sets the element size array according to the desired size.
		void AllocateElement(uint32 elementSize, bool isFloat);

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

		// Gets the element array
		std::vector<std::vector<float>>& GetElements() { return m_elements; }

		// Sets the start index for instanced elements.
		void SetStartIndex(uint32 elementIndex) { m_startIndex = elementIndex; }

		// Accessor for num m_Indices.
		uint32 GetIndexCount() const { return m_indices.size(); }

	private:

		// Index & element data.
		std::vector<uint32> m_indices;
		std::vector<uint32> m_elementSizes;
		std::vector<uint32> m_elementTypes;
		std::vector<std::vector<float>> m_elements;

		// Start index for instanced elements.
		uint32 m_startIndex = 0;

	};
}


#endif