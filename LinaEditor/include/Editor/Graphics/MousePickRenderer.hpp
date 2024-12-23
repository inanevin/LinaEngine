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

#include "Core/Graphics/Renderers/FeatureRenderer.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/World/CommonWorld.hpp"

namespace Lina
{
	class Shader;
	class Entity;
	class Texture;
	class GfxContext;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class MousePickRenderer : public FeatureRenderer
	{
	public:
		struct PerFrameData
		{
			Texture* depthTarget  = nullptr;
			Texture* renderTarget = nullptr;
			Buffer	 snapshotBuffer;
		};
		MousePickRenderer(Editor* editor, LinaGX::Instance* lgx, EntityWorld* world, WorldRenderer* wr, ResourceManagerV2* rm);
		virtual ~MousePickRenderer();

		virtual void DestroySizeRelativeResources() override;
		virtual void CreateSizeRelativeResources(const Vector2ui& size) override;

		virtual void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue) override;
		virtual void ProduceFrame(DrawCollector& collector) override;
		virtual void OnPostRender(uint32 frameIndex, LinaGX::CommandStream* stream, const LinaGX::Viewport& vp, const LinaGX::ScissorsRect& sc) override;
		virtual void OnPostSubmit(uint32 frameIndex, LinaGX::CommandStream* stream) override;

		virtual void GetBarriersTextureToAttachment(uint32 frameIndex, Vector<LinaGX::TextureBarrier>& outBarriers) override;

		void PickEntity(const Vector2& position);

	private:
		GfxContext*		 m_gfxContext = nullptr;
		RenderPass		 m_entityBufferPass;
		Shader*			 m_shader = nullptr;
		Editor*			 m_editor = nullptr;
		PerFrameData	 m_pfd[FRAMES_IN_FLIGHT];
		uint16			 m_pipelineLayout = 0;
		Vector2ui		 m_size			  = Vector2ui::Zero;
		Vector<EntityID> m_lastEntityIDs;
		bool			 m_pickEntityRequest  = false;
		Vector2			 m_pickEntityPosition = Vector2::Zero;
	};
} // namespace Lina::Editor
