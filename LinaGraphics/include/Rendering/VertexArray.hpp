/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: VertexArray
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
			m_DrawArrays = false;
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

		FORCEINLINE uint32 GetDrawArrays() { return m_DrawArrays; }

	private:

		bool m_DrawArrays = false;
		RenderDevice* m_RenderDevice = nullptr;
		uint32 m_EngineBoundID = 0;
		uint32 m_IndexCount = 0;

		
	};

}


#endif