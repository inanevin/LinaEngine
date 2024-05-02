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
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina
{

#define MAX_COPY_COMMANDS 50
#define MAX_GUI_VERTICES  10000
#define MAX_GUI_INDICES	  24000

	void GUIRenderer::Create(GfxManager* gfxManager, ShaderWriteTargetType writeTargetType, LinaGX::Window* window)
	{
		m_gfxManager		  = gfxManager;
		m_window			  = window;
		m_lgx				  = m_gfxManager->GetLGX();
		auto* rm			  = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_shader			  = rm->GetResource<Shader>(DEFAULT_SHADER_GUI_SID);
		m_shaderVariantHandle = m_shader->GetGPUHandle(writeTargetType);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data					 = m_pfd[i];
			data.copyStream				 = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, "GUIRenderer: Copy Stream"});
			data.copySemaphore.semaphore = m_lgx->CreateUserSemaphore();
			data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "GUIRenderer VertexBuffer");
			data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "GUIRenderer IndexBuffer");
			data.materials.resize(MAX_GUI_MATERIALS);

			for (int32 j = 0; j < MAX_GUI_MATERIALS; j++)
			{
				data.materials[j] = rm->CreateUserResource<Material>("GUIRendererMaterial", 0);
				data.materials[j]->SetShader(DEFAULT_SHADER_GUI_SID);
			}
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
		m_widgetManager.Tick(delta, size);
	}

	void GUIRenderer::DrawDefault(LinaVG::DrawBuffer* buf)
	{
	}

	void GUIRenderer::DrawGradient(LinaVG::GradientDrawBuffer* buf)
	{
	}

	void GUIRenderer::DrawTextured(LinaVG::TextureDrawBuffer* buf)
	{
	}

	void GUIRenderer::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
	{
	}

	void GUIRenderer::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf)
	{
	}

	void GUIRenderer::Destroy()
	{
		m_widgetManager.Shutdown();

		auto* rm = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.semaphore);
			data.guiVertexBuffer.Destroy();
			data.guiIndexBuffer.Destroy();

			for (int32 j = 0; j < MAX_GUI_MATERIALS; j++)
				rm->DestroyUserResource<Material>(data.materials[j]);

			data.materials.clear();
		}
	}

	// void GUIBackend::DrawDefault(m_lvg->DrawBuffer* buf, int threadIndex)
	// {
	// 	return;
	// 	auto& req		   = AddDrawRequest(buf, threadIndex);
	// 	req.hasTextureBind = false;
	// 	req.requestType	   = 0;
	// }
	//
	// void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int threadIndex)
	// {
	// 	auto& req				= AddDrawRequest(buf, threadIndex);
	// 	req.materialData.color1 = buf->m_color.start;
	// 	req.materialData.color2 = buf->m_color.end;
	// 	req.hasTextureBind		= false;
	// 	req.requestType			= 1;
	//
	// 	if (buf->m_color.gradientType == LinaVG::GradientType::Horizontal)
	// 	{
	// 		req.materialData.floatPack1 = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	// 		req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
	// 	}
	// 	else if (buf->m_color.gradientType == LinaVG::GradientType::Vertical)
	// 	{
	// 		req.materialData.floatPack1 = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	// 		req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
	// 	}
	// 	else if (buf->m_color.gradientType == LinaVG::GradientType::Radial)
	// 	{
	// 		req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
	// 		req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
	// 	}
	// 	else if (buf->m_color.gradientType == LinaVG::GradientType::RadialCorner)
	// 	{
	// 		req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
	// 		req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
	// 	}
	// }
	//
	// void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf, int threadIndex)
	// {
	// 	auto& req				= AddDrawRequest(buf, threadIndex);
	// 	float drawBufferType	= static_cast<float>(buf->m_drawBufferType);
	// 	req.requestType			= 2;
	// 	req.materialData.color1 = buf->m_tint;
	// 	float singleChannel		= 0.0f;
	//
	// 	if (buf->m_textureHandle == GUI_TEXTURE_HUE_HORIZONTAL)
	// 	{
	// 		req.hasTextureBind = false;
	// 		drawBufferType	   = 5.0f; // special case :)
	// 	}
	// 	else if (buf->m_textureHandle == GUI_TEXTURE_HUE_VERTICAL)
	// 	{
	// 		req.hasTextureBind = false;
	// 		drawBufferType	   = 6.0f; // special case :)
	// 	}
	// 	else if (buf->m_textureHandle == GUI_TEXTURE_COLORWHEEL)
	// 	{
	// 		req.hasTextureBind = false;
	// 		drawBufferType	   = 7.0f; // special case :)
	// 	}
	// 	else
	// 	{
	// 		req.hasTextureBind = true;
	// 		req.textureHandle  = buf->m_textureHandle;
	// 		req.samplerHandle  = m_guiSampler->GetGPUHandle();
	// 	}
	//
	// 	if (Math::Equals(req.materialData.color1.w, GUI_IS_SINGLE_CHANNEL, 0.01f))
	// 		singleChannel = 1.0f;
	//
	// 	req.materialData.floatPack1 = Vector4(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y, buf->m_textureUVOffset.x, buf->m_textureUVOffset.y);
	// 	req.materialData.floatPack2 = Vector4(buf->m_isAABuffer, singleChannel, 0.0f, drawBufferType);
	// }
	//
	// void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int threadIndex)
	// {
	// 	auto& req					  = AddDrawRequest(buf, threadIndex);
	// 	auto  txt					  = m_fontTextures[buf->m_textureHandle].texture;
	// 	auto  sampler				  = m_resourceManager->GetResource<TextureSampler>(DEFAULT_SAMPLER_TEXT_SID);
	// 	req.materialData.floatPack2.w = static_cast<float>(buf->m_drawBufferType);
	// 	req.hasTextureBind			  = true;
	// 	req.textureHandle			  = txt->GetGPUHandle();
	// 	req.samplerHandle			  = m_textSampler->GetGPUHandle();
	// 	req.requestType				  = 3;
	// }
	//
	// void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int threadIndex)
	// {
	// 	auto& req					= AddDrawRequest(buf, threadIndex);
	// 	auto  txt					= m_fontTextures[buf->m_textureHandle].texture;
	// 	auto  sampler				= m_resourceManager->GetResource<TextureSampler>(DEFAULT_SAMPLER_TEXT_SID);
	// 	req.materialData.color1		= buf->m_outlineColor;
	// 	req.materialData.floatPack1 = Vector4(buf->m_thickness, buf->m_softness, buf->m_outlineThickness, buf->m_outlineSoftness);
	// 	req.materialData.floatPack2 = Vector4(buf->m_flipAlpha ? 1.0f : 0.0f, 0.0f, 0.0f, static_cast<float>(buf->m_drawBufferType));
	// 	req.hasTextureBind			= true;
	// 	req.textureHandle			= txt->GetGPUHandle();
	// 	req.samplerHandle			= m_textSampler->GetGPUHandle();
	// 	req.requestType				= 4;
	// }

	/*
		GUIBackend::DrawRequest& GUIBackend::AddDrawRequest(LinaVG::DrawBuffer* buf, int threadIndex)
		{
			auto& buffers  = m_buffers[threadIndex];
			auto& drawData = m_drawData[threadIndex];

			drawData.drawRequests.push_back(DrawRequest());
			DrawRequest& req = drawData.drawRequests.back();
			req.indexCount	 = static_cast<uint32>(buf->m_indexBuffer.m_size);
			req.vertexOffset = buffers.vertexCounter;
			req.firstIndex	 = buffers.indexCounter;
			req.clip.pos	 = Vector2ui(buf->clipPosX, buf->clipPosY);
			req.clip.size	 = Vector2ui(buf->clipSizeX, buf->clipSizeY);

			buffers.vertexBuffer->BufferData(buffers.vertexCounter * sizeof(LinaVG::Vertex), (uint8*)buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex));
			buffers.indexBuffer->BufferData(buffers.indexCounter * sizeof(LinaVG::Index), (uint8*)buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index));

			buffers.indexCounter += req.indexCount;
			buffers.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
			return req;
		}
	*/

	void GUIRenderer::Render(LinaGX::CommandStream* stream, uint32 frameIndex, const Vector2ui& size)
	{
		// Will flush buffers and fill draw requests.
		// LinaVG::Render(threadIndex);

		// const auto& drawRequests = m_guiBackend->GetDrawData(threadIndex).drawRequests;
		// auto&		pfd			 = m_pfd[frameIndex];
		//
		// // Shader.
		// m_shader->Bind(stream, m_shaderVariantHandle);
		//
		// // Buffer setup.
		// bool copyExists = false;
		//
		// if (pfd.guiIndexBuffer.Copy(pfd.copyStream))
		// 	copyExists = true;
		//
		// if (pfd.guiVertexBuffer.Copy(pfd.copyStream))
		// 	copyExists = true;
		//
		// if (copyExists)
		// {
		// 	uint64 val = pfd.copySemaphore.value;
		// 	pfd.copySemaphore.value++;
		//
		// 	m_lgx->CloseCommandStreams(&pfd.copyStream, 1);
		//
		// 	m_lgx->SubmitCommandStreams({
		// 		.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
		// 		.streams		  = &pfd.copyStream,
		// 		.streamCount	  = 1,
		// 		.useSignal		  = true,
		// 		.signalCount	  = 1,
		// 		.signalSemaphores = &(pfd.copySemaphore.semaphore),
		// 		.signalValues	  = &val,
		// 		.isMultithreaded  = true,
		// 	});
		// }
		//
		// // Buffer bind.
		// pfd.guiVertexBuffer.BindVertex(stream, static_cast<uint32>(sizeof(LinaVG::Vertex)));
		// pfd.guiIndexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);
		// LINA_ASSERT(drawRequests.size() < MAX_GUI_MATERIALS, "Requests exceed materials size!");
		//
		// Rectui scissorsRect = Rectui(0, 0, size.x, size.y);
		// uint32 reqIndex		= 0;
		//
		// for (const auto& req : drawRequests)
		// {
		// 	Rectui currentRect = Rectui(0, 0, size.x, size.y);
		//
		// 	if (req.clip.size.x != 0 || req.clip.size.y != 0)
		// 	{
		// 		currentRect.pos	 = req.clip.pos;
		// 		currentRect.size = req.clip.size;
		// 	}
		//
		// 	if (!currentRect.Equals(scissorsRect))
		// 	{
		// 		scissorsRect					 = currentRect;
		// 		LinaGX::CMDSetScissors* scissors = stream->AddCommand<LinaGX::CMDSetScissors>();
		// 		scissors->x						 = scissorsRect.pos.x;
		// 		scissors->y						 = scissorsRect.pos.y;
		// 		scissors->width					 = scissorsRect.size.x;
		// 		scissors->height				 = scissorsRect.size.y;
		// 	}
		//
		// 	Material* mat = pfd.materials[reqIndex];
		//
		// 	// Set material data.
		// 	mat->SetBuffer(0, 0, frameIndex, 0, (uint8*)&req.materialData, sizeof(GPUMaterialGUI));
		// 	if (req.hasTextureBind)
		// 	{
		// 		mat->SetSampler(1, 0, req.samplerHandle);
		// 		mat->SetTexture(2, 0, req.textureHandle);
		// 	}
		//
		// 	// Bind.
		// 	mat->Bind(stream, frameIndex, LinaGX::DescriptorSetsLayoutSource::CustomLayout);
		//
		// 	// Draw command.
		// 	LinaGX::CMDDrawIndexedInstanced* draw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
		// 	draw->indexCountPerInstance			  = req.indexCount;
		// 	draw->instanceCount					  = 1;
		// 	draw->startInstanceLocation			  = 0;
		// 	draw->startIndexLocation			  = req.firstIndex;
		// 	draw->baseVertexLocation			  = req.vertexOffset;
		//
		// 	reqIndex++;
		//
		// 	PROFILER_ADD_DRAWCALL(req.indexCount / 3, "Editor", req.requestType);
		// }
	}

} // namespace Lina
