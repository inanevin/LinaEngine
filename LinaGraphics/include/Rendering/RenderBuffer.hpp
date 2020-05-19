/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderBuffer
Timestamp: 5/19/2020 8:10:11 PM

*/
#pragma once

#ifndef RenderBuffer_HPP
#define RenderBuffer_HPP

#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine::Graphics
{
	class RenderBuffer
	{

	public:

		RenderBuffer() {};
		~RenderBuffer() { m_RBO = m_RenderDevice->ReleaseRenderBufferObject(m_RBO); };
		
		FORCEINLINE void Construct(RenderDevice& renderDeviceIn, RenderBufferStorage storage, uint32 width, uint32 height)
		{
			m_RenderDevice->CreateRenderBufferObject(storage, width, height);
		}

	private:

		RenderDevice* m_RenderDevice;
		uint32 m_RBO;
	};
}

#endif
