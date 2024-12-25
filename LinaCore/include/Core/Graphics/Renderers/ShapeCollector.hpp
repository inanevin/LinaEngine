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

#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Common/Math/Color.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace LinaGX
{
	class Instance;
}
namespace Lina
{
	class DrawCollector;
	class ResourceUploadQueue;
	class Shader;

	class ShapeCollector
	{
	private:
		struct PerFrameData
		{
			Buffer lvgVtxBuffer;
			Buffer lvgIdxBuffer;
			Buffer line3DVtxBuffer;
			Buffer line3DIdxBuffer;
		};

		struct Line3DVertex
		{
			Vector3 position;
			Vector3 nextPosition;
			Vector4 color;
			float	direction;
		};

		struct RenderData
		{
			Vector<Line3DVertex>   line3DVertices;
			Vector<uint16>		   line3DIndices;
			Vector<LinaVG::Vertex> lvgVertices;
			Vector<LinaVG::Index>  lvgIndices;
		};

	public:
		void Initialize(DrawCollector* dc, LinaGX::Instance* lgx);
		void Shutdown();

		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);
		void SyncRender();

		void StartLVGBatch(StringID groupId, Shader* shader, const GPUEntity& entity, uint64 entityGUID);
		void EndLVGBatch();
		void DrawLVGLine(const Vector2& p1, const Vector2& p2, const ColorGrad& color, float thickness);
		void DrawLVGCircle(const Vector2& p0, float radius, const ColorGrad& color, float thickness, float startAngle, float endAngle, bool isFilled);
		void OnLinaVGDraw(LinaVG::DrawBuffer* buffer);

		void Start3DBatch(StringID groupId, Shader* shader);
		void End3DBatch();
		void DrawLine3D(StringID groupId, const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color);

	private:
		LinaGX::Instance* m_lgx = nullptr;
		PerFrameData	  m_pfd[FRAMES_IN_FLIGHT];
		DrawCollector*	  m_drawCollector;
		LinaVG::Drawer	  m_lvgDrawer;
		RenderData		  m_cpuData;
		RenderData		  m_gpuData;
		GPUEntity		  m_currentLvgEntity;
		uint64			  m_currentLvgEntityGUID = 0;
		StringID		  m_currentLvgGroup		 = 0;
		Shader*			  m_currentLvgShader	 = nullptr;

		StringID m_current3DGroup	   = 0;
		Shader*	 m_current3DShader	   = nullptr;
		uint32	 m_current3DBaseVertex = 0;
		uint32	 m_current3DBaseIndex  = 0;
	};
} // namespace Lina
