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

#include "Core/Graphics/Renderers/PhysicsDebugRenderer.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>

namespace Lina
{
	class PhysicsDebugRenderer : public JPH::DebugRenderer
	{
	public:
		virtual void  DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
		virtual void  DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
		virtual void  DrawText3D(JPH::RVec3Arg inPosition, const string_view& inString, JPH::ColorArg inColor, float inHeight) override;
		virtual Batch CreateTriangleBatch(const JPH::Triangle* inTriangles, int inTriangleCount) override;
		virtual Batch CreateTriangleBatch(const JPH::Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount) override;
		virtual void  DrawGeometry(JPH::RMat44Arg		   inModelMatrix,
								   const JPH::AABox&	   inWorldSpaceBounds,
								   float				   inLODScaleSq,
								   JPH::ColorArg		   inModelColor,
								   const JPH::GeometryRef& inGeometry,
								   JPH::ECullMode		   inCullMode	= JPH::ECullMode::CullBackFace,
								   JPH::ECastShadow		   inCastShadow = JPH::ECastShadow::On,
								   JPH::EDrawMode		   inDrawMode	= JPH::EDrawMode::Solid) override;
	};
} // namespace Lina
