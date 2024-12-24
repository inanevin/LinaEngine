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

#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	class Shader;
	class ResourceManagerV2;
	class WorldRenderer;
	class DrawCollector;
	class EntityWorld;
	class Material;
	class Model;
	class ResourceUploadQueue;
} // namespace Lina

namespace LinaGX
{
	class CommandStream;
}

namespace Lina::Editor
{
	class Editor;

	class ShapeRenderer
	{

	private:
		struct LineVertex
		{
			Vector3 position;
			Vector3 nextPosition;
			Vector4 color;
			float	direction;
		};

		struct LineRequest
		{
			Vector3	  p1;
			Vector3	  p2;
			float	  thickness;
			ColorGrad color;
		};

		struct LinaVGLine
		{
			Vector2 p1;
			Vector2 p2;
		};

	private:
		struct PerFrameData
		{
			Buffer lineVtxBuffer;
			Buffer lineIdxBuffer;
			Buffer lvgVtxBuffer;
			Buffer lvgIdxBuffer;
		};

		struct RenderData
		{
			Vector<LineRequest>	   lineRequests;
			Vector<LinaVG::Vertex> lvgVertices;
			Vector<LinaVG::Index>  lvgIndices;
		};

	public:
		ShapeRenderer(Editor* editor, WorldRenderer* wr);
		virtual ~ShapeRenderer();

		void DrawLine3D(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color);

		void Initialize();
		void Shutdown();
		void Tick(float delta, DrawCollector& collector);
		void Render(uint32 frameIndex, LinaGX::CommandStream* stream);
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);
		void SyncRender();

		void StartLVGBatch(const GPUEntity& entity, uint64 entityGUID);
		void EndLVGBatch();
		void DrawLVGLine(const Vector2& p1, const Vector2& p2, const ColorGrad& color, float thickness);

	private:
		void OnLinaVGDraw(LinaVG::DrawBuffer* buffer);

	private:
		Editor*			   m_editor		   = nullptr;
		WorldRenderer*	   m_worldRenderer = nullptr;
		ResourceManagerV2* m_rm			   = nullptr;
		EntityWorld*	   m_world		   = nullptr;

		Shader* m_lineShader   = nullptr;
		Shader* m_circleShader = nullptr;
		Shader* m_lvgShader	   = nullptr;

		PerFrameData m_pfd[FRAMES_IN_FLIGHT];
		RenderData	 m_cpuData;
		RenderData	 m_gpuData;

		Vector<LineVertex> m_dummyLines;
		Vector<uint16>	   m_dummyLineIndices;
		LinaVG::Drawer	   m_lvgDrawer;

		GPUEntity m_lvgEntity;
		uint64	  m_lvgEntityGUID = 0;
	};
} // namespace Lina::Editor
