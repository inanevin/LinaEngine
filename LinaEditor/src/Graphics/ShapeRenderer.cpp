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

#include "Editor/Graphics/ShapeRenderer.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	ShapeRenderer::ShapeRenderer(Editor* editor, WorldRenderer* wr)
	{
		m_editor		= editor;
		m_worldRenderer = wr;
		m_rm			= &m_editor->GetApp()->GetResourceManager();
		m_lineShader	= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LINE3D_ID);
		m_lvgShader		= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LVG3D_ID);
		// m_circleShader	= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);
		m_world = m_worldRenderer->GetWorld();

	} // namespace Lina::Editor

	ShapeRenderer::~ShapeRenderer()
	{
	}

	void ShapeRenderer::DrawLine3D(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color)
	{
		m_cpuData.lineRequests.push_back({
			.p1		   = p1,
			.p2		   = p2,
			.thickness = thickness,
			.color	   = color,
		});
	}

	void ShapeRenderer::Initialize()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];

			pfd.lineVtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(LineVertex) * 1000, "ShapeRendererLineVertexBuffer");
			pfd.lvgVtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(LinaVG::Vertex) * 1000, "ShapeRendererLVGVertexBuffer");
			pfd.lineIdxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(uint16) * 1000, "ShapeRendererLineIndexBuffer");
			pfd.lvgIdxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(LinaVG::Index) * 1000, "ShapeRendererLVGIndexBuffer");
		}

		m_lvgDrawer.GetCallbacks().draw = BIND(&ShapeRenderer::OnLinaVGDraw, this, std::placeholders::_1);
	}

	void ShapeRenderer::Shutdown()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];

			pfd.lineVtxBuffer.Destroy();
			pfd.lineIdxBuffer.Destroy();
			pfd.lvgVtxBuffer.Destroy();
			pfd.lvgIdxBuffer.Destroy();
		}
	}

	void ShapeRenderer::Tick(float delta, DrawCollector& collector)
	{
		DrawLine3D(Vector3(0, 2, 0), Vector3(1, 2, 0), 0.2f, Color::White);
		DrawLine3D(Vector3(0, 2, 0), Vector3(0, 3, 0), 0.2f, Color::White);
		DrawLine3D(Vector3(0, 2, 0), Vector3(0, 2, 1), 0.2f, Color::White);

		StartLVGBatch({.model = Matrix4::Identity()}, 0);

		DrawLVGLine(Vector2(0, 0), Vector2(100, 0), Color::Red, 5.0f);
		EndLVGBatch();
	}

	void ShapeRenderer::AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		PerFrameData& pfd = m_pfd[frameIndex];

		// From GPU synced data.
		m_dummyLines.resize(0);
		m_dummyLineIndices.resize(0);

		for (const LineRequest& req : m_gpuData.lineRequests)
		{
			const uint16 sz = static_cast<uint16>(m_dummyLines.size());

			m_dummyLines.push_back({
				.position	  = req.p1,
				.nextPosition = req.p2,
				.color		  = req.color.start,
				.direction	  = 1.0f * req.thickness,
			});

			m_dummyLines.push_back({
				.position	  = req.p1,
				.nextPosition = req.p2,
				.color		  = req.color.start,
				.direction	  = -1.0f * req.thickness,
			});

			m_dummyLines.push_back({
				.position	  = req.p2,
				.nextPosition = req.p1,
				.color		  = req.color.end,
				.direction	  = 1.0f * req.thickness,
			});

			m_dummyLines.push_back({
				.position	  = req.p2,
				.nextPosition = req.p1,
				.color		  = req.color.end,
				.direction	  = -1.0f * req.thickness,
			});

			m_dummyLineIndices.push_back(sz + 0);
			m_dummyLineIndices.push_back(sz + 1);
			m_dummyLineIndices.push_back(sz + 2);

			m_dummyLineIndices.push_back(sz + 2);
			m_dummyLineIndices.push_back(sz + 3);
			m_dummyLineIndices.push_back(sz);
		}

		pfd.lineVtxBuffer.BufferData(0, (uint8*)m_dummyLines.data(), sizeof(LineVertex) * m_dummyLines.size());
		pfd.lineIdxBuffer.BufferData(0, (uint8*)m_dummyLineIndices.data(), sizeof(uint16) * m_dummyLineIndices.size());

		pfd.lvgVtxBuffer.BufferData(0, (uint8*)m_gpuData.lvgVertices.data(), sizeof(LinaVG::Vertex) * m_gpuData.lvgVertices.size());
		pfd.lvgIdxBuffer.BufferData(0, (uint8*)m_gpuData.lvgIndices.data(), sizeof(LinaVG::Index) * m_gpuData.lvgIndices.size());

		queue.AddBufferRequest(&pfd.lineVtxBuffer);
		queue.AddBufferRequest(&pfd.lineIdxBuffer);
		queue.AddBufferRequest(&pfd.lvgVtxBuffer);
		queue.AddBufferRequest(&pfd.lvgIdxBuffer);
	}

	void ShapeRenderer::Render(uint32 frameIndex, LinaGX::CommandStream* stream)
	{
		PerFrameData& pfd = m_pfd[frameIndex];

		pfd.lineIdxBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);
		pfd.lineVtxBuffer.BindVertex(stream, sizeof(LineVertex));
		m_lineShader->Bind(stream, m_lineShader->GetGPUHandle());
		LinaGX::CMDDrawIndexedInstanced* draw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
		draw->indexCountPerInstance			  = static_cast<uint32>(m_gpuData.lineRequests.size()) * 6;
		draw->instanceCount					  = 1;

		DrawCollector& collector = m_worldRenderer->GetDrawCollector();
		if (collector.RenderGroupExists("Shape"_hs))
			collector.RenderGroup("Shape"_hs, stream);
	}

	void ShapeRenderer::SyncRender()
	{
		m_gpuData = m_cpuData;
		m_cpuData.lineRequests.resize(0);
		m_cpuData.lvgIndices.resize(0);
		m_cpuData.lvgVertices.resize(0);
	}

	void ShapeRenderer::StartLVGBatch(const GPUEntity& entity, uint64 entityGUID)
	{
		m_lvgDrawer.ResetFrame();
		m_lvgEntity		= entity;
		m_lvgEntityGUID = entityGUID;
	}

	void ShapeRenderer::EndLVGBatch()
	{
		m_lvgDrawer.FlushBuffers();
		m_lvgDrawer.ResetFrame();
	}

	void ShapeRenderer::DrawLVGLine(const Vector2& p1, const Vector2& p2, const ColorGrad& color, float thickness)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = color.AsLVG();
		opts.thickness = thickness;
		m_lvgDrawer.DrawLine(p1.AsLVG(), p2.AsLVG(), opts);
	}

	void ShapeRenderer::OnLinaVGDraw(LinaVG::DrawBuffer* lvg)
	{
		DrawCollector& collector = m_worldRenderer->GetDrawCollector();

		const uint32 baseVertex = static_cast<uint32>(m_cpuData.lvgVertices.size());
		const uint32 indexCount = static_cast<uint32>(lvg->indexBuffer.m_size);
		const uint32 startIndex = static_cast<uint32>(m_cpuData.lvgIndices.size());

		for (int i = 0; i < lvg->vertexBuffer.m_size; i++)
			m_cpuData.lvgVertices.push_back(lvg->vertexBuffer[i]);

		for (int i = 0; i < lvg->indexBuffer.m_size; i++)
			m_cpuData.lvgIndices.push_back(lvg->indexBuffer[i]);

		const DrawCollector::CustomDrawInstance inst = {
			.entity = m_lvgEntity,
			.entityIdent =
				{
					.entityGUID = m_lvgEntityGUID,
				},
			.pushEntity	  = true,
			.pushMaterial = false,
		};

		if (!collector.GroupExists("Shape"_hs))
			collector.CreateGroup("Shape");

		const uint32 bufferIndex = (m_editor->GetApp()->GetLGX()->GetCurrentFrameIndex() + SystemInfo::GetRendererBehindFrames()) % FRAMES_IN_FLIGHT;
		collector.AddCustomDraw("Shape"_hs, inst, m_lvgShader->GetID(), 0, &m_pfd[bufferIndex].lvgVtxBuffer, &m_pfd[bufferIndex].lvgIdxBuffer, sizeof(LinaVG::Vertex), baseVertex, indexCount, startIndex);
	}

} // namespace Lina::Editor
