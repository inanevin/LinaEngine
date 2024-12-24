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
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"

namespace Lina
{
	class Shader;
	class Entity;
	class WorldRenderer;
	class ResourceManagerV2;
	class EntityWorld;
} // namespace Lina

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina::Editor
{
	class Editor;

	class EditorWorldRenderer
	{
	private:
		struct PerFrameData
		{
			SemaphoreData copySemaphore		 = {};
			SemaphoreData signalSemaphore	 = {};
			Buffer		  entityBuffer		 = {};
			Buffer		  instanceDataBuffer = {};
		};

	public:
		EditorWorldRenderer(Editor* editor, LinaGX::Instance* lgx, WorldRenderer* wr, ResourceManagerV2* rm);
		virtual ~EditorWorldRenderer();

		void Tick(float delta);
		void SyncRender();
		void UpdateBuffers(uint32 frameIndex);
		void Render(uint32 frameIndex);

		inline SemaphoreData GetSubmitSemaphore(uint32 frameIndex)
		{
			return m_pfd[frameIndex].signalSemaphore;
		};

	private:
	private:
		EntityWorld*	   m_world = nullptr;
		RenderPass		   m_pass;
		LinaGX::Instance*  m_lgx			= nullptr;
		WorldRenderer*	   m_wr				= nullptr;
		ResourceManagerV2* m_rm				= nullptr;
		Editor*			   m_editor			= nullptr;
		uint16			   m_pipelineLayout = 0;
		PerFrameData	   m_pfd[FRAMES_IN_FLIGHT];
		DrawCollector	   m_drawCollector;

		Shader*	  m_gridShader	 = nullptr;
		Material* m_gridMaterial = nullptr;
	};
} // namespace Lina::Editor
