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

#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Graphics/Data/Vertex.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina::Editor
{
#define MAX_GFX_COMMANDS  1000
#define MAX_COPY_COMMANDS 1000

#define MAX_GUI_VERTICES 120000
#define MAX_GUI_INDICES	 140000

#define VSYNC_DX LinaGX::DXVsync::None
#define VSYNC_VK LinaGX::VKVsync::None

	SurfaceRenderer::SurfaceRenderer(Editor* editor, LinaGX::Window* window, const Color& clearColor) : m_window(window)
	{
		m_editor			= editor;
		m_resourceManagerV2 = &m_editor->GetApp()->GetResourceManager();
		m_lgx				= Application::GetLGX();
		m_size				= m_window->GetSize();
        m_rtSize = Vector2ui(static_cast<float>(m_window->GetSize().x) * m_window->GetDPIScale(), static_cast<float>(m_window->GetSize().y) * m_window->GetDPIScale());
        m_size = m_rtSize;
        // m_rtSize = m_size;
		m_guiDefault		= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_DEFAULT_ID);
		m_guiColorWheel		= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_COLOR_WHEEL_ID);
		m_guiHue			= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_HUE_DISPLAY_ID);
		m_guiText			= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_TEXT_ID);
		m_guiSDF			= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_SDF_TEXT_ID);
		m_guiGlitch			= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_GLITCH_ID);
		m_guiDisplayTarget	= m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_DISPLAYTARGET_ID);
        m_guiSwapchain = m_editor->GetApp()->GetResourceManager().GetResource<Shader>(EDITOR_SHADER_GUI_SWAPCHAIN_ID);
        m_swapchainTextureSampler = m_resourceManagerV2->CreateResource<TextureSampler>(m_resourceManagerV2->ConsumeResourceID());
        m_clearColor = clearColor;
        
        LinaGX::SamplerDesc samplerData = {};
        samplerData.minFilter            = LinaGX::Filter::Nearest;
        samplerData.magFilter            = LinaGX::Filter::Nearest;
        samplerData.mode                = LinaGX::SamplerAddressMode::ClampToEdge;
        samplerData.anisotropy            = 6;
        samplerData.borderColor            = LinaGX::BorderColor::WhiteOpaque;
        samplerData.mipLodBias            = 0.0f;
        samplerData.minLod                = 0.0f;
        samplerData.maxLod                = 0;
        m_swapchainTextureSampler->GenerateHW(samplerData);
        
		const LinaGX::VSyncStyle vsync = {
			.vulkanVsync = VSYNC_VK,
			.dx12Vsync	 = VSYNC_DX,
		};

		// Swapchain
		const auto monitorSize = window->GetMonitorSize();
		const auto windowSize  = window->GetSize();
		m_swapchain			   = m_lgx->CreateSwapchain({
					   .format		 = SystemInfo::GetSwapchainFormat(),
					   .x			 = 0,
					   .y			 = 0,
					   .width		 = windowSize.x,
					   .height		 = windowSize.y,
					   .window		 = window->GetWindowHandle(),
					   .osHandle	 = window->GetOSHandle(),
					   .isFullscreen = windowSize.x == monitorSize.x && windowSize.y == monitorSize.y,
					   .vsyncStyle	 = vsync,
                       .scalingFactor = window->GetDPIScale(),
		   });
        
        const LinaGX::TextureDesc rtDesc = {
            .format       = SystemInfo::GetLDRFormat(),
            .flags       = LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
            .width       = m_rtSize.x,
            .height       = m_rtSize.y,
            .samples   = 1,
            .debugName = "Surface Renderer Target",
        };

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data	   = m_pfd[i];
			data.gfxStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "SurfaceRenderer: Gfx Stream"});
			

			const String cmdStreamDbg = "SurfaceRenderer: CopyStream" + TO_STRING(i);
			data.copyStream			  = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, cmdStreamDbg.c_str()});
			data.copySemaphore		  = SemaphoreData(m_lgx->CreateUserSemaphore());

			const String istr = TO_STRING(i);
			data.guiVertexBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "SurfaceRenderer: VertexBuffer" + istr);
			data.guiIndexBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "SurfaceRenderer: IndexBuffer" + istr);
            
            data.renderTarget = m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID());
            data.renderTarget->GenerateHWFromDesc(rtDesc);
            
            m_swapchainPass.SetColorAttachment(i, 0, {.clearColor = {clearColor.x, clearColor.y, clearColor.z, clearColor.w}, .texture = static_cast<uint32>(m_swapchain), .isSwapchain = true});
            
            m_guiPass.SetColorAttachment(i, 0, {.clearColor = {clearColor.x, clearColor.y, clearColor.z, clearColor.w}, .texture = data.renderTarget->GetGPUHandle(), .isSwapchain = false});
		}

		m_cpuDraw.materialBuffer = {new uint8[10000], 10000};
		m_gpuDraw.materialBuffer = {new uint8[10000], 10000};

		// RP
		m_guiPass.Create(EditorGfxHelpers::GetGUIPassDescription());
        m_guiPass.SetSize(m_rtSize);

        m_swapchainPass.Create(EditorGfxHelpers::GetSwapchainPassDescription());
        m_swapchainPass.SetSize(m_size);
        
		m_widgetManager.Initialize(&m_editor->GetApp()->GetResourceManager(), m_window, &m_lvgDrawer);
		m_lvgDrawer.GetCallbacks().draw = BIND(&SurfaceRenderer::DrawDefault, this, std::placeholders::_1);
        
        m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		delete[] m_cpuDraw.materialBuffer.data();
		delete[] m_gpuDraw.materialBuffer.data();

		m_widgetManager.Shutdown();
        m_swapchainPass.Destroy();
		m_guiPass.Destroy();
        m_swapchainTextureSampler->DestroyHW();
        m_resourceManagerV2->DestroyResource(m_swapchainTextureSampler);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());
			data.guiVertexBuffer.Destroy();
			data.guiIndexBuffer.Destroy();
            data.renderTarget->DestroyHW();
            m_resourceManagerV2->DestroyResource(data.renderTarget);
		}

		m_lgx->DestroySwapchain(m_swapchain);
        m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void SurfaceRenderer::OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize)
	{
		if (window != m_window)
			return;

		const LinaGX::LGXVector2ui monitorSize = m_window->GetMonitorSize();

		const LinaGX::VSyncStyle vsync = {
			.vulkanVsync = VSYNC_VK,
			.dx12Vsync	 = VSYNC_DX,
		};

		LinaGX::SwapchainRecreateDesc desc = {
			.swapchain	  = m_swapchain,
			.width		  = newSize.x,
			.height		  = newSize.y,
			.isFullscreen = newSize.x == monitorSize.x && newSize.y == monitorSize.y,
			.vsyncStyle	  = vsync,
            .scalingFactor = m_window->GetDPIScale(),
		};

		m_lgx->RecreateSwapchain(desc);
		m_size = newSize;
        m_rtSize = Vector2ui(static_cast<float>(m_window->GetSize().x) * m_window->GetDPIScale(), static_cast<float>(m_window->GetSize().y) * m_window->GetDPIScale());
        // m_rtSize = m_size;
        m_size = m_rtSize;
        m_guiPass.SetSize(m_rtSize);
        m_swapchainPass.SetSize(m_size);

        const LinaGX::TextureDesc rtDesc = {
            .format       = SystemInfo::GetLDRFormat(),
            .flags       = LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
            .width       = m_rtSize.x,
            .height       = m_rtSize.y,
            .samples   = 1,
            .debugName = "Surface Renderer Target",
        };

        for(uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            PerFrameData& data = m_pfd[i];
            data.renderTarget->DestroyHW();
            data.renderTarget->GenerateHWFromDesc(rtDesc);
            
            m_guiPass.SetColorAttachment(i, 0, {.clearColor = {m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w}, .texture = data.renderTarget->GetGPUHandle(), .isSwapchain = false});
        }
        
        m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	bool SurfaceRenderer::CheckVisibility()
	{
		auto ws		= m_window->GetSize();
		m_isVisible = m_window->GetIsVisible() && ws.x != 0 && ws.y != 0 && !m_window->GetIsMinimized();
		m_lgx->SetSwapchainActive(m_swapchain, m_isVisible);
		return m_isVisible;
	}

	void SurfaceRenderer::PreTick()
	{
		m_widgetManager.PreTick();
	}

	void SurfaceRenderer::Tick(float delta)
	{
		m_widgetManager.Tick(delta, m_rtSize);
		m_widgetManager.Draw();

		LinaVG::StyleOptions opts;
		opts.isFilled  = false;
		opts.thickness = Theme::GetDef().baseBorderThickness * 2;
		opts.color	   = Theme::GetDef().background0.AsLVG4();

        const Vector2 sz = m_widgetManager.GetSize() - Vector2(2, 2);
		m_lvgDrawer.DrawRect(LinaVG::Vec2(2, 2), sz.AsLVG(), opts, 0.0f, FOREGROUND_DRAW_ORDER);
        
		m_lvgDrawer.FlushBuffers();
		m_lvgDrawer.ResetFrame();
        
        RenderPass::InstancedDraw draw = {
            .shaderHandle  = m_guiSwapchain->GetGPUHandle(),
            .vertexCount      = 3,
            .instanceCount = 1,
        };
        
        m_swapchainPass.AddDrawCall(draw);
	}

	void SurfaceRenderer::DrawDefault(LinaVG::DrawBuffer* buf)
	{

		const uint32 frameIndex = m_lgx->GetCurrentFrameIndex();
		auto&		 pfd		= m_pfd[frameIndex];

		// Update vertex / index buffers.
		const uint32 vtxCount = static_cast<uint32>(buf->vertexBuffer.m_size);
		const uint32 idxCount = static_cast<uint32>(buf->indexBuffer.m_size);

		RenderPass::InstancedDraw drawCall = {
			.vertexBuffers =
				{
					&m_pfd[0].guiVertexBuffer,
					&m_pfd[1].guiVertexBuffer,
				},
			.indexBuffers =
				{
					&m_pfd[0].guiIndexBuffer,
					&m_pfd[1].guiIndexBuffer,
				},
			.vertexSize	   = sizeof(LinaVG::Vertex),
			.baseVertex	   = static_cast<uint32>(m_cpuDraw.guiVertices.size()),
			.vertexCount   = vtxCount,
			.baseIndex	   = static_cast<uint32>(m_cpuDraw.guiIndices.size()),
			.indexCount	   = idxCount,
			.instanceCount = 1,
			.baseInstance  = 0,
			.pushConstant  = static_cast<uint32>(m_cpuDraw.materialBufferCounter) / static_cast<uint32>(sizeof(uint32)),
			.clip		   = Recti(buf->clip.x, buf->clip.y, buf->clip.z, buf->clip.w),
			.useScissors   = true,
		};

		GUIUserData* guiUserData = nullptr;
		if (buf->userData != nullptr)
		{
			guiUserData = static_cast<GUIUserData*>(buf->userData);

			if (guiUserData->specialType == GUISpecialType::DisplayTarget && buf->textureHandle == nullptr)
				guiUserData = nullptr;
		}

		ResourceManagerV2& rm		  = m_editor->GetApp()->GetResourceManager();
		GUIBackend&		   guiBackend = m_editor->GetApp()->GetGUIBackend();

		if (buf->shapeType == LinaVG::DrawBufferShapeType::Shape || buf->shapeType == LinaVG::DrawBufferShapeType::AA)
		{
			if (guiUserData)
			{
				if (guiUserData->specialType == GUISpecialType::None)
				{
					Texture* texture = buf->textureHandle == nullptr ? nullptr : static_cast<Texture*>(buf->textureHandle);

					GPUMaterialGUIDefault material = {
						.uvTilingAndOffset = buf->textureUV,
						.hasTexture		   = texture != nullptr,
						.displayChannels   = 0,
						.displayLod		   = 0,
						.singleChannel	   = texture == nullptr ? (uint32)0 : (uint32)(texture->GetMeta().format == LinaGX::Format::R8_UNORM),
						.diffuse =
							{
								.textureIndex = texture != nullptr ? texture->GetBindlessIndex() : 0,
								.samplerIndex = texture != nullptr ? m_editor->GetEditorRenderer().GetGUISampler()->GetBindlessIndex() : 0,
							},
					};

					material.displayLod		 = guiUserData->mipLevel;
					material.displayChannels = static_cast<uint32>(guiUserData->displayChannels);

					if (guiUserData->sampler != 0)
						material.diffuse.samplerIndex = rm.GetResource<TextureSampler>(guiUserData->sampler)->GetBindlessIndex();

					drawCall.shaderHandle = m_guiDefault->GetGPUHandle();

					MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIDefault));
					m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIDefault);
				}
				else if (guiUserData->specialType == GUISpecialType::Glitch)
				{
					drawCall.shaderHandle		  = m_guiGlitch->GetGPUHandle();
					Texture*			 texture  = buf->textureHandle == nullptr ? nullptr : static_cast<Texture*>(buf->textureHandle);
					GPUMaterialGUIGlitch material = {
						.uvTilingAndOffset = buf->textureUV,
						.diffuse =
							{
								.textureIndex = texture != nullptr ? texture->GetBindlessIndex() : 0,
								.samplerIndex = texture != nullptr ? m_editor->GetEditorRenderer().GetGUISampler()->GetBindlessIndex() : 0,
							},
					};

					MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIGlitch));
					m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIGlitch);
				}
				else if (guiUserData->specialType == GUISpecialType::DisplayTarget)
				{
					drawCall.shaderHandle = m_guiDisplayTarget->GetGPUHandle();

					Texture* texture = buf->textureHandle == nullptr ? nullptr : static_cast<Texture*>(buf->textureHandle);

					GPUMaterialGUIDisplayTarget material = {
						.uvTilingAndOffset = buf->textureUV,
						.diffuse =
							{
								.textureIndex = texture != nullptr ? texture->GetBindlessIndex() : 0,
								.samplerIndex = texture != nullptr ? m_editor->GetEditorRenderer().GetGUISampler()->GetBindlessIndex() : 0,
							},
					};

					MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIDisplayTarget));
					m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIDisplayTarget);
				}
				else if (guiUserData->specialType == GUISpecialType::ColorWheel)
				{
					drawCall.shaderHandle			  = m_guiColorWheel->GetGPUHandle();
					GPUMaterialGUIColorWheel material = {
						.wheelRadius	= 0.5f,
						.edgeSmoothness = 0.005f,
					};

					MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIColorWheel));
					m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIColorWheel);
				}
				else if (guiUserData->specialType == GUISpecialType::VerticalHue || guiUserData->specialType == GUISpecialType::HorizontalHue)
				{
					drawCall.shaderHandle				= m_guiHue->GetGPUHandle();
					const bool				 isVertical = guiUserData->specialType == GUISpecialType::VerticalHue;
					GPUMaterialGUIHueDisplay material	= {
						  .uvContribution = Vector2(isVertical == false ? 1.0f : 0.0f, isVertical ? 1.0f : 0.0f),
					  };

					MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIHueDisplay));
					m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIHueDisplay);
				}
			}
			else
			{
				drawCall.shaderHandle = m_guiDefault->GetGPUHandle();
				Texture* texture	  = buf->textureHandle == nullptr ? nullptr : static_cast<Texture*>(buf->textureHandle);

				GPUMaterialGUIDefault material = {
					.uvTilingAndOffset = buf->textureUV,
					.hasTexture		   = texture != nullptr,
					.displayChannels   = 0,
					.displayLod		   = 0,
					.singleChannel	   = texture == nullptr ? (uint32)0 : (uint32)(texture->GetMeta().format == LinaGX::Format::R8_UNORM),
					.diffuse =
						{
							.textureIndex = texture != nullptr ? texture->GetBindlessIndex() : 0,
							.samplerIndex = texture != nullptr ? m_editor->GetEditorRenderer().GetGUISampler()->GetBindlessIndex() : 0,
						},
				};
				MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIDefault));
				m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIDefault);
			}
		}
		else if (buf->shapeType == LinaVG::DrawBufferShapeType::Text)
		{
			drawCall.shaderHandle = m_guiText->GetGPUHandle();

			LinaVG::Atlas*	   atlas	= static_cast<LinaVG::Atlas*>(buf->textureHandle);
			Texture*		   texture	= guiBackend.GetFontTexture(atlas).texture;
			GPUMaterialGUIText material = {
				.diffuse =
					{
						.textureIndex = texture->GetBindlessIndex(),
						.samplerIndex = m_editor->GetEditorRenderer().GetGUITextSampler()->GetBindlessIndex(),
					},
			};

			MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUIText));
			m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUIText);
		}
		else if (buf->shapeType == LinaVG::DrawBufferShapeType::SDFText)
		{
			drawCall.shaderHandle = m_guiSDF->GetGPUHandle();

			Texture*			  texture  = guiBackend.GetFontTexture(static_cast<LinaVG::Atlas*>(buf->textureHandle)).texture;
			GPUMaterialGUISDFText material = {
				.diffuse =
					{
						.textureIndex = texture->GetBindlessIndex(),
						.samplerIndex = m_editor->GetEditorRenderer().GetGUITextSampler()->GetBindlessIndex(),
					},
				.outlineColor	  = Vector4(0, 0, 0, 0),
				.thickness		  = 0.475f,
				.softness		  = 0.07f,
				.outlineThickness = 0.0f,
				.outlineSoftness  = 0.02f,
			};

			if (guiUserData)
			{
				material.thickness		  = guiUserData->sdfThickness;
				material.softness		  = guiUserData->sdfSoftness;
				material.outlineThickness = guiUserData->sdfOutlineThickness;
				material.outlineSoftness  = guiUserData->sdfOutlineSoftness;
				material.outlineColor	  = guiUserData->sdfOutlineColor;
			}

			MEMCPY(m_cpuDraw.materialBuffer.data() + m_cpuDraw.materialBufferCounter, (uint8*)&material, sizeof(GPUMaterialGUISDFText));
			m_cpuDraw.materialBufferCounter += sizeof(GPUMaterialGUISDFText);
		}

		m_guiPass.AddDrawCall(drawCall);

		m_cpuDraw.guiVertices.insert(m_cpuDraw.guiVertices.end(), buf->vertexBuffer.m_data, buf->vertexBuffer.m_data + vtxCount);
		m_cpuDraw.guiIndices.insert(m_cpuDraw.guiIndices.end(), buf->indexBuffer.m_data, buf->indexBuffer.m_data + idxCount);

		PROFILER_ADD_DRAWCALL(1);
		PROFILER_ADD_VERTICESINDICES(vtxCount, idxCount);
		PROFILER_ADD_TRIS(idxCount / 3);
	}

	void SurfaceRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

        GPUDataEditorGUIView view = {.proj = GfxHelpers::GetProjectionFromSize(m_rtSize)};
		m_guiPass.GetBuffer(frameIndex, "GUIViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataEditorGUIView));
        
        GPUDataEditorSwapchainPass swapchainPass = {
            .textureIndex = m_pfd[frameIndex].renderTarget->GetBindlessIndex(),
            .samplerIndex = m_swapchainTextureSampler->GetBindlessIndex(),
        };
        m_swapchainPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&swapchainPass, sizeof(GPUDataEditorSwapchainPass));

		currentFrame.guiIndexBuffer.BufferData(0, (uint8*)m_gpuDraw.guiIndices.data(), m_gpuDraw.guiIndices.size() * sizeof(LinaVG::Index));
		currentFrame.guiVertexBuffer.BufferData(0, (uint8*)m_gpuDraw.guiVertices.data(), m_gpuDraw.guiVertices.size() * sizeof(LinaVG::Vertex));
		Buffer& materialBuffer = m_guiPass.GetBuffer(frameIndex, "GUIMaterials"_hs);
		materialBuffer.BufferData(0, m_gpuDraw.materialBuffer.data(), m_gpuDraw.materialBufferCounter);

		m_uploadQueue.AddBufferRequest(&currentFrame.guiIndexBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.guiVertexBuffer);
		m_guiPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
        m_swapchainPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
            
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

	void SurfaceRenderer::SyncRender()
	{
		m_gpuDraw.guiIndices			= m_cpuDraw.guiIndices;
		m_gpuDraw.guiVertices			= m_cpuDraw.guiVertices;
		m_gpuDraw.materialBufferCounter = m_cpuDraw.materialBufferCounter;
		MEMCPY(m_gpuDraw.materialBuffer.data(), m_cpuDraw.materialBuffer.data(), m_cpuDraw.materialBufferCounter);

		m_cpuDraw.materialBufferCounter = 0;
		m_cpuDraw.guiIndices.resize(0);
		m_cpuDraw.guiVertices.resize(0);

		m_guiPass.SyncRender();
        m_swapchainPass.SyncRender();
	}

	LinaGX::CommandStream* SurfaceRenderer::Render(uint32 frameIndex)
	{
		LINA_ASSERT(m_isVisible, "");

		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.ResetModified();

		UpdateBuffers(frameIndex);

		LinaGX::CMDBindDescriptorSets* bindGlobal = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bindGlobal->descriptorSetHandles		  = currentFrame.gfxStream->EmplaceAuxMemory<uint16>(m_editor->GetApp()->GetGfxContext().GetDescriptorSetGlobal(frameIndex));
		bindGlobal->firstSet					  = 0;
		bindGlobal->setCount					  = 1;
		bindGlobal->layoutSource				  = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bindGlobal->customLayout				  = m_editor->GetApp()->GetGfxContext().GetPipelineLayoutGlobal();

		// Barrier to Color Attachment
		LinaGX::CMDBarrier* barrierToColor	= currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToColor->srcStageFlags		= LinaGX::PSF_TopOfPipe;
		barrierToColor->dstStageFlags		= LinaGX::PSF_ColorAttachment;
		barrierToColor->textureBarrierCount = 2;
		barrierToColor->textureBarriers		= currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
		barrierToColor->textureBarriers[0]	= GfxHelpers::GetTextureBarrierPresent2Color(static_cast<uint32>(m_swapchain), true);
        barrierToColor->textureBarriers[1] = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.renderTarget->GetGPUHandle());

		DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Surface Color Target");
        m_guiPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_editor->GetEditorRenderer().GetPipelineLayoutGUI(), 1);
		m_guiPass.Begin(currentFrame.gfxStream, frameIndex);
		m_guiPass.Render(frameIndex, currentFrame.gfxStream);
		m_guiPass.End(currentFrame.gfxStream);
		DEBUG_LABEL_END(currentFrame.gfxStream);
        
        DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Surface Swapchain");
        m_swapchainPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_editor->GetEditorRenderer().GetPipelineLayoutSwapchain(), 1);
        m_swapchainPass.Begin(currentFrame.gfxStream, frameIndex);
        m_swapchainPass.Render(frameIndex, currentFrame.gfxStream);
        m_swapchainPass.End(currentFrame.gfxStream);
        DEBUG_LABEL_END(currentFrame.gfxStream);

		// Barrier to Present
		LinaGX::CMDBarrier* barrierToPresent  = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToPresent->srcStageFlags		  = LinaGX::PSF_ColorAttachment;
		barrierToPresent->dstStageFlags		  = LinaGX::PSF_BottomOfPipe;
		barrierToPresent->textureBarrierCount = 2;
		barrierToPresent->textureBarriers	  = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
        barrierToPresent->textureBarriers[0]  = GfxHelpers::GetTextureBarrierColor2Present(static_cast<uint32>(m_swapchain), true);
        barrierToPresent->textureBarriers[1]  = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.renderTarget->GetGPUHandle());

		// Close
		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);

		return currentFrame.gfxStream;
	}

} // namespace Lina::Editor
