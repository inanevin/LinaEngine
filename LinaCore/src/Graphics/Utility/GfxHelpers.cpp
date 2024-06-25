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
#include "Core/Graphics/Data/RenderData.hpp"

namespace Lina
{
	LinaGX::DescriptorSetDesc GfxHelpers::GetSetDescPersistentGlobal()
	{
		LinaGX::DescriptorBinding binding0 = {
			.descriptorCount = 1,
			.type			 = LinaGX::DescriptorType::UBO,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding1 = {
			.descriptorCount = 1,
			.type			 = LinaGX::DescriptorType::SSBO,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding2 = {
			.descriptorCount = 1000,
			.type			 = LinaGX::DescriptorType::SeparateImage,
			.unbounded		 = true,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding3 = {
			.descriptorCount = 1000,
			.type			 = LinaGX::DescriptorType::SeparateSampler,
			.unbounded		 = true,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {binding0, binding1, binding2, binding3}};
	}

	LinaGX::DescriptorSetDesc GfxHelpers::GetSetDescPersistentRenderPass(RenderPassDescriptorType type)
	{
		if (type == RenderPassDescriptorType::Gui)
		{
			LinaGX::DescriptorBinding binding0 = {
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding1 = {
				.type	= LinaGX::DescriptorType::SSBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};
			return {.bindings = {binding0, binding1}};
		}
		else if (type == RenderPassDescriptorType::Main)
		{
			LinaGX::DescriptorBinding binding0 = {
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding1 = {
				.type	= LinaGX::DescriptorType::SSBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding2 = {
				.type	= LinaGX::DescriptorType::SSBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			return {.bindings = {binding0, binding1, binding2}};
		}
		else if (type == RenderPassDescriptorType::ForwardTransparency)
		{
			LinaGX::DescriptorBinding binding0 = {
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			LinaGX::DescriptorBinding binding1 = {
				.type	= LinaGX::DescriptorType::SSBO,
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

			return {.bindings = {binding0, binding1, binding2, binding3}};
		}
		else if (type == RenderPassDescriptorType::Lighting)
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
				.type	= LinaGX::DescriptorType::UBO,
				.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
			};

			return {.bindings = {binding0, binding1, binding2}};
		}

		LINA_ASSERT(false, "");
		return {};
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

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierUndef2TransferDest(uint32 texture)
	{
		return {
			.texture		= texture,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureBarrierState::TransferDestination,
			.srcAccessFlags = 0,
			.dstAccessFlags = LinaGX::AF_TransferWrite,
		};
	}

	LinaGX::TextureBarrier GfxHelpers::GetTextureBarrierTransferDest2Sampled(uint32 texture)
	{
		return {
			.texture		= texture,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureBarrierState::ShaderRead,
			.srcAccessFlags = LinaGX::AF_TransferWrite,
			.dstAccessFlags = LinaGX::AF_ShaderRead,
		};
	}

	LinaGX::PipelineLayoutDesc GfxHelpers::GetPLDescPersistentGlobal()
	{
		return {.descriptorSetDescriptions = {GetSetDescPersistentGlobal()}, .debugName = "Persistent Global Layout"};
	}

	LinaGX::PipelineLayoutDesc GfxHelpers::GetPLDescPersistentRenderPass(RenderPassDescriptorType renderPassType)
	{
		LinaGX::PipelineLayoutDesc desc;
		desc.descriptorSetDescriptions = {GetSetDescPersistentGlobal(), GetSetDescPersistentRenderPass(renderPassType)};
		desc.debugName				   = "Persistent RenderPass Layout";
		// if (renderPassType == RenderPassDescriptorType::Main)
		//	desc.constantRanges.push_back(LinaGX::PipelineLayoutPushConstantRange{.stages = {LinaGX::ShaderStage::Vertex}, .size = sizeof(GPUPushConstantRPMain)});

		return desc;
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

	RenderPassDescription GfxHelpers::GetRenderPassDescription(LinaGX::Instance* lgx, RenderPassDescriptorType type)
	{
		LinaGX::DescriptorSetDesc setDesc = GfxHelpers::GetSetDescPersistentRenderPass(type);

		if (type == RenderPassDescriptorType::Gui)
		{
			return {
				.buffers		= {{
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
									   .debugName	 = "RP: GUI - ViewData",
									   .size		 = sizeof(GPUDataView),
									   .stagingOnly	 = true,
									   .bindingIndex = 0,
									   .ident		 = "ViewData"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_StorageBuffer,
									   .debugName	 = "RP: GUI - GUIMaterials",
									   .size		 = sizeof(GPUMaterialGUI) * 250,
									   .stagingOnly	 = false,
									   .bindingIndex = 1,
									   .ident		 = "GUIMaterials"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_IndirectBuffer,
									   .debugName	 = "RP: GUI - IndirectBuffer",
									   .size		 = lgx->GetIndexedIndirectCommandSize() * static_cast<size_t>(250),
									   .stagingOnly	 = false,
									   .bindingIndex = -1,
									   .ident		 = "IndirectBuffer"_hs,
							   }},
				.setDescription = setDesc,
			};
		}

		if (type == RenderPassDescriptorType::Main)
		{
			// 1 is object buffer reserved.
			return {
				.buffers		= {{
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
									   .debugName	 = "RP: Main - ViewData",
									   .size		 = sizeof(GPUDataView),
									   .stagingOnly	 = true,
									   .bindingIndex = 0,
									   .ident		 = "ViewData"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_IndirectBuffer,
									   .debugName	 = "RP: Main - IndirectBuffer",
									   .size		 = lgx->GetIndexedIndirectCommandSize() * static_cast<size_t>(250),
									   .stagingOnly	 = false,
									   .bindingIndex = -1,
									   .ident		 = "IndirectBuffer"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_StorageBuffer,
									   .debugName	 = "RP: Main - IndirectConstants",
									   .size		 = sizeof(GPUIndirectConstants0) * 2500,
									   .stagingOnly	 = false,
									   .bindingIndex = 2,
									   .ident		 = "IndirectConstants"_hs,
							   }},
				.setDescription = setDesc,
			};
		}

		if (type == RenderPassDescriptorType::Lighting)
		{
			return {
				.buffers =
					{
						{
							.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
							.debugName	  = "RP: FWTransparency - ViewData",
							.size		  = sizeof(GPUDataView),
							.stagingOnly  = true,
							.bindingIndex = 0,
							.ident		  = "ViewData"_hs,
						},
						{
							.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
							.debugName	  = "RP: FWTransparency - Atmosphere",
							.size		  = sizeof(GPUDataAtmosphere),
							.stagingOnly  = true,
							.bindingIndex = 1,
							.ident		  = "AtmosphereData"_hs,
						},
						{
							.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
							.debugName	  = "RP: FWTransparency - DeferredLightingPassData",
							.size		  = sizeof(GPUDataDeferredLightingPass),
							.stagingOnly  = true,
							.bindingIndex = 2,
							.ident		  = "PassData"_hs,
						},

					},
				.setDescription = setDesc,
			};
		}

		if (type == RenderPassDescriptorType::ForwardTransparency)
		{
			// 1 is object buffer reserved.
			return {
				.buffers		= {{
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
									   .debugName	 = "RP: FWTransparency - ViewData",
									   .size		 = sizeof(GPUDataView),
									   .stagingOnly	 = true,
									   .bindingIndex = 0,
									   .ident		 = "ViewData"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_IndirectBuffer,
									   .debugName	 = "RP: FWTransparency - IndirectBuffer",
									   .size		 = lgx->GetIndexedIndirectCommandSize() * static_cast<size_t>(250),
									   .stagingOnly	 = false,
									   .bindingIndex = -1,
									   .ident		 = "IndirectBuffer"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_StorageBuffer,
									   .debugName	 = "RP: FWTransparency - IndirectConstants",
									   .size		 = sizeof(GPUIndirectConstants0) * 2500,
									   .stagingOnly	 = false,
									   .bindingIndex = 2,
									   .ident		 = "IndirectConstants"_hs,
							   },
								   {
									   .bufferType	 = LinaGX::ResourceTypeHint::TH_StorageBuffer,
									   .debugName	 = "RP: FWTransparency - GUIMaterials",
									   .size		 = 100 * sizeof(GPUMaterialGUI),
									   .stagingOnly	 = false,
									   .bindingIndex = 3,
									   .ident		 = "GUIMaterials"_hs,
							   }},
				.setDescription = setDesc,
			};
		}
	}
} // namespace Lina
