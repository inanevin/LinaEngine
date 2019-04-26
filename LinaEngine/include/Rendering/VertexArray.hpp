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

#include "Rendering/IndexedModel.hpp"

namespace LinaEngine::Graphics
{
	class VertexArray
	{
	public:

		FORCEINLINE VertexArray(RenderEngine<PAMRenderEngine>& engine, const IndexedModel& model, BufferUsage bufferUsage) : 
			renderEngine(&engine),
			engineBoundID(model.createVertexArray(engine, bufferUsage)), numIndices(model.getNumIndices()) {}

		FORCEINLINE ~VertexArray()
		{
			engineBoundID = renderEngine->ReleaseVertexArray(engineBoundID);
		}

		FORCEINLINE void updateBuffer(uint32 bufferIndex, const void* data, uintptr dataSize)
		{
			return renderEngine->UpdateVertexArrayBuffer(engineBoundID, bufferIndex, data, dataSize);
		}

		FORCEINLINE uint32 getId() { return engineBoundID; }
		inline uint32 getNumIndices() { return numIndices;  }

	private:

		RenderEngine<PAMRenderEngine>* renderEngine;
		uint32 engineBoundID;
		uint32 numIndices;

		NULL_COPY_AND_ASSIGN(VertexArray);
	};

}


#endif