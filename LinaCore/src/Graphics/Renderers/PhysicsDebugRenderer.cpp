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
#include "Core/Graphics/Pipeline/RenderPass.hpp"

namespace Lina
{

	PhysicsDebugRenderer::PhysicsDebugRenderer(RenderPass* targetPass)
	{
		m_targetPass = targetPass;
		JPH::DebugRenderer::Initialize();
		m_shapeRenderer.Initialize();
	}

	PhysicsDebugRenderer::~PhysicsDebugRenderer()
	{
		m_shapeRenderer.Shutdown();
	}

	void PhysicsDebugRenderer::SyncRender()
	{
		m_shapeRenderer.SyncRender();
	}

	void PhysicsDebugRenderer::AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		m_shapeRenderer.AddBuffersToUploadQueue(frameIndex, queue);
	}

	void PhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow inCastShadow)
	{
		const RenderPass::InstancedDraw draw = {};
		// m_targetPass->AddDrawCall(draw);
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
