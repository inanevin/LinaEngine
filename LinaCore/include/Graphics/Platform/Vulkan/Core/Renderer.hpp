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

#ifndef Renderer_HPP
#define Renderer_HPP

#include "Data/Bitmask.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Platform/Vulkan/Utility/VulkanStructures.hpp"
#include "Graphics/Platform/Vulkan/Objects/CommandPool.hpp"
#include "Graphics/Platform/Vulkan/Objects/CommandBuffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/DescriptorPool.hpp"

namespace Lina
{
	class GfxManager;
	class Fence;
	class Swapchain;
	class EntityWorld;

	class Renderer
	{
	public:
		Renderer(GfxManager* manager, Bitmask16 mask, RendererType type);
		virtual ~Renderer();

		virtual void		   SyncData(){};
		virtual void		   Tick(){};
		virtual void		   OnPostPresent(VulkanResult res){};
		virtual void		   AcquiredImageInvalid(uint32 frameIndex){};
		virtual CommandBuffer* Render(uint32 frameIndex, Fence& fence) = 0;

		virtual Swapchain* GetSwapchain() const
		{
			return nullptr;
		};

		virtual EntityWorld* GetWorld() const
		{
			return nullptr;
		}

		inline const Bitmask16& GetMask() const
		{
			return m_mask;
		}

		virtual bool AcquireImage(uint32 frameIndex)
		{
			return true;
		};

		virtual uint32 GetAcquiredImage() const
		{
			return 0;
		};

		inline RendererType GetType() const
		{
			return m_type;
		}

	protected:
		GfxManager*			  m_gfxManager = nullptr;
		RendererType		  m_type	   = RendererType::None;
		Bitmask16			  m_mask	   = 0;
		CommandPool			  m_cmdPool;
		DescriptorPool		  m_descriptorPool;
		Vector<CommandBuffer> m_cmds;
		bool				  m_initialized = false;
	};

} // namespace Lina

#endif
