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

#include "SwapchainRenderer.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Application.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{
	SwapchainRenderer::SwapchainRenderer(Application* app, LinaGX::Instance* lgx, LinaGX::Window* window, bool vsyncOn)
	{
		m_app	 = app;
		m_rm	 = &app->GetResourceManager();
		m_lgx	 = lgx;
		m_window = window;
		m_size	 = window->GetSize();
		m_vsync	 = vsyncOn;

		const LinaGX::DescriptorSetDesc setDesc = GfxHelpers::GetSetDescPersistentGlobal();
		const RenderPassDescription		rpDesc	= {
				 .buffers		 = {},
				 .setDescription = setDesc,
		 };
		m_renderPass.Create(GfxHelpers::GetRenderPassDescription(RenderPassType::RENDER_PASS_SWAPCHAIN));
		m_renderPass.SetSize(m_size);

		m_shaderSwapchain  = m_rm->GetResource<Shader>(ENGINE_SHADER_SWAPCHAIN_ID);
		m_samplerSwapchain = m_rm->CreateResource<TextureSampler>(m_rm->ConsumeResourceID());

		LinaGX::SamplerDesc samplerData = {};
		samplerData.minFilter			= LinaGX::Filter::Nearest;
		samplerData.magFilter			= LinaGX::Filter::Nearest;
		samplerData.mode				= LinaGX::SamplerAddressMode::ClampToEdge;
		samplerData.anisotropy			= 6;
		samplerData.borderColor			= LinaGX::BorderColor::WhiteOpaque;
		samplerData.mipLodBias			= 0.0f;
		samplerData.minLod				= 0.0f;
		samplerData.maxLod				= 0;
		m_samplerSwapchain->GenerateHW(samplerData);

		const LinaGX::VSyncStyle vsync = {
			.vulkanVsync = m_vsync ? LinaGX::VKVsync::FIFO : LinaGX::VKVsync::None,
			.dx12Vsync	 = m_vsync ? LinaGX::DXVsync::EveryVBlank : LinaGX::DXVsync::None,
		};

		const auto monitorSize = window->GetMonitorSize();
		const auto windowSize  = window->GetSize();
		m_swapchain			   = m_lgx->CreateSwapchain({
					   .format		  = SystemInfo::GetSwapchainFormat(),
					   .x			  = 0,
					   .y			  = 0,
					   .width		  = windowSize.x,
					   .height		  = windowSize.y,
					   .window		  = window->GetWindowHandle(),
					   .osHandle	  = window->GetOSHandle(),
					   .isFullscreen  = windowSize.x == monitorSize.x && windowSize.y == monitorSize.y,
					   .vsyncStyle	  = vsync,
					   .scalingFactor = window->GetDPIScale(),
		   });

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& data = m_pfd[i];
			data.gfxStream	   = m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, 20, 24000, 4096, 32, "SwapchainRenderer: Gfx Stream"});
			data.copyStream	   = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, 20, 4000, 1024, 32, "SwapchainRenderer: CopyStream"});
			data.copySemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
			m_renderPass.SetColorAttachment(i, 0, {.clearColor = {0, 0, 0, 0}, .texture = static_cast<uint32>(m_swapchain), .isSwapchain = true});
		}

		m_pipelineLayout = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(RenderPassType::RENDER_PASS_SWAPCHAIN));
		m_app->GetGfxContext().MarkBindlessDirty();
	}

	SwapchainRenderer::~SwapchainRenderer()
	{
		m_lgx->DestroySwapchain(m_swapchain);
		m_lgx->DestroyPipelineLayout(m_pipelineLayout);
		m_samplerSwapchain->DestroyHW();
		m_rm->DestroyResource(m_samplerSwapchain);
		m_samplerSwapchain = nullptr;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());
		}

		m_renderPass.Destroy();
	}

	void SwapchainRenderer::OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize)
	{
		if (window != m_window)
			return;
		const LinaGX::VSyncStyle vsync = {
			.vulkanVsync = m_vsync ? LinaGX::VKVsync::FIFO : LinaGX::VKVsync::None,
			.dx12Vsync	 = m_vsync ? LinaGX::DXVsync::EveryVBlank : LinaGX::DXVsync::None,
		};

		const LinaGX::LGXVector2ui monitorSize = m_window->GetMonitorSize();

		LinaGX::SwapchainRecreateDesc desc = {
			.swapchain	   = m_swapchain,
			.width		   = newSize.x,
			.height		   = newSize.y,
			.isFullscreen  = newSize.x == monitorSize.x && newSize.y == monitorSize.y,
			.vsyncStyle	   = vsync,
			.scalingFactor = m_window->GetDPIScale(),
		};

		m_lgx->RecreateSwapchain(desc);
		m_size = newSize;

		m_renderPass.SetSize(m_size);
		m_app->GetGfxContext().MarkBindlessDirty();
	}

	bool SwapchainRenderer::CheckVisibility()
	{
		auto ws		= m_window->GetSize();
		m_isVisible = m_window->GetIsVisible() && ws.x != 0 && ws.y != 0 && !m_window->GetIsMinimized();
		m_lgx->SetSwapchainActive(m_swapchain, m_isVisible);
		return m_isVisible;
	}

	void SwapchainRenderer::Tick()
	{
		if (m_wr)
		{
			RenderPass::InstancedDraw draw = {
				.shaderHandle  = m_shaderSwapchain->GetGPUHandle(),
				.vertexCount   = 3,
				.instanceCount = 1,
			};
			m_renderPass.AddDrawCall(draw);
		}
	}

	void SwapchainRenderer::SyncRender()
	{
		m_renderPass.SyncRender();
	}

	LinaGX::CommandStream* SwapchainRenderer::Render(uint32 frameIndex)
	{
		PerFrameData& currentFrame = m_pfd[frameIndex];

		// Transfer
		{
			const uint32 txtFrameIndex = (frameIndex + SystemInfo::GetRendererBehindFrames()) % FRAMES_IN_FLIGHT;

			GPUDataSwapchainPass pass = {
				.textureIndex = m_wr ? m_wr->GetLightingPassOutput(txtFrameIndex)->GetBindlessIndex() : 0,
				.samplerIndex = m_samplerSwapchain->GetBindlessIndex(),
			};

			m_renderPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&pass, sizeof(GPUDataSwapchainPass));
			m_renderPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

			if (m_uploadQueue.FlushAll(currentFrame.copyStream))
			{
				currentFrame.copySemaphore.Increment();
				m_lgx->CloseCommandStreams(&currentFrame.copyStream, 1);
				m_lgx->SubmitCommandStreams({
					.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
					.streams		  = &currentFrame.copyStream,
					.streamCount	  = 1,
					.useSignal		  = true,
					.signalCount	  = 1,
					.signalSemaphores = currentFrame.copySemaphore.GetSemaphorePtr(),
					.signalValues	  = currentFrame.copySemaphore.GetValuePtr(),
				});
			}
		}

		LinaGX::CMDBindDescriptorSets* bindGlobal = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bindGlobal->descriptorSetHandles		  = currentFrame.gfxStream->EmplaceAuxMemory<uint16>(m_app->GetGfxContext().GetDescriptorSetGlobal(frameIndex));
		bindGlobal->firstSet					  = 0;
		bindGlobal->setCount					  = 1;
		bindGlobal->layoutSource				  = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bindGlobal->customLayout				  = m_app->GetGfxContext().GetPipelineLayoutGlobal();

		// Present2Color
		LinaGX::CMDBarrier* barrierToColor	= currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToColor->srcStageFlags		= LinaGX::PSF_TopOfPipe;
		barrierToColor->dstStageFlags		= LinaGX::PSF_ColorAttachment;
		barrierToColor->textureBarrierCount = 1;
		barrierToColor->textureBarriers		= currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
		barrierToColor->textureBarriers[0]	= GfxHelpers::GetTextureBarrierPresent2Color(static_cast<uint32>(m_swapchain), true);

		DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Surface Swapchain");
		m_renderPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_pipelineLayout, 1);
		m_renderPass.Begin(currentFrame.gfxStream, frameIndex);
		m_renderPass.Render(frameIndex, currentFrame.gfxStream);
		m_renderPass.End(currentFrame.gfxStream);
		DEBUG_LABEL_END(currentFrame.gfxStream);

		// Barrier2Present
		LinaGX::CMDBarrier* barrierToPresent  = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToPresent->srcStageFlags		  = LinaGX::PSF_ColorAttachment;
		barrierToPresent->dstStageFlags		  = LinaGX::PSF_BottomOfPipe;
		barrierToPresent->textureBarrierCount = 1;
		barrierToPresent->textureBarriers	  = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
		barrierToPresent->textureBarriers[0]  = GfxHelpers::GetTextureBarrierColor2Present(static_cast<uint32>(m_swapchain), true);

		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);

		return currentFrame.gfxStream;
	}

} // namespace Lina
