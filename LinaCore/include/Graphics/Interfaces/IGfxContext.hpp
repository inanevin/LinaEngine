/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#ifndef IGfxContext_HPP
#define IGfxContext_HPP

#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Data/Vector.hpp"

namespace Lina
{
	class Renderer;
	class Recti;
	class Texture;
	class Shader;
	class Material;
	class IGfxResourceGPU;
	class IGfxResourceCPU;
	class GfxManager;

	class IGfxContext
	{
	public:
		IGfxContext(Renderer* renderer, CommandType contextType) : m_renderer(renderer), m_contextType(contextType){};
		virtual ~IGfxContext() = default;

		virtual uint32 CreateCommandAllocator()																																							  = 0;
		virtual uint32 CreateCommandList(uint32 allocator)																																				  = 0;
		virtual void   ReleaseCommandAllocator(uint32 allocator)																																		  = 0;
		virtual void   ReleaseCommandList(uint32 handle)																																				  = 0;
		virtual void   ResetCommandListAndAllocator(uint32 cmdAllocatorHandle, uint32 cmdListHandle)																									  = 0;
		virtual void   ResetCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle)																												  = 0;
		virtual void   ResetCommandAllocator(uint32 cmdAllocatorHandle)																																	  = 0;
		virtual void   PrepareCommandList(uint32 cmdListHandle, const Viewport& viewport, const Recti& scissors)																						  = 0;
		virtual void   SetScissors(uint32 cmdListHandle, const Recti& scissors)																															  = 0;
		virtual void   FinalizeCommandList(uint32 cmdListHandle)																																		  = 0;
		virtual void   ExecuteCommandLists(const Vector<uint32>& lists)																																	  = 0;
		virtual void   ResourceBarrier(uint32 cmdListHandle, ResourceTransition* transitions, uint32 count)																								  = 0;
		virtual void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture)																														  = 0;
		virtual void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencil)																								  = 0;
		virtual void   EndRenderPass(uint32 cmdListHandle)																																				  = 0;
		virtual void   BindPipeline(uint32 cmdListHandle, Shader* shader)																																  = 0;
		virtual void   BindMaterials(Material** materials, uint32 materialsSize)																														  = 0;
		virtual void   SetMaterialID(uint32 cmdListHandle, uint32 id)																																	  = 0;
		virtual void   BindDynamicTextures(Texture** textures, uint32 texturesSize)																														  = 0;
		virtual void   BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxResourceCPU* buf)																								  = 0;
		virtual void   BindObjectBuffer(uint32 cmdListHandle, IGfxResourceGPU* res)																														  = 0;
		virtual void   BindVertexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t vertexSize = sizeof(Vertex), uint32 slot = 0, size_t maxSize = 0)											  = 0;
		virtual void   BindIndexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t maxSize = 0)																								  = 0;
		virtual void   DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance)															  = 0;
		virtual void   DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) = 0;
		virtual void   DrawIndexedIndirect(uint32 cmdListHandle, IGfxResourceCPU* indirectBuffer, uint32 count, uint64 indirectOffset)																	  = 0;
		virtual void   SetTopology(uint32 cmdListHandle, Topology topology)																																  = 0;
		virtual void   WaitForFences(uint32 fenceHandle, uint64 frameFenceValue)																														  = 0;
		virtual void   Signal(uint32 fenceHandle, uint64 frameFenceValue)																																  = 0;
		virtual void   Wait(uint32 fenceHandle, uint64 frameFenceValue)																																	  = 0;
		virtual void   Copy(IGfxResourceCPU* staging, IGfxResourceGPU* destination, uint32 cmdList)																										  = 0;

	protected:
		Renderer*	m_renderer	  = nullptr;
		CommandType m_contextType = CommandType::Graphics;
	};
} // namespace Lina

#endif
