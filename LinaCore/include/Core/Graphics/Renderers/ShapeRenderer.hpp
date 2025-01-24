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

#include "Common/Math/Vector.hpp"
#include "Common/Math/Color.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	class RenderPass;
	class ResourceUploadQueue;

	class ShapeRenderer
	{
	private:
		struct PerFrameData
		{
			Buffer vtxBuffer;
			Buffer idxBuffer;
		};

		struct DrawData
		{
			Vector<Line3DVertex> vertices;
			Vector<uint16>		 indices;
		};

	public:
		void Initialize();
		void Shutdown();
		void SyncRender();
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);

		void StartBatch();
		void SubmitBatch(RenderPass& pass, uint32 pushConstantValue, uint32 shaderHandle);

		void DrawLine3D(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color);
		void DrawWireframeCube3D(const Vector3& center, const Vector3& extents, float thickness, const ColorGrad& color);
		void DrawWireframePlane3D(const Vector3& center, const Vector3& extents, float thickness, const ColorGrad& color);
		void DrawWireframeCircle3D(const Vector3& center, float radius, float thickness, const ColorGrad& color);
		void DrawWireframeSphere3D(const Vector3& center, float radius, float thickness, const ColorGrad& color);
		void DrawWireframeHemiSphere3D(const Vector3& center, float radius, float thickness, const ColorGrad& color);
		void DrawWireCone3D(const Vector3& top, const Vector3& dir, float length, const float radius, float thickness, const ColorGrad& color, bool drawLines);

		void DrawWireframeCylinder3D(const Vector3& center, float height, float radius, float thickness, const ColorGrad& color);
		void DrawWireframeCapsule3D(const Vector3& center, float height, float radius, float thickness, const ColorGrad& color);

	private:
		PerFrameData m_pfd[FRAMES_IN_FLIGHT];
		DrawData	 m_cpuDrawData = {};
		DrawData	 m_gpuDrawData = {};
		uint32		 m_startVertex = 0;
		uint32		 m_startIndex  = 0;
	};
} // namespace Lina
