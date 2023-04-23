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

#include "Graphics/Core/GUIRenderer.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Interfaces/IGfxResourceCPU.hpp"
#include "Graphics/Interfaces/IGfxResourceGPU.hpp"
#include "Graphics/Resource/Material.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Math/Math.hpp"
#include "Math/Rect.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina
{
#define DEF_VTX_BUF_SIZE	4
#define DEF_INDEX_BUF_SIZE	4
#define DEF_MAT_SIZE		24
#define DEF_DRAWREQ_RESERVE 25

	GUIRenderer::GUIRenderer(GfxManager* gfxMan, StringID ownerSid, uint32 imageCount, IUploadContext* context)
		: m_uploadContext(context), m_gfxManager(gfxMan), m_ownerSid(ownerSid), m_imageCount(imageCount),
		  m_materialPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(Material) * DEF_MAT_SIZE, sizeof(Material), "GUI Renderer Material Pool")
	{
		m_renderer		  = m_gfxManager->GetRenderer();
		m_resourceManager = gfxMan->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		AllocateMaterials();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame			 = m_frames[i];
			frame.vtxBufferGPU	 = m_renderer->CreateGPUResource(DEF_VTX_BUF_SIZE, GPUResourceType::CPUVisibleIfPossible, false, L"GUI Renderer Vertex GPU");
			frame.indexBufferGPU = m_renderer->CreateGPUResource(DEF_INDEX_BUF_SIZE, GPUResourceType::CPUVisibleIfPossible, false, L"GUI Renderer Index GPU");
			frame.viewDataBuffer = m_renderer->CreateCPUResource(sizeof(GPUViewData), CPUResourceHint::ConstantBuffer, L"GUI Renderer View Data");
			frame.drawRequests.resize(DEF_DRAWREQ_RESERVE);
		}
	}

	GUIRenderer::~GUIRenderer()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frames[i];
			m_renderer->DeleteGPUResource(frame.indexBufferGPU);
			m_renderer->DeleteGPUResource(frame.vtxBufferGPU);
			m_renderer->DeleteCPUResource(frame.viewDataBuffer);
		}

		for (auto mat : m_materials)
		{
			mat->~Material();
			m_materialPool.Free(mat);
			mat = nullptr;
		}

		m_materials.clear();
	}

	void GUIRenderer::FeedGradient(LinaVG::GradientDrawBuffer* buf)
	{
		return;

		OrderedDrawRequest& req			   = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Gradient);
		req.materialDefinition.color1	   = FL4(buf->m_color.start);
		req.materialDefinition.color2	   = FL4(buf->m_color.end);
		req.materialDefinition.float4pack2 = Vector4(buf->m_color.radialSize, 0.0f, 0.0f, 0.0f);
		req.materialDefinition.intpack1	   = Vector4i(1, static_cast<int>(buf->m_color.gradientType), buf->m_isAABuffer, 0);
		req.clipPosX					   = buf->clipPosX;
		req.clipPosY					   = buf->clipPosY;
		req.clipSizeX					   = buf->clipSizeX;
		req.clipSizeY					   = buf->clipSizeY;
	}

	void GUIRenderer::FeedTextured(LinaVG::TextureDrawBuffer* buf)
	{
		return;

		const StringID		sid				= "Resources/Core/Textures/Logo_White_512.png"_hs;
		OrderedDrawRequest& req				= AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Textured);
		req.materialDefinition.intpack1		= Vector4i(2, 0, 0, 0);
		req.materialDefinition.float4pack1	= Vector4(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y, buf->m_textureUVOffset.x, buf->m_textureUVOffset.y);
		req.materialDefinition.color1		= Vector4(buf->m_tint.x, buf->m_tint.y, buf->m_tint.z, buf->m_tint.w);
		req.materialDefinition.diffuseIndex = buf->m_textureHandle;
		req.clipPosX						= buf->clipPosX;
		req.clipPosY						= buf->clipPosY;
		req.clipSizeX						= buf->clipSizeX;
		req.clipSizeY						= buf->clipSizeY;
	}

	void GUIRenderer::FeedDefault(LinaVG::DrawBuffer* buf)
	{
		OrderedDrawRequest& req			= AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Default);
		req.materialDefinition.intpack1 = Vector4i(0, 0, 0, 0);
		req.clipPosX					= buf->clipPosX;
		req.clipPosY					= buf->clipPosY;
		req.clipSizeX					= buf->clipSizeX;
		req.clipSizeY					= buf->clipSizeY;
	}

	void GUIRenderer::FeedSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
	{
		return;

		OrderedDrawRequest& req				= AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::SimpleText);
		req.materialDefinition.intpack1		= Vector4i(3, 0, 0, 0);
		req.materialDefinition.diffuseIndex = buf->m_textureHandle;
		req.clipPosX						= buf->clipPosX;
		req.clipPosY						= buf->clipPosY;
		req.clipSizeX						= buf->clipSizeX;
		req.clipSizeY						= buf->clipSizeY;
	}

	void GUIRenderer::FeedSDFText(LinaVG::SDFTextDrawBuffer* buf)
	{
		return;

		OrderedDrawRequest& req				 = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::SDF);
		const float			thickness		 = 1.0f - Math::Clamp(buf->m_thickness, 0.0f, 1.0f);
		const float			softness		 = Math::Clamp(buf->m_softness, 0.0f, 10.0f) * 0.1f;
		const float			outlineThickness = Math::Clamp(buf->m_outlineThickness, 0.0f, 1.0f);
		req.materialDefinition.color1		 = FL4(buf->m_outlineColor);
		req.materialDefinition.float4pack2	 = Vector4(thickness, softness, outlineThickness, 0.0f);
		req.materialDefinition.intpack1		 = Vector4i(4, buf->m_outlineThickness != 0.0f ? 1 : 0, buf->m_flipAlpha, 0);
		req.materialDefinition.diffuseIndex	 = buf->m_textureHandle;
		req.clipPosX						 = buf->clipPosX;
		req.clipPosY						 = buf->clipPosY;
		req.clipSizeX						 = buf->clipSizeX;
		req.clipSizeY						 = buf->clipSizeY;
	}

	void GUIRenderer::Render(uint32 cmdList)
	{
		auto& frame = m_frames[m_frameIndex];
		if (frame.drawRequests.empty())
			return;

		// Transfer vertex & index data

		frame.vtxBufferGPU->Copy(CopyDataType::CopyImmediately, m_uploadContext);
		frame.indexBufferGPU->Copy(CopyDataType::CopyImmediately, m_uploadContext);
		
		m_renderer->BindVertexBuffer(cmdList, frame.vtxBufferGPU, sizeof(LinaVG::Vertex), 0, sizeof(LinaVG::Vertex) * frame.vertexCounter);
		m_renderer->BindIndexBuffer(cmdList, frame.indexBufferGPU, sizeof(LinaVG::Index) * frame.indexCounter);

		// View data & pipeline
		{
			frame.viewData.proj = m_projection;
			frame.viewDataBuffer->BufferData(&frame.viewData, sizeof(GPUViewData));
			m_renderer->BindUniformBuffer(cmdList, GBB_ViewData, frame.viewDataBuffer);
			m_renderer->BindPipeline(cmdList, m_materials[0]->GetShader());
		}

		

		// Allocate new materials if necessary,
		// Assign request definition data & bind materials, then finally draw.
		{
			const uint32 requestsSize = frame.drawReqCounter;

			if (requestsSize > static_cast<uint32>(m_materials.size()))
				AllocateMaterials();

			for (uint32 i = 0; i < requestsSize; i++)
				AssignStandardMaterial(m_materials[i], frame.drawRequests[i].materialDefinition);

			m_renderer->BindMaterials(m_materials.data(), requestsSize);

			for (uint32 i = 0; i < requestsSize; i++)
			{
				auto& req = frame.drawRequests[i];
				m_renderer->SetMaterialID(cmdList, m_materials[i]->GetGPUBindlessIndex());

				if (req.clipSizeX == 0 || req.clipSizeY == 0)
					m_renderer->SetScissors(cmdList, Recti(Vector2i::Zero, m_size));
				else
					m_renderer->SetScissors(cmdList, Recti(Vector2i(req.clipPosX, req.clipPosY), Vector2i(req.clipSizeX, req.clipSizeY)));

				m_renderer->DrawIndexedInstanced(cmdList, req.indexSize, 1, req.firstIndex, req.vertexOffset, 0);

				m_renderer->SetScissors(cmdList, Recti(Vector2i::Zero, m_size));
			}
		}

		frame.vertexCounter = frame.indexCounter = 0;
		frame.drawReqCounter					 = 0;
	}

	void GUIRenderer::Prepare(const Vector2i& surfaceRendererSize, uint32 frameIndex, uint32 imageIndex)
	{
		m_frameIndex = frameIndex;
		m_imageIndex = imageIndex;

		const bool shouldUpdateProjection = surfaceRendererSize != m_size;
		m_size							  = surfaceRendererSize;

		if (shouldUpdateProjection)
			UpdateProjection();
	}

	void GUIRenderer::AllocateMaterials()
	{
		const uint32 currentSize = static_cast<uint32>(m_materials.size());
		const uint32 targetSize	 = currentSize + DEF_MAT_SIZE;
		m_materials.resize(targetSize, nullptr);

		for (uint32 i = currentSize; i < targetSize; i++)
		{
			const String   path = "GUIRenderer_" + TO_STRING(m_ownerSid) + "_Material_" + TO_STRING(i);
			const StringID sid	= TO_SID(path);
			auto&		   mat	= m_materials[i];
			mat					= new (m_materialPool.Allocate(sizeof(Material))) Material(m_resourceManager, true, path, sid);
			mat->SetShader("Resources/Core/Shaders/GUIStandard.linashader"_hs);
		}
	}

	void GUIRenderer::UpdateProjection()
	{
		const Vector2i pos	= Vector2i();
		float		   L	= static_cast<float>(pos.x);
		float		   R	= static_cast<float>(pos.x + m_size.x);
		float		   B	= static_cast<float>(pos.y + m_size.y);
		float		   T	= static_cast<float>(pos.y);
		const float	   zoom = 1.0f;
		L *= zoom;
		R *= zoom;
		T *= zoom;
		B *= zoom;
		m_projection = Matrix4::Orthographic(L, R, B, T, 0.0f, 1.0f);
	}

	void GUIRenderer::AssignStandardMaterial(Material* mat, const GUIStandardMaterialDefinition& def)
	{
		mat->SetProperty("color1", def.color1);
		mat->SetProperty("color2", def.color2);
		mat->SetProperty("float4pack1", def.float4pack1);
		mat->SetProperty("float4pack2", def.float4pack2);
		mat->SetProperty("int4pack1", def.intpack1);
		mat->SetProperty("diffuse", def.diffuseIndex);
	}

	GUIRenderer::OrderedDrawRequest& GUIRenderer::AddOrderedDrawRequest(LinaVG::DrawBuffer* buf, LinaVGDrawCategoryType type)
	{
		auto& frame = m_frames[m_frameIndex];

		OrderedDrawRequest req = OrderedDrawRequest();
		req.firstIndex		   = frame.indexCounter;
		req.vertexOffset	   = frame.vertexCounter;
		req.type			   = type;
		req.indexSize		   = static_cast<uint32>(buf->m_indexBuffer.m_size);
		req.meta.clipX		   = buf->clipPosX;
		req.meta.clipY		   = buf->clipPosY;
		req.meta.clipW		   = buf->clipSizeX == 0 ? m_size.x : buf->clipSizeX;
		req.meta.clipH		   = buf->clipSizeY == 0 ? m_size.y : buf->clipSizeY;

		frame.indexBufferGPU->BufferData(buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index), frame.indexCounter * sizeof(LinaVG::Index));
		frame.vtxBufferGPU->BufferData(buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex), frame.vertexCounter * sizeof(LinaVG::Vertex));

		frame.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
		frame.indexCounter += static_cast<uint32>(buf->m_indexBuffer.m_size);

		const size_t currentSz = frame.drawRequests.size();
		if (frame.drawReqCounter == currentSz)
			frame.drawRequests.resize(currentSz + DEF_DRAWREQ_RESERVE);

		frame.drawRequests[frame.drawReqCounter] = req;
		frame.drawReqCounter++;

		return frame.drawRequests[frame.drawReqCounter];
	}
} // namespace Lina
