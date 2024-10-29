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
#include "Common/Platform/LinaVGIncl.hpp"

namespace
{
	LINAGX_STRING FormatString(const char* fmt, va_list args)
	{
		// Determine the required size
		va_list args_copy;
		va_copy(args_copy, args);
		int size = vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for the null terminator
		va_end(args_copy);

		// Allocate a buffer and format the string
		std::vector<char> buffer(size);
		vsnprintf(buffer.data(), size, fmt, args);

		return std::string(buffer.data());
	}

	void LinaGX_ErrorCallback(const char* err, ...)
	{
		va_list args;
		va_start(args, err);

		// Use vsnprintf to safely format the string into a buffer.
		char buffer[4096]; // Adjust buffer size as needed.
		vsnprintf(buffer, sizeof(buffer), err, args);

		LINA_ERR(buffer); // Pass the formatted string to LINA_ERR.

		va_end(args);
	}

	void LinaGX_LogCallback(const char* err, ...)
	{
		va_list args;
		va_start(args, err);

		// Use vsnprintf to safely format the string into a buffer.
		char buffer[4096]; // Adjust buffer size as needed.
		vsnprintf(buffer, sizeof(buffer), err, args);

		LINA_INFO(buffer); // Pass the formatted string to LINA_ERR.

		va_end(args);
	}
} // namespace

namespace Lina
{
	LinaGX::Instance* GfxHelpers::InitializeLinaGX()
	{
		// Setup LinaGX
		LinaGX::Instance* lgx = new LinaGX::Instance();

		LinaGX::Config.dx12Config = {
			.allowTearing				 = true,
			.enableDebugLayers			 = true,
			.serializeShaderDebugSymbols = true,
		};

		LinaGX::Config.vulkanConfig = {
			.enableVulkanFeatures = LinaGX::VulkanFeatureFlags::VKF_Bindless | LinaGX::VulkanFeatureFlags::VKF_MultiDrawIndirect,
		};

		LinaGX::Config.gpuLimits = {
			.samplerLimit = 2048,
			.bufferLimit  = 2048,
		};

		LinaGX::Config.logLevel		 = LinaGX::LogLevel::Verbose;
		LinaGX::Config.errorCallback = LinaGX_ErrorCallback;
		LinaGX::Config.infoCallback	 = LinaGX_LogCallback;
		LinaGX::BackendAPI api		 = LinaGX::BackendAPI::DX12;

#ifdef LINA_PLATFORM_APPLE
		api = LinaGX::BackendAPI::Metal;
#endif

		LinaGX::Config.multithreadedQueueSubmission = true;
		LinaGX::Config.api							= api;
		LinaGX::Config.gpu							= LinaGX::PreferredGPUType::Discrete;
		LinaGX::Config.framesInFlight				= FRAMES_IN_FLIGHT;
		LinaGX::Config.backbufferCount				= BACK_BUFFER_COUNT;
		LinaGX::Config.mutexLockCreationDeletion	= true;

		lgx->Initialize();
		return lgx;
	}

	void GfxHelpers::InitializeLinaVG()
	{
		LinaVG::Config.globalAAMultiplier = 1.0f;
		LinaVG::Config.gcCollectInterval  = 4000;
		LinaVG::Config.textCachingEnabled = true;
		LinaVG::Config.textCacheReserve	  = 10000;
		LinaVG::Config.maxFontAtlasSize	  = 1024;
		LinaVG::Config.errorCallback	  = [](const std::string& err) { LINA_ERR(err.c_str()); };
		LinaVG::Config.logCallback		  = [](const std::string& log) { LINA_TRACE(log.c_str()); };
		LinaVG::InitializeText();
	}

	void GfxHelpers::ShutdownLinaVG()
	{
		LinaVG::TerminateText();
	}

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
			.descriptorCount = 100,
			.type			 = LinaGX::DescriptorType::SeparateImage,
			.unbounded		 = true,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding3 = {
			.descriptorCount = 100,
			.type			 = LinaGX::DescriptorType::SeparateSampler,
			.unbounded		 = true,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {binding0, binding1, binding2, binding3}};
	}

	LinaGX::DescriptorSetDesc GfxHelpers::GetSetDescPersistentRenderPass(RenderPassDescriptorType type)
	{
		if (type == RenderPassDescriptorType::Deferred)
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
		else if (type == RenderPassDescriptorType::Forward)
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

			return {.bindings = {binding0, binding1}};
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

		if (type == RenderPassDescriptorType::Deferred)
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
							.debugName	  = "RP: Lighting - ViewData",
							.size		  = sizeof(GPUDataView),
							.stagingOnly  = true,
							.bindingIndex = 0,
							.ident		  = "ViewData"_hs,
						},
						{
							.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
							.debugName	  = "RP: Lighting - PassData",
							.size		  = sizeof(GPUDataLightingPass),
							.stagingOnly  = true,
							.bindingIndex = 1,
							.ident		  = "PassData"_hs,
						},

					},
				.setDescription = setDesc,
			};
		}

		if (type == RenderPassDescriptorType::Forward)
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
							   }},
				.setDescription = setDesc,
			};
		}
		return {};
	}
} // namespace Lina
