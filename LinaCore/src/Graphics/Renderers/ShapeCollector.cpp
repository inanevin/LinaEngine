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

#include "Core/Graphics/Renderers/ShapeCollector.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{

	void ShapeCollector::Initialize(DrawCollector* dc, LinaGX::Instance* lgx)
	{
		m_drawCollector = dc;
		m_lgx			= lgx;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];

			pfd.line3DVtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(Line3DVertex) * 1000, "ShapeRendererLineVertexBuffer");
			pfd.lvgVtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(LinaVG::Vertex) * 2000, "ShapeRendererLVGVertexBuffer");
			pfd.line3DIdxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(uint16) * 1000, "ShapeRendererLineIndexBuffer");
			pfd.lvgIdxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(LinaVG::Index) * 5000, "ShapeRendererLVGIndexBuffer");
		}

		m_lvgDrawer.GetCallbacks().draw = BIND(&ShapeCollector::OnLinaVGDraw, this, std::placeholders::_1);
	}

	void ShapeCollector::Shutdown()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];

			pfd.line3DVtxBuffer.Destroy();
			pfd.line3DIdxBuffer.Destroy();
			pfd.lvgVtxBuffer.Destroy();
			pfd.lvgIdxBuffer.Destroy();
		}
	}

	void ShapeCollector::AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		PerFrameData& pfd = m_pfd[frameIndex];

		pfd.lvgVtxBuffer.BufferData(0, (uint8*)m_gpuData.lvgVertices.data(), sizeof(LinaVG::Vertex) * m_gpuData.lvgVertices.size());
		pfd.lvgIdxBuffer.BufferData(0, (uint8*)m_gpuData.lvgIndices.data(), sizeof(LinaVG::Index) * m_gpuData.lvgIndices.size());
		pfd.line3DIdxBuffer.BufferData(0, (uint8*)m_gpuData.line3DIndices.data(), sizeof(uint16) * m_gpuData.line3DIndices.size());
		pfd.line3DVtxBuffer.BufferData(0, (uint8*)m_gpuData.line3DVertices.data(), sizeof(Line3DVertex) * m_gpuData.line3DVertices.size());

		queue.AddBufferRequest(&pfd.line3DVtxBuffer);
		queue.AddBufferRequest(&pfd.line3DIdxBuffer);
		queue.AddBufferRequest(&pfd.lvgVtxBuffer);
		queue.AddBufferRequest(&pfd.lvgIdxBuffer);
	}

	void ShapeCollector::SyncRender()
	{
		m_gpuData = m_cpuData;
		m_cpuData.lvgIndices.resize(0);
		m_cpuData.lvgVertices.resize(0);
		m_cpuData.line3DIndices.resize(0);
		m_cpuData.line3DVertices.resize(0);
	}

	void ShapeCollector::StartLVGBatch(StringID groupId, Shader* shader, const GPUEntity& entity, uint64 entityGUID)
	{
		m_currentLvgEntity	   = entity;
		m_currentLvgEntityGUID = entityGUID;
		m_currentLvgGroup	   = groupId;
		m_currentLvgShader	   = shader;
	}

	void ShapeCollector::EndLVGBatch()
	{
		m_lvgDrawer.FlushBuffers();
		m_lvgDrawer.ResetFrame();
	}

	void ShapeCollector::DrawLVGLine(const Vector2& p1, const Vector2& p2, const ColorGrad& color, float thickness)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = color.AsLVG();
		opts.thickness = thickness;
		m_lvgDrawer.DrawLine(p1.AsLVG(), p2.AsLVG(), opts);
	}

	void ShapeCollector::DrawLVGCircle(const Vector2& p0, float radius, const ColorGrad& color, float thickness, float startAngle, float endAngle, bool isFilled)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = color.AsLVG();
		opts.thickness = thickness;
		opts.isFilled  = isFilled;
		m_lvgDrawer.DrawCircle(p0.AsLVG(), radius, opts, 36, 0.0f, startAngle, endAngle);
	}

	void ShapeCollector::OnLinaVGDraw(LinaVG::DrawBuffer* lvg)
	{
		const uint32 baseVertex = static_cast<uint32>(m_cpuData.lvgVertices.size());
		const uint32 indexCount = static_cast<uint32>(lvg->indexBuffer.m_size);
		const uint32 startIndex = static_cast<uint32>(m_cpuData.lvgIndices.size());

		const size_t currentVtx = m_cpuData.lvgVertices.size();
		const size_t currentIdx = m_cpuData.lvgIndices.size();

		m_cpuData.lvgVertices.resize(currentVtx + lvg->vertexBuffer.m_size);
		m_cpuData.lvgIndices.resize(currentIdx + lvg->indexBuffer.m_size);

		MEMCPY(m_cpuData.lvgVertices.data() + currentVtx, lvg->vertexBuffer.m_data, lvg->vertexBuffer.m_size * sizeof(LinaVG::Vertex));
		MEMCPY(m_cpuData.lvgIndices.data() + currentIdx, lvg->indexBuffer.m_data, lvg->indexBuffer.m_size * sizeof(LinaVG::Index));

		const DrawCollector::CustomDrawInstance inst = {
			.entity = m_currentLvgEntity,
			.entityIdent =
				{
					.entityGUID = m_currentLvgEntityGUID,
				},
			.pushEntity	  = true,
			.pushMaterial = false,
		};

		const uint32 bufferIndex = (m_lgx->GetCurrentFrameIndex() + SystemInfo::GetRendererBehindFrames()) % FRAMES_IN_FLIGHT;
		m_drawCollector->AddCustomDraw(m_currentLvgGroup, inst, m_currentLvgShader->GetID(), 0, &m_pfd[bufferIndex].lvgVtxBuffer, &m_pfd[bufferIndex].lvgIdxBuffer, sizeof(LinaVG::Vertex), baseVertex, indexCount, startIndex);
	}

	void ShapeCollector::Start3DBatch(StringID groupId, Shader* shader)
	{
		m_current3DGroup	  = groupId;
		m_current3DShader	  = shader;
		m_current3DBaseVertex = static_cast<uint32>(m_cpuData.line3DVertices.size());
		m_current3DBaseIndex  = static_cast<uint32>(m_cpuData.line3DIndices.size());
	}

	void ShapeCollector::End3DBatch()
	{
		const DrawCollector::CustomDrawInstance inst = {
			.pushEntity	   = false,
			.pushMaterial  = false,
			.pushBoneIndex = false,
		};

		const uint32 indexCount = static_cast<uint32>(m_cpuData.line3DIndices.size()) - m_current3DBaseIndex;

		if (indexCount == 0)
			return;

		const uint32 bufferIndex = (m_lgx->GetCurrentFrameIndex() + SystemInfo::GetRendererBehindFrames()) % FRAMES_IN_FLIGHT;
		m_drawCollector->AddCustomDraw(m_current3DGroup, inst, m_current3DShader->GetID(), 0, &m_pfd[bufferIndex].line3DVtxBuffer, &m_pfd[bufferIndex].line3DIdxBuffer, sizeof(Line3DVertex), m_current3DBaseVertex, indexCount, m_current3DBaseIndex);
	}

	void ShapeCollector::DrawLine3D(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color)
	{
		const uint16 sz = static_cast<uint16>(m_cpuData.line3DVertices.size());

		m_cpuData.line3DVertices.push_back({
			.position	  = p1,
			.nextPosition = p2,
			.color		  = color.start,
			.direction	  = 1.0f * thickness,
		});

		m_cpuData.line3DVertices.push_back({
			.position	  = p1,
			.nextPosition = p2,
			.color		  = color.start,
			.direction	  = -1.0f * thickness,
		});

		m_cpuData.line3DVertices.push_back({
			.position	  = p2,
			.nextPosition = p1,
			.color		  = color.end,
			.direction	  = 1.0f * thickness,
		});

		m_cpuData.line3DVertices.push_back({
			.position	  = p2,
			.nextPosition = p1,
			.color		  = color.end,
			.direction	  = -1.0f * thickness,
		});

		m_cpuData.line3DIndices.push_back(sz + 0);
		m_cpuData.line3DIndices.push_back(sz + 1);
		m_cpuData.line3DIndices.push_back(sz + 2);
		m_cpuData.line3DIndices.push_back(sz + 2);
		m_cpuData.line3DIndices.push_back(sz + 3);
		m_cpuData.line3DIndices.push_back(sz);
	}

} // namespace Lina
