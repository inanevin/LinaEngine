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

#pragma once

namespace Lina
{
	class ResourceUploadQueue;
	class RenderPass;
	class Shader;
} // namespace Lina

#ifdef JPH_DEBUG_RENDERER

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include "Core/Graphics/Renderers/ShapeRenderer.hpp"

namespace Lina
{
	class PhysicsDebugRenderer : public JPH::DebugRenderer
	{

	private:
		struct PerFrameData
		{
			Buffer triangleVtxBuffer;
			Buffer triangleIdxBuffer;
		};

		struct DrawData
		{
			Vector<PhysicsDebugVertex> triangleVertices;
			Vector<uint16>			   triangleIndices;
		};

	public:
		PhysicsDebugRenderer(RenderPass* pass, Shader* shapeShader, Shader* shaderTriangle);
		virtual ~PhysicsDebugRenderer();

		void BeginDraws();
		void SubmitDraws();
		void SyncRender();
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);

		virtual void					  DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
		virtual void					  DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow inCastShadow) override;
		virtual void					  DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
		virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const JPH::DebugRenderer::Triangle* inTriangles, int inTriangleCount) override;
		virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const JPH::DebugRenderer::Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount) override;

		virtual void DrawGeometry(JPH::RMat44Arg						 inModelMatrix,
								  const JPH::AABox&						 inWorldSpaceBounds,
								  float									 inLODScaleSq,
								  JPH::ColorArg							 inModelColor,
								  const JPH::DebugRenderer::GeometryRef& inGeometry,
								  JPH::DebugRenderer::ECullMode			 inCullMode	  = JPH::DebugRenderer::ECullMode::CullBackFace,
								  JPH::DebugRenderer::ECastShadow		 inCastShadow = JPH::DebugRenderer::ECastShadow::On,
								  JPH::DebugRenderer::EDrawMode			 inDrawMode	  = JPH::DebugRenderer::EDrawMode::Solid) override;

	private:
		/// Implementation specific batch object
		class BatchImpl : public JPH::RefTargetVirtual
		{
		public:
			JPH_OVERRIDE_NEW_DELETE

			virtual void AddRef() override
			{
				++mRefCount;
			}
			virtual void Release() override
			{
				if (--mRefCount == 0)
					delete this;
			}

			JPH::Array<JPH::DebugRenderer::Triangle> mTriangles;

		private:
			JPH::atomic<uint32> mRefCount = 0;
		};

	private:
		PerFrameData  m_pfd[FRAMES_IN_FLIGHT];
		DrawData	  m_cpuDrawData	   = {};
		DrawData	  m_gpuDrawData	   = {};
		Shader*		  m_shaderShape	   = nullptr;
		Shader*		  m_shaderTriangle = nullptr;
		RenderPass*	  m_targetPass	   = nullptr;
		ShapeRenderer m_shapeRenderer;
	};

} // namespace Lina

#else

namespace Lina
{
	class PhysicsDebugRenderer
	{
	public:
		void Initialize(RenderPass* targetPass, Shader*, Shader*){};
		void Shutdown(){};
		void SyncRender(){};
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue){};
	};
} // namespace Lina
#endif
