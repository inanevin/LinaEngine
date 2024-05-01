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

#include "Core/Graphics/Utility/GfxHelpers.hpp"

namespace Lina
{
	LinaGX::DescriptorSetDesc GfxHelpers::GetSetDescPersistentGlobal()
	{
		LinaGX::DescriptorBinding binding0 = {
			.descriptorCount = 1,
			.type			 = LinaGX::DescriptorType::UBO,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {binding0}};
	}

	LinaGX::DescriptorSetDesc GfxHelpers::GetSetDescPersistentRenderPass(RenderPassDescriptorType type)
	{
		if (type == RenderPassDescriptorType::Basic)
		{
			LinaGX::DescriptorBinding binding0 = {
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			return {.bindings = {binding0}};
		}
		else if (type == RenderPassDescriptorType::Main)
		{
			LinaGX::DescriptorBinding binding0 = {
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding1 = {
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding2 = {
				.type	= LinaGX::DescriptorType::SSBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding3 = {
				.type	= LinaGX::DescriptorType::SSBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding4 = {
				.type	= LinaGX::DescriptorType::SSBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			return {.bindings = {binding0, binding1, binding2, binding3, binding4}};
		}

		LINA_ASSERT(false, "");
		return {};
	}

	LinaGX::DescriptorSetDesc GfxHelpers::GetSetDescriptionDynamicGUI()
	{

		LinaGX::DescriptorBinding binding0 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding1 = {
			.type	= LinaGX::DescriptorType::SeparateSampler,
			.stages = {LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding2 = {
			.type	= LinaGX::DescriptorType::SeparateImage,
			.stages = {LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {binding0, binding1, binding2}};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierColor2Present(uint32 texture, bool isSwapchain)
	{
		return {
			.texture		= texture,
			.isSwapchain	= isSwapchain,
			.toState		= LinaGX::TextureBarrierState::Present,
			.srcAccessFlags = LinaGX::AF_ColorAttachmentWrite,
			.dstAccessFlags = 0,
		};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierPresent2Color(uint32 texture, bool isSwapchain)
	{
		return {
			.texture		= texture,
			.isSwapchain	= isSwapchain,
			.toState		= LinaGX::TextureBarrierState::ColorAttachment,
			.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite,
			.dstAccessFlags = LinaGX::AF_ColorAttachmentRead,
		};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierColorRead2Att(uint32 texture)
	{
		return {
			.texture		= texture,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureBarrierState::ColorAttachment,
			.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite,
			.dstAccessFlags = LinaGX::AF_ColorAttachmentRead,
		};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierColorAtt2Read(uint32 texture)
	{
		return {
			.texture		= texture,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureBarrierState::ShaderRead,
			.srcAccessFlags = LinaGX::AF_ColorAttachmentRead,
			.dstAccessFlags = LinaGX::AF_ShaderRead,
		};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierDepthRead2Att(uint32 texture)
	{
		return {
			.texture		= texture,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureBarrierState::DepthStencilAttachment,
			.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite,
			.dstAccessFlags = LinaGX::AF_DepthStencilAttachmentRead,
		};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierDepthAtt2Read(uint32 texture)
	{
		return {
			.texture		= texture,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureBarrierState::ShaderRead,
			.srcAccessFlags = LinaGX::AF_DepthStencilAttachmentRead,
			.dstAccessFlags = LinaGX::AF_ShaderRead,
		};
	}

	LinaGX::PipelineLayoutDesc GfxHelpers::GetPLDescPersistentGlobal()
	{
		return {.descriptorSetDescriptions = {GetSetDescPersistentGlobal()}, .debugName = "Persistent Global Layout"};
	}

	LinaGX::PipelineLayoutDesc GfxHelpers::GetPLDescPersistentRenderPass(RenderPassDescriptorType renderpassType)
	{
		return {.descriptorSetDescriptions = {GetSetDescPersistentGlobal(), GetSetDescPersistentRenderPass(renderpassType)}, .debugName = "Persistent Renderpass Layout"};
	}

	LinaGX::DescriptorBinding GfxHelpers::GetBindingFromShaderBinding(const LinaGX::ShaderDescriptorSetBinding& b)
	{
		return {
			.descriptorCount  = b.descriptorCount,
			.type			  = b.type,
			.unbounded		  = b.descriptorCount == 0,
			.useDynamicOffset = false,
			.isWritable		  = b.isWritable,
			.stages			  = b.stages,
		};
	}

	Matrix4 GfxHelpers::GetProjectionFromSize(const Vector2ui& size)
	{
		float		L	 = static_cast<float>(0.0f);
		float		R	 = static_cast<float>(size.x);
		float		B	 = static_cast<float>(size.y);
		float		T	 = static_cast<float>(0.0);
		const float zoom = 1.0f;
		L *= zoom;
		R *= zoom;
		T *= zoom;
		B *= zoom;
		return Matrix4::Orthographic(L, R, B, T, 0.0f, 1.0f);
	}
} // namespace Lina
