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

#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Core/GUIRenderer.hpp"
#include "Graphics/Core/LGXWrapper.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Core/GPUBuffer.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Resources/Core/CommonResources.hpp"
#include "System/ISystem.hpp"
#include "Platform/LinaGXIncl.hpp"

namespace Lina
{
	GUIBackend::GUIBackend(GfxManager* man)
	{
		m_gfxManager	  = man;
		m_resourceManager = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_lgx			  = m_gfxManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper)->GetLGX();
	}

	bool Lina::GUIBackend::Initialize()
	{
		return true;
	}

	void GUIBackend::Terminate()
	{
		for (auto [sid, txt] : m_fontTextures)
			delete txt;

		m_fontTextures.clear();
	}

	void GUIBackend::StartFrame(int threadCount)
	{
		m_guiRenderData.resize(threadCount);
	}

	void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread)
	{
		auto& req				= AddDrawRequest(buf, m_guiRenderData[thread]);
		req.materialData.color1 = FL4(buf->m_color.start);
		req.materialData.color2 = FL4(buf->m_color.end);

		if (buf->m_color.gradientType == LinaVG::GradientType::Horizontal)
		{
			req.materialData.floatPack1 = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, 0.0f);
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::Vertical)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, 0.0f);
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::Radial)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, 0.0f);
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::RadialCorner)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
			req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, 0.0f);
		}
	}

	void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread)
	{
		auto& req							  = AddDrawRequest(buf, m_guiRenderData[thread]);
		auto  txt							  = m_resourceManager->GetResource<Texture>(buf->m_textureHandle);
		auto  sampler						  = m_resourceManager->GetResource<TextureSampler>(txt->GetSamplerSID());
		req.materialData.floatPack1			  = Vector4(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y, buf->m_textureUVOffset.x, buf->m_textureUVOffset.y);
		req.materialData.floatPack2			  = Vector4(buf->m_isAABuffer, 0.0f, 0.0f, 0.0f);
		req.materialData.diffuse.textureIndex = txt->GetGPUHandle();
		req.materialData.diffuse.samplerIndex = sampler->GetGPUHandle();
	}

	void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf, int thread)
	{
		auto& req = AddDrawRequest(buf, m_guiRenderData[thread]);
	}

	void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread)
	{
		auto& req							  = AddDrawRequest(buf, m_guiRenderData[thread]);
		auto  txt							  = m_resourceManager->GetResource<Texture>(buf->m_textureHandle);
		auto  sampler						  = m_resourceManager->GetResource<TextureSampler>(txt->GetSamplerSID());
		req.materialData.diffuse.textureIndex = txt->GetGPUHandle();
		req.materialData.diffuse.samplerIndex = sampler->GetGPUHandle();
	}

	void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread)
	{
		auto& req							  = AddDrawRequest(buf, m_guiRenderData[thread]);
		auto  txt							  = m_resourceManager->GetResource<Texture>(buf->m_textureHandle);
		auto  sampler						  = m_resourceManager->GetResource<TextureSampler>(txt->GetSamplerSID());
		req.materialData.color1				  = FL4(buf->m_outlineColor);
		req.materialData.floatPack1			  = Vector4(buf->m_thickness, buf->m_softness, buf->m_outlineThickness, buf->m_outlineThickness != 0.0f);
		req.materialData.floatPack2			  = Vector4(buf->m_flipAlpha ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);
		req.materialData.diffuse.textureIndex = txt->GetGPUHandle();
		req.materialData.diffuse.samplerIndex = sampler->GetGPUHandle();
	}

	void GUIBackend::Render(int threadID)
	{
		auto& data = m_guiRenderData[threadID];

		// Update set 1 scene data
		{
			GPUGUISceneData sceneData;
			sceneData.projection = GetProjectionFromSize(data.size);
			MEMCPY(data.sceneDataMapping, &sceneData, sizeof(GPUGUISceneData));
		}

		// Update material data.
		{
			Vector<GPUGUIMaterialData> materials;
			materials.resize(data.drawRequests.size());

			uint32 i = 0;
			for (const auto& req : data.drawRequests)
				materials[i] = req.materialData;

			data.materialBuffer->BufferData(0, materials.data(), materials.size() * sizeof(GPUGUIMaterialData));
		}

		bool copyExists = false;

		// Check need for copy operation
		{
			if (data.materialBuffer->Copy(data.copyStream))
				copyExists = true;

			if (data.indexBuffer->Copy(data.copyStream))
				copyExists = true;

			if (data.vertexBuffer->Copy(data.copyStream))
				copyExists = true;

			if (copyExists)
			{
				uint64 val = *data.copySemaphoreValue;
				val++;
				*data.copySemaphoreValue = val;

				LinaGX::SubmitDesc desc = {
					.queue			  = LinaGX::QueueType::Transfer,
					.streams		  = &data.copyStream,
					.streamCount	  = 1,
					.useSignal		  = true,
					.signalCount	  = 1,
					.signalSemaphores = &data.copySemaphore,
					.signalValues	  = &val,
				};

				m_lgx->SubmitCommandStreams(desc);
			}
		}

		// Buffers
		{

			LinaGX::CMDBindVertexBuffers* vtx = data.gfxStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
			vtx->extension					  = nullptr;
			vtx->offset						  = 0;
			vtx->slot						  = 0;
			vtx->vertexSize					  = sizeof(LinaVG::Vertex);
			vtx->resource					  = data.vertexBuffer->GetGPUResource();

			LinaGX::CMDBindIndexBuffers* index = data.gfxStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
			index->extension				   = nullptr;
			index->indexFormat				   = LinaGX::IndexType::Uint16;
			index->offset					   = 0;
			index->resource					   = data.indexBuffer->GetGPUResource();
		}

		// Descriptors
		{
			LinaGX::CMDBindDescriptorSets* bind = data.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
			bind->extension						= nullptr;
			bind->descriptorSetHandles			= data.gfxStream->EmplaceAuxMemory<uint16>(m_gfxManager->GetCurrentDescriptorSet0GlobalData(), data.descriptorSet1, data.descriptorSet2);
			bind->firstSet						= 0;
			bind->setCount						= 3;
			bind->isCompute						= false;
		}

		// Draw
		{
			uint32 i = 0;
			for (const auto& req : data.drawRequests)
			{
				// Material id.
				LinaGX::CMDBindConstants* constants = data.gfxStream->AddCommand<LinaGX::CMDBindConstants>();
				constants->extension				= nullptr;
				constants->offset					= 0;
				constants->stages					= data.gfxStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Fragment);
				constants->stagesSize				= 1;
				constants->size						= sizeof(GPUGUIConstants);
				constants->data						= data.gfxStream->EmplaceAuxMemory<GPUGUIConstants>(req.constants);

				// Draw command.
				LinaGX::CMDDrawIndexedInstanced* draw = data.gfxStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
				draw->extension						  = nullptr;
				draw->indexCountPerInstance			  = req.indexCount;
				draw->instanceCount					  = 1;
				draw->startInstanceLocation			  = 0;
				draw->startIndexLocation			  = req.firstIndex;
				draw->baseVertexLocation			  = req.vertexOffset;

				i++;
			}
		}
	}

	void GUIBackend::EndFrame()
	{
	}

	void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
	{
	}

	void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
	{
		m_boundFontTexture = texture;
	}

	LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
	{
		return 0;
	}

	void GUIBackend::Prepare(int threadID, const GUIRenderData& data)
	{
		m_guiRenderData[threadID] = data;
	}

	GUIBackend::DrawRequest& GUIBackend::AddDrawRequest(LinaVG::DrawBuffer* buf, GUIRenderData& data)
	{
		data.drawRequests.push_back(DrawRequest());
		DrawRequest& req		 = data.drawRequests.back();
		req.indexCount			 = static_cast<uint32>(buf->m_indexBuffer.m_size);
		req.vertexOffset		 = data.vertexCounter;
		req.firstIndex			 = data.indexCounter;
		req.clip.pos			 = Vector2ui(buf->clipPosX, buf->clipPosY);
		req.clip.size			 = Vector2ui(buf->clipSizeX, buf->clipSizeY);
		req.constants.bufferType = static_cast<uint32>(buf->m_drawBufferType);
		req.constants.materialID = static_cast<uint32>(data.drawRequests.size() - 1);

		data.vertexBuffer->BufferData(data.vertexCounter * sizeof(LinaVG::Vertex), buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex));
		data.indexBuffer->BufferData(data.indexCounter * sizeof(LinaVG::Index), buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index));
		data.indexCounter += req.indexCount;
		data.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
		return req;
	}

	Matrix4 GUIBackend::GetProjectionFromSize(const Vector2ui& size)
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
