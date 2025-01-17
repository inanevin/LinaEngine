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

#ifdef JPH_DEBUG_RENDERER

#include "Core/Graphics/Renderers/PhysicsDebugRenderer.hpp"
#include "Core/Physics/CommonPhysics.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"

namespace Lina
{

	PhysicsDebugRenderer::PhysicsDebugRenderer(RenderPass* targetPass, Shader* shapeShader, Shader* shaderTriangle)
	{
		m_targetPass	 = targetPass;
		m_shaderShape	 = shapeShader;
		m_shaderTriangle = shaderTriangle;

		JPH::DebugRenderer::Initialize();
		m_shapeRenderer.Initialize();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];
			pfd.triangleVtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(PhysicsDebugVertex) * 50000, "PhyDebugRenderer Vertices");
			pfd.triangleIdxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(uint16) * 80000, "PhyDebugRenderer Indices");
		}
	}

	PhysicsDebugRenderer::~PhysicsDebugRenderer()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];
			pfd.triangleVtxBuffer.Destroy();
			pfd.triangleIdxBuffer.Destroy();
		}

		m_shapeRenderer.Shutdown();
	}

	void PhysicsDebugRenderer::SyncRender()
	{
		m_gpuDrawData = m_cpuDrawData;
		m_cpuDrawData.triangleVertices.resize(0);
		m_cpuDrawData.triangleIndices.resize(0);
		m_shapeRenderer.SyncRender();
	}

	void PhysicsDebugRenderer::AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		PerFrameData& currentFrame = m_pfd[frameIndex];
		currentFrame.triangleVtxBuffer.BufferData(0, (uint8*)m_gpuDrawData.triangleVertices.data(), sizeof(PhysicsDebugVertex) * m_gpuDrawData.triangleVertices.size());
		currentFrame.triangleIdxBuffer.BufferData(0, (uint8*)m_gpuDrawData.triangleIndices.data(), sizeof(uint16) * m_gpuDrawData.triangleIndices.size());
		queue.AddBufferRequest(&currentFrame.triangleVtxBuffer);
		queue.AddBufferRequest(&currentFrame.triangleIdxBuffer);
		m_shapeRenderer.AddBuffersToUploadQueue(frameIndex, queue);
	}

	void PhysicsDebugRenderer::BeginDraws()
	{
		m_shapeRenderer.StartBatch();
	}

	void PhysicsDebugRenderer::SubmitDraws()
	{
		m_shapeRenderer.SubmitBatch(*m_targetPass, 0, m_shaderShape->GetGPUHandle());

		const uint32 ic = static_cast<uint32>(m_cpuDrawData.triangleIndices.size());
		if (ic == 0)
			return;

		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {&m_pfd[0].triangleVtxBuffer, &m_pfd[1].triangleVtxBuffer},
			.indexBuffers  = {&m_pfd[0].triangleIdxBuffer, &m_pfd[1].triangleIdxBuffer},
			.vertexSize	   = sizeof(PhysicsDebugVertex),
			.shaderHandle  = m_shaderTriangle->GetGPUHandle(),
			.baseVertex	   = 0,
			.vertexCount   = static_cast<uint32>(m_cpuDrawData.triangleVertices.size()),
			.baseIndex	   = 0,
			.indexCount	   = ic,
			.instanceCount = 1,
			.baseInstance  = 0,
			.pushConstant  = 0,
		};

		m_targetPass->AddDrawCall(draw);
	}

	void PhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow inCastShadow)
	{
		const uint16 sz = static_cast<uint16>(m_cpuDrawData.triangleVertices.size());
		m_cpuDrawData.triangleVertices.push_back({
			.position = FromJoltVec3(inV1),
			.color	  = FromJoltColor(inColor),
		});
		m_cpuDrawData.triangleVertices.push_back({
			.position = FromJoltVec3(inV2),
			.color	  = FromJoltColor(inColor),
		});
		m_cpuDrawData.triangleVertices.push_back({
			.position = FromJoltVec3(inV3),
			.color	  = FromJoltColor(inColor),
		});
		m_cpuDrawData.triangleIndices.push_back(sz);
		m_cpuDrawData.triangleIndices.push_back(sz + 1);
		m_cpuDrawData.triangleIndices.push_back(sz + 2);
	}

	void PhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
	{
		const Color color = Color(inColor.r, inColor.g, inColor.b, inColor.a);
		m_shapeRenderer.DrawLine3D(FromJoltVec3(inFrom), FromJoltVec3(inTo), 1.0f, color);
	}

	void PhysicsDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight)
	{
	}

	void PhysicsDebugRenderer::DrawGeometry(JPH::RMat44Arg						   inModelMatrix,
											const JPH::AABox&					   inWorldSpaceBounds,
											float								   inLODScaleSq,
											JPH::ColorArg						   inModelColor,
											const JPH::DebugRenderer::GeometryRef& inGeometry,
											JPH::DebugRenderer::ECullMode		   inCullMode,
											JPH::DebugRenderer::ECastShadow		   inCastShadow,
											JPH::DebugRenderer::EDrawMode		   inDrawMode)
	{
		// Figure out which LOD to use
		const JPH::DebugRenderer::LOD* lod = inGeometry->mLODs.data();

		// Draw the batch
		const BatchImpl* batch = static_cast<const BatchImpl*>(lod->mTriangleBatch.GetPtr());
		for (const JPH::DebugRenderer::Triangle& triangle : batch->mTriangles)
		{
			JPH::RVec3 v0	 = inModelMatrix * JPH::Vec3(triangle.mV[0].mPosition);
			JPH::RVec3 v1	 = inModelMatrix * JPH::Vec3(triangle.mV[1].mPosition);
			JPH::RVec3 v2	 = inModelMatrix * JPH::Vec3(triangle.mV[2].mPosition);
			JPH::Color color = inModelColor * triangle.mV[0].mColor;

			switch (inDrawMode)
			{
			case EDrawMode::Wireframe:
				DrawLine(v0, v1, color);
				DrawLine(v1, v2, color);
				DrawLine(v2, v0, color);
				break;

			case EDrawMode::Solid:
				DrawTriangle(v0, v1, v2, color, inCastShadow);
				break;
			}
		}
	}

	JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount)
	{
		BatchImpl* batch = new BatchImpl;
		if (inVertices == nullptr || inVertexCount == 0 || inIndices == nullptr || inIndexCount == 0)
			return batch;

		// Convert indexed triangle list to triangle list
		batch->mTriangles.resize(inIndexCount / 3);
		for (size_t t = 0; t < batch->mTriangles.size(); ++t)
		{
			JPH::DebugRenderer::Triangle& triangle = batch->mTriangles[t];
			triangle.mV[0]						   = inVertices[inIndices[t * 3 + 0]];
			triangle.mV[1]						   = inVertices[inIndices[t * 3 + 1]];
			triangle.mV[2]						   = inVertices[inIndices[t * 3 + 2]];
		}

		return batch;
	}

	JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Triangle* inTriangles, int inTriangleCount)
	{
		BatchImpl* batch = new BatchImpl;
		if (inTriangles == nullptr || inTriangleCount == 0)
			return batch;

		batch->mTriangles.assign(inTriangles, inTriangles + inTriangleCount);
		return batch;
	}

} // namespace Lina

#endif
