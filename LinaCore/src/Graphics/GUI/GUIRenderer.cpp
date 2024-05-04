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

#include "Core/Graphics/GUI/GUIRenderer.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{

#define MAX_COPY_COMMANDS 50
#define MAX_GUI_VERTICES  20000
#define MAX_GUI_INDICES	  48000

	void GUIRenderer::Create(GfxManager* gfxManager, RenderPass* renderPass, ShaderWriteTargetType writeTargetType, LinaGX::Window* window)
	{
		m_gfxManager		  = gfxManager;
		m_window			  = window;
		m_lgx				  = m_gfxManager->GetLGX();
		m_rm				  = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_shader			  = m_rm->GetResource<Shader>(DEFAULT_SHADER_GUI_SID);
		m_shaderVariantHandle = m_shader->GetGPUHandle(writeTargetType);
		m_defaultGUISampler	  = m_rm->GetResource<TextureSampler>(DEFAULT_SAMPLER_GUI_SID);
		m_textGUISampler	  = m_rm->GetResource<TextureSampler>(DEFAULT_SAMPLER_TEXT_SID);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto&		 data			 = m_pfd[i];
			const String istr			 = TO_STRING(i);
			const String cmdStreamDbg	 = "GUIRenderer: CopyStream" + istr;
			data.copyStream				 = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, cmdStreamDbg.c_str()});
			data.copySemaphore.semaphore = m_lgx->CreateUserSemaphore();
			data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "GUIRenderer: VertexBuffer" + istr);
			data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "GUIRenderer: IndexBuffer" + istr);
			data.guiMaterialBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, MAX_GUI_MATERIALS * sizeof(GPUMaterialGUI), "GUIRenderer: MaterialBuffer" + istr);
			data.guiMaterialBuffer.MemsetMapped(0);
			data.guiIndirectBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndirectBuffer, MAX_GUI_MATERIALS * m_lgx->GetIndexedIndirectCommandSize(), "GUIRenderer IndirectBuffer" + istr);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = renderPass->GetDescriptorSet(i),
				.binding   = 1,
				.buffers   = {data.guiMaterialBuffer.GetGPUResource()},
			});
		}

		m_lvg.GetCallbacks().drawDefault	= BIND(&GUIRenderer::DrawDefault, this, std::placeholders::_1);
		m_lvg.GetCallbacks().drawGradient	= BIND(&GUIRenderer::DrawGradient, this, std::placeholders::_1);
		m_lvg.GetCallbacks().drawTextured	= BIND(&GUIRenderer::DrawTextured, this, std::placeholders::_1);
		m_lvg.GetCallbacks().drawSimpleText = BIND(&GUIRenderer::DrawSimpleText, this, std::placeholders::_1);
		m_lvg.GetCallbacks().drawSDFText	= BIND(&GUIRenderer::DrawSDFText, this, std::placeholders::_1);

		m_widgetManager.Initialize(gfxManager->GetSystem(), window, &m_lvg);
	}

	void GUIRenderer::PreTick()
	{
		m_widgetManager.PreTick();
	}

	void GUIRenderer::Tick(float delta, const Vector2ui& size)
	{
		m_widgetManager.Tick(SystemInfo::GetDeltaTimeF(), size);
	}

	void GUIRenderer::DrawDefault(LinaVG::DrawBuffer* buf)
	{
		auto& req = AddDrawRequest(buf);
	}

	void GUIRenderer::DrawGradient(LinaVG::GradientDrawBuffer* buf)
	{
		auto& req				= AddDrawRequest(buf);
		req.materialData.color1 = buf->m_color.start;
		req.materialData.color2 = buf->m_color.end;

		if (buf->m_color.gradientType == LinaVG::GradientType::Horizontal)
		{
			req.materialData.floatPack1 = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::Vertical)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::Radial)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
			req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::RadialCorner)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
			req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
	}

	void GUIRenderer::DrawTextured(LinaVG::TextureDrawBuffer* buf)
	{
		auto& req				= AddDrawRequest(buf);
		float drawBufferType	= static_cast<float>(buf->m_drawBufferType);
		req.materialData.color1 = buf->m_tint;
		float singleChannel		= 0.0f;

		if (buf->m_textureHandle == GUI_TEXTURE_HUE_HORIZONTAL)
		{
			drawBufferType = 5.0f; // special case :)
		}
		else if (buf->m_textureHandle == GUI_TEXTURE_HUE_VERTICAL)
		{
			drawBufferType = 6.0f; // special case :)
		}
		else if (buf->m_textureHandle == GUI_TEXTURE_COLORWHEEL)
		{
			drawBufferType = 7.0f; // special case :)
		}
		else
		{
			req.materialData.color2.x = static_cast<float>(buf->m_textureHandle - 1);
			req.materialData.color2.y = static_cast<float>(m_defaultGUISampler->GetBindlessIndex());
		}

		if (Math::Equals(req.materialData.color1.w, GUI_IS_SINGLE_CHANNEL, 0.01f))
			singleChannel = 1.0f;

		req.materialData.floatPack1 = Vector4(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y, buf->m_textureUVOffset.x, buf->m_textureUVOffset.y);
		req.materialData.floatPack2 = Vector4(buf->m_isAABuffer, singleChannel, 0.0f, drawBufferType);
	}

	void GUIRenderer::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
	{
		auto& req					  = AddDrawRequest(buf);
		auto  txt					  = m_gfxManager->GetGUIBackend().GetFontTexture(buf->m_textureHandle).texture;
		req.materialData.floatPack2.w = static_cast<float>(buf->m_drawBufferType);
		req.materialData.color2.x	  = static_cast<float>(txt->GetBindlessIndex());
		req.materialData.color2.y	  = static_cast<float>(m_textGUISampler->GetBindlessIndex());
	}

	void GUIRenderer::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf)
	{
		auto& req					= AddDrawRequest(buf);
		auto  txt					= m_gfxManager->GetGUIBackend().GetFontTexture(buf->m_textureHandle).texture;
		req.materialData.color1		= buf->m_outlineColor;
		req.materialData.floatPack1 = Vector4(buf->m_thickness, buf->m_softness, buf->m_outlineThickness, buf->m_outlineSoftness);
		req.materialData.floatPack2 = Vector4(buf->m_flipAlpha ? 1.0f : 0.0f, 0.0f, 0.0f, static_cast<float>(buf->m_drawBufferType));
		req.materialData.color2.x	= static_cast<float>(txt->GetBindlessIndex());
		req.materialData.color2.y	= static_cast<float>(m_textGUISampler->GetBindlessIndex());
	}

	void GUIRenderer::Destroy()
	{
		m_widgetManager.Shutdown();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.semaphore);
			data.guiVertexBuffer.Destroy();
			data.guiIndexBuffer.Destroy();
			data.guiMaterialBuffer.Destroy();
			data.guiIndirectBuffer.Destroy();
		}
	}

	GUIRenderer::DrawRequest& GUIRenderer::AddDrawRequest(LinaVG::DrawBuffer* buf)
	{
		const uint32 frame = m_gfxManager->GetLGX()->GetCurrentFrameIndex();
		auto&		 pfd   = m_pfd[frame];
		m_drawRequests.push_back({});
		auto& req		 = m_drawRequests.back();
		req.indexCount	 = static_cast<uint32>(buf->m_indexBuffer.m_size);
		req.vertexOffset = pfd.vertexCounter;
		req.firstIndex	 = pfd.indexCounter;
		req.clip.pos	 = Vector2ui(buf->clipPosX, buf->clipPosY);
		req.clip.size	 = Vector2ui(buf->clipSizeX, buf->clipSizeY);
		pfd.guiVertexBuffer.BufferData(pfd.vertexCounter * sizeof(LinaVG::Vertex), (uint8*)buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex));
		pfd.guiIndexBuffer.BufferData(pfd.indexCounter * sizeof(LinaVG::Index), (uint8*)buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index));
		pfd.indexCounter += req.indexCount;
		pfd.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
		return req;
	}

	void GUIRenderer::Render(LinaGX::CommandStream* stream, uint32 frameIndex, const Vector2ui& size)
	{
		auto& pfd = m_pfd[frameIndex];

		m_widgetManager.Draw();
		m_lvg.FlushBuffers();
		m_lvg.ResetFrame();
		pfd.vertexCounter = pfd.indexCounter = 0;

		if (m_drawRequests.empty())
			return;

		// Build draw commands
		uint32 drawID		= 0;
		uint32 totalIndices = 0;

		for (const auto& req : m_drawRequests)
		{
			m_lgx->BufferIndexedIndirectCommandData(pfd.guiIndirectBuffer.GetMapped(), static_cast<size_t>(drawID) * m_lgx->GetIndexedIndirectCommandSize(), drawID, req.indexCount, 1, req.firstIndex, req.vertexOffset, 0);
			pfd.guiIndirectBuffer.MarkDirty();
			pfd.guiMaterialBuffer.BufferData(static_cast<size_t>(drawID) * sizeof(GPUMaterialGUI), (uint8*)&req.materialData, sizeof(GPUMaterialGUI));
			drawID++;
			totalIndices += req.indexCount;
		}

		m_drawRequests.clear();

		// Buffer setup.
		uint8 copyExists = 0;
		copyExists |= pfd.guiIndexBuffer.Copy(pfd.copyStream);
		copyExists |= pfd.guiVertexBuffer.Copy(pfd.copyStream);
		copyExists |= pfd.guiMaterialBuffer.Copy(pfd.copyStream);
		copyExists |= pfd.guiIndirectBuffer.Copy(pfd.copyStream);

		if (copyExists != 0)
		{
			pfd.copySemaphore.value++;

			m_lgx->CloseCommandStreams(&pfd.copyStream, 1);

			m_lgx->SubmitCommandStreams({
				.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &pfd.copyStream,
				.streamCount	  = 1,
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = &(pfd.copySemaphore.semaphore),
				.signalValues	  = &pfd.copySemaphore.value,
				.isMultithreaded  = true,
			});
		}
		m_shader->Bind(stream, m_shaderVariantHandle);

		pfd.guiVertexBuffer.BindVertex(stream, static_cast<uint32>(sizeof(LinaVG::Vertex)));
		pfd.guiIndexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);

		LinaGX::CMDDrawIndexedIndirect* draw = stream->AddCommand<LinaGX::CMDDrawIndexedIndirect>();
		draw->count							 = drawID;
		draw->indirectBuffer				 = pfd.guiIndirectBuffer.GetGPUResource();
		draw->indirectBufferOffset			 = 0;
		PROFILER_ADD_DRAWCALL(totalIndices / 3, "Editor GUI Indirect", drawID);
	}

} // namespace Lina
