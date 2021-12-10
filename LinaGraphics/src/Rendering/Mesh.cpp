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

#include "Rendering/Mesh.hpp"  
#include "PackageManager/PAMRenderDevice.hpp"

namespace Lina::Graphics
{
	void Mesh::AddElement(uint32 elementIndex, float e0)
	{
		if(m_bufferElements[elementIndex].m_isFloat)
			m_bufferElements[elementIndex].m_floatElements.push_back(e0);
		else
			m_bufferElements[elementIndex].m_intElements.push_back(e0);
	}

	void Mesh::AddElement(uint32 elementIndex, int e0)
	{

		if (m_bufferElements[elementIndex].m_isFloat)
			m_bufferElements[elementIndex].m_floatElements.push_back(e0);
		else
			m_bufferElements[elementIndex].m_intElements.push_back(e0);
	}


	void Mesh::AddIndices(uint32 i0, uint32 i1, uint32 i2)
	{
		m_indices.push_back(i0);
		m_indices.push_back(i1);
		m_indices.push_back(i2);
	}

	void Mesh::AllocateElement(uint32 elementSize, uint32 attrib, bool isFloat, bool isInstanced)
	{
		m_bufferElements.push_back(BufferData(elementSize, attrib, isFloat, isInstanced));
	}

	void Mesh::CreateVertexArray(RenderDevice& renderDevice, BufferUsage bufferUsage)
	{
		uint32 numVertices = m_bufferElements[0].m_floatElements.size() / m_bufferElements[0].m_elementSize;
		uint32 numIndices = m_indices.size();
		
		int totalVertexComponents = 0;
		for (int i = 0; i < m_bufferElements.size(); i++)
		{
			if (!m_bufferElements[i].m_isInstanced)
				totalVertexComponents++;
		}

		int totalInstanceComponents = m_bufferElements.size() - totalVertexComponents;

		// Init vertex array.
		uint32 id = renderDevice.CreateVertexArray(m_bufferElements, totalVertexComponents, totalInstanceComponents, numVertices, &m_indices[0], numIndices, bufferUsage);
		m_vertexArray.Setup(renderDevice, id, GetIndexCount());
	}
}

