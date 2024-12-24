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

#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/World/CommonWorld.hpp"

namespace Lina
{
	class Shader;
	class Entity;
	class Texture;
	class WorldRenderer;
	class EntityWorld;
	class DrawCollector;
	class ResourceManagerV2;
} // namespace Lina

namespace LinaGX
{
	class Instance;
}

namespace Lina::Editor
{
	class Editor;

	class MousePickRenderer
	{
	public:
		struct PerFrameData
		{
			Texture* depthTarget  = nullptr;
			Texture* renderTarget = nullptr;
			Buffer	 snapshotBuffer;
		};

		MousePickRenderer(Editor* editor, WorldRenderer* wr);
		virtual ~MousePickRenderer();

		void Initialize();
		void Shutdown();

		void DestroySizeRelativeResources();
		void CreateSizeRelativeResources();
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);
		void Tick(float delta, DrawCollector& collector);
		void Render(uint32 frameIndex, LinaGX::CommandStream* stream, DrawCollector& collector);
		void SyncRender();

		inline EntityID GetLastHoveredEntity() const
		{
			return m_lastHoveredEntityCPU;
		}

	private:
		LinaGX::Instance*  m_lgx	= nullptr;
		Editor*			   m_editor = nullptr;
		WorldRenderer*	   m_wr		= nullptr;
		ResourceManagerV2* m_rm		= nullptr;
		EntityWorld*	   m_world	= nullptr;

		RenderPass		 m_entityBufferPass;
		PerFrameData	 m_pfd[FRAMES_IN_FLIGHT];
		uint16			 m_pipelineLayout		= 0;
		Vector2ui		 m_size					= Vector2ui::Zero;
		EntityID		 m_lastHoveredEntityGPU = 0;
		EntityID		 m_lastHoveredEntityCPU = 0;
		Vector<EntityID> m_lastEntityIDs;
		bool			 m_init = false;
	};
} // namespace Lina::Editor
