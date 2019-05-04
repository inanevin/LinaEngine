/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderTarget
Timestamp: 5/4/2019 1:57:14 AM

*/

#include "LinaPch.hpp"
#include "Rendering/RenderTarget.hpp"  

namespace LinaEngine::Graphics
{
	/*void RenderTarget::Construct(PAMRenderDevice* renderDeviceIn)
	{
		m_EngineBoundID = 0;
		renderDevice = renderDeviceIn;
	}*/

/*	void RenderTarget::Construct(PAMRenderDevice* renderDeviceIn, Texture* texture, uint32 width, uint32 height, FramebufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel)
	{
		renderDevice = renderDeviceIn;
		m_EngineBoundID = renderDevice->CreateRenderTarget(texture->GetID(), width, height, attachment, attachmentNumber, mipLevel);
		CheckCompressed(*texture);
	}

	void RenderTarget::Construct(PAMRenderDevice* renderDeviceIn, Texture* texture, FramebufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel)
	{
		renderDevice = renderDeviceIn;
		m_EngineBoundID = renderDevice->CreateRenderTarget(texture->GetID(), texture->GetWidth(), texture->GetHeight(), attachment, attachmentNumber, mipLevel);
		CheckCompressed(*texture);
	}*/
}

