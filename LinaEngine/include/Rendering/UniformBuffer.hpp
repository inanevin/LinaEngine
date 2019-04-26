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

#include "PackageManager/PAMRenderEngine.hpp"


namespace LinaEngine::Graphics
{
	class UniformBuffer
	{
	public:

		// Param const creates buffer through render engine.
		FORCEINLINE UniformBuffer(RenderEngine<PAMRenderEngine>& renderEngineIn, uintptr dataSize, BufferUsage usage, const void* data = nullptr) :
			renderEngine(&renderEngineIn), m_EngineBoundID(renderEngine->CreateUniformBuffer(data, dataSize, usage)), m_BufferSize(dataSize) {}
		
		// Destructor releases the buffer through render engine.
		FORCEINLINE ~UniformBuffer()
		{
			m_EngineBoundID = renderEngine->ReleaseUniformBuffer(m_EngineBoundID);
		}

		// Updates the uniform buffer w/ new data, allows dynamic size change.
		FORCEINLINE void Update(const void* data, uintptr dataSize) {renderEngine->UpdateUniformBuffer(m_EngineBoundID, data, dataSize); }
		FORCEINLINE void Update(const void* data) { Update(data, m_BufferSize); }
		FORCEINLINE uint32 GetID() { return m_EngineBoundID; }

	private:

		RenderEngine<PAMRenderEngine>* renderEngine;
		uint32 m_EngineBoundID;
		uintptr m_BufferSize;

		NULL_COPY_AND_ASSIGN(UniformBuffer);

	};
}


#endif