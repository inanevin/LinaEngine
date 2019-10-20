/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: UniformBuffer
Timestamp: 4/26/2019 6:41:58 PM

*/

#pragma once

#ifndef UniformBuffer_HPP
#define UniformBuffer_HPP

#include "PackageManager/PAMRenderDevice.hpp"


namespace LinaEngine::Graphics
{
	class UniformBuffer
	{
	public:

		// Param const creates buffer through render engine.
		FORCEINLINE UniformBuffer() {}

		// Destructor releases the buffer through render engine.
		FORCEINLINE ~UniformBuffer()
		{
			if (m_IsConstructed)
				m_EngineBoundID = m_RenderDevice->ReleaseUniformBuffer(m_EngineBoundID);
		}

		// Updates the uniform buffer w/ new data, allows dynamic size change.
		FORCEINLINE void Construct(PAMRenderDevice& renderDeviceIn, uintptr dataSize, BufferUsage usage, const void* data = nullptr)
		{
			m_RenderDevice = &renderDeviceIn;
			m_BufferSize = dataSize;
			m_EngineBoundID = m_RenderDevice->CreateUniformBuffer(data, dataSize, usage);
			m_IsConstructed = true;
		}

		FORCEINLINE void Bind(uint32 point)
		{
			m_RenderDevice->BindUniformBuffer(m_EngineBoundID, point);
		}

		FORCEINLINE void Update(const void* data, uintptr offset, uintptr dataSize) { m_RenderDevice->UpdateUniformBuffer(m_EngineBoundID, data, offset, dataSize); }
		
		FORCEINLINE uint32 GetID() { return m_EngineBoundID; }

	private:

		PAMRenderDevice* m_RenderDevice;
		uint32 m_EngineBoundID;
		uintptr m_BufferSize;
		bool m_IsConstructed = false;

		NULL_COPY_AND_ASSIGN(UniformBuffer);

	};
}


#endif