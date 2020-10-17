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
Class: VertexArray

Used for creation and storage of VAO data in the GPU backend.

Timestamp: 4/26/2019 12:30:15 AM
*/

#pragma once

#ifndef VertexArray_HPP
#define VertexArray_HPP


#include "Core/Common.hpp"
#include "Core/SizeDefinitions.hpp"
#include "RenderingCommon.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "IndexedModel.hpp"

namespace LinaEngine::Graphics
{

	class VertexArray
	{
	public:

		FORCEINLINE VertexArray() : m_EngineBoundID(0), m_IndexCount(0), m_RenderDevice(nullptr) {};
		FORCEINLINE ~VertexArray()
		{
			m_EngineBoundID = m_RenderDevice->ReleaseVertexArray(m_EngineBoundID);
		}
	
		FORCEINLINE void Construct(RenderDevice& deviceIn, const IndexedModel& model, BufferUsage bufferUsage)
		{
			m_RenderDevice = &deviceIn;
			m_EngineBoundID = model.CreateVertexArray(deviceIn, bufferUsage);
			m_IndexCount = model.GetIndexCount();
		}

		FORCEINLINE void UpdateBuffer(uint32 bufferIndex, const void* data, uintptr dataSize)
		{
			return m_RenderDevice->UpdateVertexArrayBuffer(m_EngineBoundID, bufferIndex, data, dataSize);
		}

		FORCEINLINE uint32 GetID()
		{ 
			return m_EngineBoundID;
		}

		FORCEINLINE uint32 GetIndexCount()
		{ 
			return m_IndexCount;  
		}


	private:

		RenderDevice* m_RenderDevice = nullptr;
		uint32 m_EngineBoundID = 0;
		uint32 m_IndexCount = 0;

		
	};

}


#endif