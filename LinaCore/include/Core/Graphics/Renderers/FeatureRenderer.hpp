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

#include "Common/Data/Vector.hpp"
#include "Core/World/Camera.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Resources/CommonResources.hpp"

namespace LinaGX
{
	class Instance;
	class CommandStream;
	struct Viewport;
	struct ScissorsRect;
	struct TextureBarrier;
}; // namespace LinaGX

namespace Lina
{
	class Component;
	class EntityWorld;
	class ResourceManagerV2;
	class Buffer;
	class RenderPass;
	class ResourceUploadQueue;
	class DrawCollector;
	class WorldRenderer;

	class FeatureRenderer
	{
	public:
		FeatureRenderer() = delete;
		FeatureRenderer(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm, WorldRenderer* wr) : m_lgx(lgx), m_world(world), m_rm(rm), m_wr(wr){};
		virtual ~FeatureRenderer() = default;

		virtual void OnComponentAdded(Component* comp){};
		virtual void OnComponentRemoved(Component* comp){};
		virtual void FetchRenderables(){};

		virtual void DestroySizeRelativeResources(){};
		virtual void CreateSizeRelativeResources(const Vector2ui& size){};

		virtual void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue){};

		virtual void OnProduceFrame(DrawCollector& drawCollector){};
		virtual void OnRenderPass(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPassType type){};
		virtual void OnRenderPassPost(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPassType type){};
		virtual void OnPostRender(uint32 frameIndex, LinaGX::CommandStream* stream, const LinaGX::Viewport& vp, const LinaGX::ScissorsRect& sc){};
		virtual void OnPostSubmit(uint32 frameIndex, LinaGX::CommandStream* stream){};
		virtual void SyncRender(){};

		virtual void GetBarriersTextureToAttachment(uint32 frameIndex, Vector<LinaGX::TextureBarrier>& outBarriers){};
		virtual void GetBarriersTextureToShaderRead(uint32 frameIndex, Vector<LinaGX::TextureBarrier>& outBarriers){};

	protected:
		LinaGX::Instance*  m_lgx   = nullptr;
		EntityWorld*	   m_world = nullptr;
		ResourceManagerV2* m_rm	   = nullptr;
		WorldRenderer*	   m_wr	   = nullptr;

	private:
	};
} // namespace Lina
