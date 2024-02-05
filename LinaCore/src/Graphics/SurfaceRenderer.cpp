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

#include "Core/Graphics/SurfaceRenderer.hpp"
#include "Core/Graphics/GUIBackend.hpp"
#include "Core/Graphics/Interfaces/IGUIDrawer.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/SystemInfo.hpp"
#include "Core/Graphics/Data/Vertex.hpp"
#include "Core/Graphics/Resource/Shader.hpp"

namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

#define MAX_GUI_VERTICES	  5000
#define MAX_GUI_INDICES		  5000
#define MAX_GUI_MATERIALS	  100
#define MAX_GFX_COMMANDS	  250
#define MAX_TRANSFER_COMMANDS 150

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, LinaGX::Window* window, StringID sid, const Vector2ui& initialSize) : m_gfxManager(man), m_window(window), m_sid(sid), m_size(initialSize)
	{
		m_gfxManager->GetSystem()->AddListener(this);
		m_guiBackend = m_gfxManager->GetGUIBackend();
		m_lgx		 = m_gfxManager->GetLGX();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.gfxStream	   = m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "SurfaceRenderer: Stream"});
			data.guiCopyStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_TRANSFER_COMMANDS, 12000, 1024, 32, "SurfaceRenderer: Stream"});
			data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "Surface Renderer GUI Vertex Buffer");
			data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "Surface Renderer GUI Index Buffer");
			data.guiMaterialBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, MAX_GUI_MATERIALS * sizeof(GUIBackend::GPUGUIMaterialData));
			data.guiCopySemaphore = m_lgx->CreateUserSemaphore();

			LinaGX::ResourceDesc resourceDesc = {
				.size		   = sizeof(GPUSceneData),
				.typeHintFlags = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
				.heapType	   = LinaGX::ResourceHeap::StagingHeap,
				.debugName	   = "Surface Renderer GUI Scene Data",
			};

			data.guiSceneResource = m_lgx->CreateResource(resourceDesc);
			m_lgx->MapResource(data.guiSceneResource, data.guiSceneDataMapping);

			// Set 1 - Scene Data
			{
				LinaGX::DescriptorBinding set1Binding = {
					.descriptorCount = 1,
					.type			 = LinaGX::DescriptorType::UBO,
					.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
				};

				data.guiDescriptorSet1 = m_lgx->CreateDescriptorSet({.bindings = {set1Binding}});

				m_lgx->DescriptorUpdateBuffer({
					.setHandle = data.guiDescriptorSet1,
					.buffers   = {data.guiSceneResource},
				});
			}

			// Set 2 - Material Data
			{
				// Material data
				LinaGX::DescriptorBinding set2Binding = {
					.descriptorCount = 1,
					.type			 = LinaGX::DescriptorType::SSBO,
					.stages			 = {LinaGX::ShaderStage::Fragment},
				};

				data.guiDescriptorSet2 = m_lgx->CreateDescriptorSet({.bindings = {set2Binding}});

				m_lgx->DescriptorUpdateBuffer({
					.setHandle = data.guiDescriptorSet2,
					.binding   = 0,
					.buffers   = {data.guiMaterialBuffer.GetGPUResource()},
				});
			}
		}

		const auto monitorSize = window->GetMonitorSize();
		const auto windowSize  = window->GetSize();

		m_gfxQueue = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics);

		m_swapchain = m_lgx->CreateSwapchain({
			.format		  = LinaGX::Format::B8G8R8A8_SRGB,
			.x			  = 0,
			.y			  = 0,
			.width		  = windowSize.x,
			.height		  = windowSize.y,
			.window		  = window->GetWindowHandle(),
			.osHandle	  = window->GetOSHandle(),
			.isFullscreen = windowSize.x == monitorSize.x && windowSize.y == monitorSize.y,
			.vsyncStyle	  = m_gfxManager->GetCurrentVsync(),
		});
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			data.guiIndexBuffer.Destroy();
			data.guiVertexBuffer.Destroy();
			data.guiMaterialBuffer.Destroy();
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.guiCopyStream);
			m_lgx->DestroyDescriptorSet(data.guiDescriptorSet1);
			m_lgx->DestroyDescriptorSet(data.guiDescriptorSet2);
			m_lgx->DestroyResource(data.guiSceneResource);
			m_lgx->DestroyUserSemaphore(data.guiCopySemaphore);
		}

		m_lgx->DestroySwapchain(m_swapchain);
		m_gfxManager->GetSystem()->RemoveListener(this);
	}

	void SurfaceRenderer::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
	}

	void SurfaceRenderer::SetGUIDrawer(IGUIDrawer* drawer)
	{
		m_guiDrawer = drawer;
		m_guiDrawer->SetSurfaceRenderer(this);
	}

	void SurfaceRenderer::Present()
	{
		LinaGX::PresentDesc desc = {
			.swapchains		= &m_swapchain,
			.swapchainCount = 1,
		};
		m_lgx->Present(desc);
	}

	void SurfaceRenderer::Resize(const Vector2ui& newSize)
	{
		const LinaGX::LGXVector2ui monitorSize = m_window->GetMonitorSize();

		LinaGX::SwapchainRecreateDesc desc = {
			.swapchain	  = m_swapchain,
			.width		  = newSize.x,
			.height		  = newSize.y,
			.isFullscreen = newSize.x == monitorSize.x && newSize.y == monitorSize.y,
		};

		m_lgx->RecreateSwapchain(desc);
		m_size = newSize;
	}

	void SurfaceRenderer::Tick()
	{
		auto ws		= m_window->GetSize();
		m_isVisible = m_window->GetIsVisible() && ws.x != 0 && ws.y && !m_window->GetIsMinimized();
	}

	void SurfaceRenderer::Render(int guiThreadID, uint32 frameIndex)
	{
		if (!IsVisible())
			return;

		auto& currentFrame = m_pfd[frameIndex];

		const uint64 copySemaphoreValue = currentFrame.guiCopySemaphoreValue;

		LinaGX::Viewport viewport = {
			.x		  = 0,
			.y		  = 0,
			.width	  = m_size.x,
			.height	  = m_size.y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		LinaGX::ScissorsRect scissors = {
			.x		= 0,
			.y		= 0,
			.width	= m_size.x,
			.height = m_size.y,
		};

		// Descriptors
		{
			LinaGX::CMDBindDescriptorSets* bind = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
			bind->descriptorSetHandles			= currentFrame.gfxStream->EmplaceAuxMemory<uint16>(m_gfxManager->GetDescriptorSet0());
			bind->firstSet						= 0;
			bind->setCount						= 1;
		}

		// Barrier to Color Attachment
		{
			LinaGX::CMDBarrier* barrier				 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags					 = LinaGX::PSF_TopOfPipe;
			barrier->dstStageFlags					 = LinaGX::PSF_ColorAttachment;
			barrier->textureBarrierCount			 = 1;
			barrier->textureBarriers				 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
			barrier->textureBarriers->srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite;
			barrier->textureBarriers->dstAccessFlags = LinaGX::AF_ColorAttachmentRead;
			barrier->textureBarriers->isSwapchain	 = true;
			barrier->textureBarriers->texture		 = static_cast<uint32>(m_swapchain);
			barrier->textureBarriers->toState		 = LinaGX::TextureBarrierState::ColorAttachment;
		}

		// Begin render pass
		{
			LinaGX::RenderPassColorAttachment colorAtt = {
				.texture	 = static_cast<uint32>(m_swapchain),
				.isSwapchain = true,
			};

			LinaGX::CMDBeginRenderPass* rp = currentFrame.gfxStream->AddCommand<LinaGX::CMDBeginRenderPass>();
			rp->colorAttachmentCount	   = 1;
			rp->colorAttachments		   = currentFrame.gfxStream->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(colorAtt);
			rp->viewport				   = viewport;
			rp->scissors				   = scissors;
		}

		// TODO: Draw full-screen quad texture if set.
		//
		// // Draw GUI if set.
		// if (m_guiDrawer != nullptr)
		// {
		// 	GUIBackend::GUIRenderData guiRenderData = {
		// 		.size				= m_size,
		// 		.gfxStream			= currentFrame.gfxStream,
		// 		.copyStream			= currentFrame.guiCopyStream,
		// 		.copySemaphore		= currentFrame.guiCopySemaphore,
		// 		.copySemaphoreValue = &currentFrame.guiCopySemaphoreValue,
		// 		.vertexBuffer		= &currentFrame.guiVertexBuffer,
		// 		.indexBuffer		= &currentFrame.guiIndexBuffer,
		// 		.materialBuffer		= &currentFrame.guiMaterialBuffer,
		// 		.descriptorSet1		= currentFrame.guiDescriptorSet1,
		// 		.descriptorSet2		= currentFrame.guiDescriptorSet2,
		// 		.sceneDataMapping	= currentFrame.guiSceneDataMapping,
		// 		.variantPassType	= ShaderVariantPassType::Swapchain,
		// 	};
		//
		// 	guiRenderData.drawRequests.reserve(50);
		//
		// 	m_guiDrawer->DrawGUI(guiThreadID);
		// 	m_guiBackend->Prepare(guiThreadID, guiRenderData);
		// 	LinaVG::Render(guiThreadID);
		// 	m_guiBackend->Render(guiThreadID);
		// }
		//

		// Set shader
		{
			LinaGX::CMDBindPipeline* bindPipeline = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindPipeline>();
			bindPipeline->shader				  = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->GetResource<Shader>("Resources/Core/Shaders/Test.linashader"_hs)->GetGPUHandle();
		}

		// Draw the triangle
		{
			LinaGX::CMDDrawInstanced* drawInstanced = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawInstanced>();
			drawInstanced->vertexCountPerInstance	= 3;
			drawInstanced->instanceCount			= 1;
		}

		// End render pass
		{
			LinaGX::CMDEndRenderPass* end = currentFrame.gfxStream->AddCommand<LinaGX::CMDEndRenderPass>();
		}

		// Barrier to Present
		{
			LinaGX::CMDBarrier* barrier				 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags					 = LinaGX::PSF_ColorAttachment;
			barrier->dstStageFlags					 = LinaGX::PSF_BottomOfPipe;
			barrier->textureBarrierCount			 = 1;
			barrier->textureBarriers				 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
			barrier->textureBarriers->srcAccessFlags = LinaGX::AF_ColorAttachmentWrite;
			barrier->textureBarriers->dstAccessFlags = 0;
			barrier->textureBarriers->isSwapchain	 = true;
			barrier->textureBarriers->texture		 = static_cast<uint32>(m_swapchain);
			barrier->textureBarriers->toState		 = LinaGX::TextureBarrierState::Present;
		}

		// Send
		{
			const auto& uploadQueue = m_gfxManager->GetResourceUploadQueue();

			m_waitSemaphores.clear();
			m_waitValues.clear();

			if (copySemaphoreValue != currentFrame.guiCopySemaphoreValue)
			{
				m_waitSemaphores.push_back(currentFrame.guiCopySemaphore);
				m_waitValues.push_back(currentFrame.guiCopySemaphoreValue);
			}

			if (uploadQueue.HasTransfer())
			{
				m_waitSemaphores.push_back(uploadQueue.GetSemaphore());
				m_waitValues.push_back(uploadQueue.GetSemaphoreValue());
			}

			m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);

			// LinaGX::SubmitDesc desc = {
			// 	.targetQueue	 = m_gfxQueue,
			// 	.streams		 = &currentFrame.gfxStream,
			// 	.streamCount	 = 1,
			// 	.useWait		 = !m_waitSemaphores.empty(),
			// 	.waitCount		 = static_cast<uint32>(m_waitSemaphores.size()),
			// 	.waitSemaphores	 = m_waitSemaphores.data(),
			// 	.waitValues		 = m_waitValues.data(),
			// 	.isMultithreaded = true,
			// };
			//
			// m_lgx->SubmitCommandStreams(desc);
		}
	}

} // namespace Lina
