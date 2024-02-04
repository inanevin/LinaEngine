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
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Core/GPUBuffer.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Resources/Core/CommonResources.hpp"
#include "System/System.hpp"
#include "Platform/LinaGXIncl.hpp"

namespace Lina
{
	GUIBackend::GUIBackend(GfxManager* man)
	{
		m_gfxManager	  = man;
		m_resourceManager = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	bool Lina::GUIBackend::Initialize()
	{
		return true;
	}

	void GUIBackend::Terminate()
	{
		for (const auto& ft : m_fontTextures)
		{
			delete ft.texture;
			delete[] ft.pixels;
		}

		m_fontTextures.clear();
	}

	void GUIBackend::StartFrame(int threadCount)
	{
		m_guiRenderData.resize(threadCount);
	}

	void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread)
	{
		auto& req				= AddDrawRequest(buf, m_guiRenderData[thread]);
		req.materialData.color1 = buf->m_color.start;
		req.materialData.color2 = buf->m_color.end;

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
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
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
		req.materialData.diffuse.textureIndex = txt->GetBindlessIndex();
		req.materialData.diffuse.samplerIndex = sampler->GetBindlessIndex();
		req.materialData.color1				  = buf->m_tint;
	}

	void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf, int thread)
	{
		auto& req = AddDrawRequest(buf, m_guiRenderData[thread]);
	}

	void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread)
	{
		auto& req							  = AddDrawRequest(buf, m_guiRenderData[thread]);
		auto  txt							  = m_fontTextures[buf->m_textureHandle].texture;
		auto  sampler						  = m_resourceManager->GetResource<TextureSampler>(DEFAULT_GUI_TEXT_SAMPLER_SID);
		req.materialData.diffuse.textureIndex = txt->GetBindlessIndex();
		req.materialData.diffuse.samplerIndex = sampler->GetBindlessIndex();
	}

	void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread)
	{
		auto& req							  = AddDrawRequest(buf, m_guiRenderData[thread]);
		auto  txt							  = m_fontTextures[buf->m_textureHandle].texture;
		auto  sampler						  = m_resourceManager->GetResource<TextureSampler>(DEFAULT_GUI_TEXT_SAMPLER_SID);
		req.materialData.color1				  = buf->m_outlineColor;
		req.materialData.floatPack1			  = Vector4(buf->m_thickness, buf->m_softness, buf->m_outlineThickness, buf->m_outlineThickness != 0.0f ? 1.0f : 0.0f);
		req.materialData.floatPack2			  = Vector4(buf->m_flipAlpha ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);
		req.materialData.diffuse.textureIndex = txt->GetBindlessIndex();
		req.materialData.diffuse.samplerIndex = sampler->GetBindlessIndex();
	}

	void GUIBackend::Render(int threadID)
	{
		// Lazy-get shader
		if (m_shader == nullptr)
			m_shader = m_resourceManager->GetResource<Shader>("Resources/Core/Shaders/GUIStandard.linashader"_hs);

		if (m_lgx == nullptr)
			m_lgx = m_gfxManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper)->GetLGX();

		auto& data = m_guiRenderData[threadID];

		// Update set 1 scene data
		{
			GPUSceneData sceneData;
			sceneData.proj = GetProjectionFromSize(data.size);
			MEMCPY(data.sceneDataMapping, &sceneData, sizeof(GPUSceneData));
		}

		// Update material data.
		{
			Vector<GPUGUIMaterialData> materials;
			materials.resize(data.drawRequests.size());

			uint32 i = 0;
			for (const auto& req : data.drawRequests)
			{
				materials[i] = req.materialData;
				i++;
			}

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
					.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
					.streams		  = &data.copyStream,
					.streamCount	  = 1,
					.useSignal		  = true,
					.signalCount	  = 1,
					.signalSemaphores = &data.copySemaphore,
					.signalValues	  = &val,
					.isMultithreaded  = true,
				};

				m_lgx->CloseCommandStreams(&data.copyStream, 1);
				m_lgx->SubmitCommandStreams(desc);
			}
		}

		// Pipeline
		{
			LinaGX::CMDBindPipeline* pipeline = data.gfxStream->AddCommand<LinaGX::CMDBindPipeline>();
			pipeline->shader				  = m_shader->GetGPUHandle(data.variantPassType);
		}

		// Buffers
		{

			LinaGX::CMDBindVertexBuffers* vtx = data.gfxStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
			vtx->offset						  = 0;
			vtx->slot						  = 0;
			vtx->vertexSize					  = sizeof(LinaVG::Vertex);
			vtx->resource					  = data.vertexBuffer->GetGPUResource();

			LinaGX::CMDBindIndexBuffers* index = data.gfxStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
			index->indexType				   = LinaGX::IndexType::Uint16;
			index->offset					   = 0;
			index->resource					   = data.indexBuffer->GetGPUResource();
		}

		// Descriptors
		{
			LinaGX::CMDBindDescriptorSets* bind = data.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
			bind->descriptorSetHandles			= data.gfxStream->EmplaceAuxMemory<uint16>(m_gfxManager->GetCurrentDescriptorSet0GlobalData(), data.descriptorSet1, data.descriptorSet2);
			bind->firstSet						= 0;
			bind->setCount						= 3;
			bind->isCompute						= false;
		}

		// Draw
		{
			Rectui scissorsRect = Rectui(0, 0, data.size.x, data.size.y);
			for (const auto& req : data.drawRequests)
			{
				Rectui currentRect = Rectui(0, 0, data.size.x, data.size.y);

				if (req.clip.size.x != 0 || req.clip.size.y != 0)
				{
					currentRect.pos	 = req.clip.pos;
					currentRect.size = req.clip.size;
				}

				if (!currentRect.Equals(scissorsRect))
				{
					scissorsRect					 = currentRect;
					LinaGX::CMDSetScissors* scissors = data.gfxStream->AddCommand<LinaGX::CMDSetScissors>();
					scissors->x						 = scissorsRect.pos.x;
					scissors->y						 = scissorsRect.pos.y;
					scissors->width					 = scissorsRect.size.x;
					scissors->height				 = scissorsRect.size.y;
				}

				// Material id.
				LinaGX::CMDBindConstants* constants = data.gfxStream->AddCommand<LinaGX::CMDBindConstants>();
				constants->offset					= 0;
				constants->stages					= data.gfxStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Fragment);
				constants->stagesSize				= 1;
				constants->size						= sizeof(GPUGUIConstants);
				constants->data						= data.gfxStream->EmplaceAuxMemory<GPUGUIConstants>(req.constants);

				// Draw command.
				LinaGX::CMDDrawIndexedInstanced* draw = data.gfxStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
				draw->indexCountPerInstance			  = req.indexCount;
				draw->instanceCount					  = 1;
				draw->startInstanceLocation			  = 0;
				draw->startIndexLocation			  = req.firstIndex;
				draw->baseVertexLocation			  = req.vertexOffset;
			}
		}
	}

	Vector<Texture*> GUIBackend::GetFontTextures()
	{
		Vector<Texture*> vec;
		vec.resize(m_fontTextures.size());

		uint32 i = 0;
		for (const auto& ft : m_fontTextures)
		{
			vec[i] = ft.texture;
			i++;
		}

		return vec;
	}

	void GUIBackend::EndFrame()
	{
	}

	void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
	{
		auto&  ft		   = m_fontTextures[m_boundFontTexture];
		uint32 startOffset = offsetY * ft.width + offsetX;

		for (int i = 0; i < height; i++)
		{
			const uint32 size = width;
			MEMCPY(ft.pixels + startOffset, &data[width * i], size);
			startOffset += ft.width;
		}
	}

	void GUIBackend::BufferEnded()
	{
		auto& ft = m_fontTextures[m_boundFontTexture];
		ft.texture->SetCustomData(ft.pixels, ft.width, ft.height, 1, LinaGX::Format::R8_UNORM);
	}

	void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
	{
		m_boundFontTexture = texture;
	}

	LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
	{
		const String name		 = "GUI Backend Font Texture " + TO_STRING(m_fontTextures.size());
		FontTexture	 fontTexture = {
			 .texture = new Texture(m_resourceManager, true, name, TO_SID(name)),
			 .width	  = width,
			 .height  = height,
		 };

		fontTexture.pixels = new uint8[width * height];

		m_fontTextures.push_back(fontTexture);
		m_boundFontTexture = static_cast<LinaVG::BackendHandle>(m_fontTextures.size() - 1);
		return m_boundFontTexture;
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
