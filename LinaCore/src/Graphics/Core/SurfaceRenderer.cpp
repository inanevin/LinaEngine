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

#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Graphics/Core/LGXWrapper.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/SystemInfo.hpp"
#include "Graphics/Data/Vertex.hpp"

namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, LinaGX::Window* window, StringID sid, const Vector2ui& initialSize) : m_gfxManager(man), m_window(window), m_sid(sid), m_size(initialSize)
	{
		m_gfxManager->GetSystem()->AddListener(this);
		m_guiBackend = m_gfxManager->GetGUIBackend();
		m_lgx		 = m_gfxManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper)->GetLGX();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		= m_pfd[i];
			data.gfxStream	= m_lgx->CreateCommandStream(100, LinaGX::QueueType::Graphics);
			data.copyStream = m_lgx->CreateCommandStream(100, LinaGX::QueueType::Transfer);
			data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "Surface Renderer GUI Vertex Buffer");
			data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "Surface Renderer GUI Index Buffer");
			data.guiMaterialBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, MAX_GUI_MATERIALS * sizeof(GUIBackend::GPUGUIMaterialData));
			data.copySemaphore = m_lgx->CreateUserSemaphore();

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
					.binding		 = 0,
					.descriptorCount = 1,
					.type			 = LinaGX::DescriptorType::UBO,
					.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
				};

				LinaGX::DescriptorSetDesc set1Desc = {
					.bindings	   = &set1Binding,
					.bindingsCount = 1,
				};

				data.guiDescriptorSet1 = m_lgx->CreateDescriptorSet(set1Desc);

				LinaGX::DescriptorUpdateBufferDesc sceneDataUpdate = {
					.setHandle		 = data.guiDescriptorSet1,
					.binding		 = 0,
					.descriptorCount = 1,
					.resources		 = &data.guiSceneResource,
					.descriptorType	 = LinaGX::DescriptorType::UBO,
				};

				m_lgx->DescriptorUpdateBuffer(sceneDataUpdate);
			}

			// Set 2 - Material Data
			{
				// Material data
				LinaGX::DescriptorBinding set2Binding = {
					.binding		 = 0,
					.descriptorCount = 1,
					.type			 = LinaGX::DescriptorType::SSBO,
					.stages			 = {LinaGX::ShaderStage::Fragment},
				};

				LinaGX::DescriptorSetDesc set2Desc = {
					.bindings	   = &set2Binding,
					.bindingsCount = 1,
				};

				data.guiDescriptorSet2 = m_lgx->CreateDescriptorSet(set2Desc);

				uint32 res = data.guiMaterialBuffer.GetGPUResource();

				LinaGX::DescriptorUpdateBufferDesc update = {
					.setHandle		 = data.guiDescriptorSet2,
					.binding		 = 0,
					.descriptorCount = 1,
					.resources		 = &res,
					.descriptorType	 = LinaGX::DescriptorType::SSBO,
				};

				m_lgx->DescriptorUpdateBuffer(update);
			}
		}

		const auto monitorSize = window->GetMonitorSize();
		const auto windowSize  = window->GetSize();

		LinaGX::SwapchainDesc swapchainDesc = LinaGX::SwapchainDesc{
			.format		  = LinaGX::Format::B8G8R8A8_UNORM,
			.depthFormat  = LinaGX::Format::D32_SFLOAT,
			.x			  = 0,
			.y			  = 0,
			.width		  = windowSize.x,
			.height		  = windowSize.y,
			.window		  = window->GetWindowHandle(),
			.osHandle	  = window->GetOSHandle(),
			.isFullscreen = windowSize.x == monitorSize.x && windowSize.y == monitorSize.y,
			.vsyncMode	  = m_gfxManager->GetCurrentVsync(),
		};

		m_swapchain = m_lgx->CreateSwapchain(swapchainDesc);
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
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyDescriptorSet(data.guiDescriptorSet1);
			m_lgx->DestroyDescriptorSet(data.guiDescriptorSet2);
			m_lgx->DestroyResource(data.guiSceneResource);
			m_lgx->DestroyUserSemaphore(data.copySemaphore);
		}

		m_lgx->DestroySwapchain(m_swapchain);
		m_gfxManager->GetSystem()->RemoveListener(this);
	}

	void SurfaceRenderer::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_WindowResized)
		{
		}
	}

	void SurfaceRenderer::SetGUIDrawer(IGUIDrawer* drawer)
	{
		m_guiDrawer = drawer;
		m_guiDrawer->SetSurfaceRenderer(this);
	}

	bool SurfaceRenderer::IsVisible()
	{
		return m_window->GetIsVisible();
	}

	void SurfaceRenderer::Render(int guiThreadID, uint32 frameIndex)
	{
		if (!IsVisible())
			return;

		auto& currentFrame = m_pfd[frameIndex];

		const uint64 copySemaphoreValue = currentFrame.copySemaphoreValue;

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

		// Begin render pass
		{
			LinaGX::CMDBeginRenderPass* rp = currentFrame.gfxStream->AddCommand<LinaGX::CMDBeginRenderPass>();
			rp->extension				   = nullptr;
			rp->isSwapchain				   = true;
			rp->swapchain				   = m_swapchain;
			rp->clearColor[0]			   = 0.2f;
			rp->clearColor[1]			   = 0.2f;
			rp->clearColor[2]			   = 0.2f;
			rp->clearColor[3]			   = 1.0f;
			rp->viewport				   = viewport;
			rp->scissors				   = scissors;
		}

		// TODO: Draw full-screen quad texture if set.

		// Draw GUI if set.
		if (m_guiDrawer != nullptr)
		{
			GUIBackend::GUIRenderData guiRenderData = {
				.size				= m_size,
				.gfxStream			= currentFrame.gfxStream,
				.copyStream			= currentFrame.copyStream,
				.copySemaphore		= currentFrame.copySemaphore,
				.copySemaphoreValue = &currentFrame.copySemaphoreValue,
				.vertexBuffer		= &currentFrame.guiVertexBuffer,
				.indexBuffer		= &currentFrame.guiIndexBuffer,
				.materialBuffer		= &currentFrame.guiMaterialBuffer,
				.descriptorSet1		= currentFrame.guiDescriptorSet1,
				.descriptorSet2		= currentFrame.guiDescriptorSet2,
				.sceneDataMapping	= currentFrame.guiSceneDataMapping,
				.variantPassType	= ShaderVariantPassType::Swapchain,
			};

			guiRenderData.drawRequests.reserve(50);

			m_guiDrawer->DrawGUI(guiThreadID);
			m_guiBackend->Prepare(guiThreadID, guiRenderData);
			LinaVG::Render(guiThreadID);
			m_guiBackend->Render(guiThreadID);
		}

		// End render pass
		{
			LinaGX::CMDEndRenderPass* end = currentFrame.gfxStream->AddCommand<LinaGX::CMDEndRenderPass>();
			end->extension				  = nullptr;
			end->isSwapchain			  = true;
			end->swapchain				  = m_swapchain;
		}

		// Send
		{
			const auto& uploadQueue = m_gfxManager->GetResourceUploadQueue();

			Vector<uint16> waitSemaphores;
			Vector<uint64> waitValues;

			if (copySemaphoreValue != currentFrame.copySemaphoreValue)
			{
				waitSemaphores.push_back(currentFrame.copySemaphore);
				waitValues.push_back(currentFrame.copySemaphoreValue);
			}

			if (uploadQueue.HasTransfer())
			{
				waitSemaphores.push_back(uploadQueue.GetSemaphore());
				waitValues.push_back(uploadQueue.GetSemaphoreValue());
			}

			LinaGX::SubmitDesc desc = {
				.streams		= &currentFrame.gfxStream,
				.streamCount	= 1,
				.useWait		= !waitSemaphores.empty(),
				.waitCount		= static_cast<uint32>(waitSemaphores.size()),
				.waitSemaphores = waitSemaphores.data(),
				.waitValues		= waitValues.data(),
			};

			m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);
			m_lgx->SubmitCommandStreams(desc);
		}
	}

	void SurfaceRenderer::Present()
	{
		if (!IsVisible())
			return;

		LinaGX::PresentDesc desc = {
			.swapchain = m_swapchain,
		};

		m_lgx->Present(desc);
	}

} // namespace Lina
