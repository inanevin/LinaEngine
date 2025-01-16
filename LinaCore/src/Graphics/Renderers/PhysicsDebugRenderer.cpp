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

namespace Lina
{

	void PhysicsDebugRenderer::Initialize()
	{
		m_shapeRenderer.Initialize();
	}

	void PhysicsDebugRenderer::Shutdown()
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
	}

	JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount)
	{
	}

	JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Triangle* inTriangles, int inTriangleCount)
	{
	}

} // namespace Lina

#endif
