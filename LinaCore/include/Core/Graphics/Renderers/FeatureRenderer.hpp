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

	class FeatureRenderer
	{
	public:
		FeatureRenderer() = delete;
		FeatureRenderer(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm) : m_lgx(lgx), m_world(world), m_rm(rm){};
		virtual ~FeatureRenderer() = default;

		virtual void OnComponentAdded(Component* comp){};
		virtual void OnComponentRemoved(Component* comp){};
		virtual void FetchRenderables(){};

		virtual void ProduceFrame(DrawCollector& drawCollector){};
		virtual void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue){};
		virtual void RenderDrawPass(LinaGX::CommandStream* stream, uint32 frameIndex, RenderPass& pass, RenderPassType type){};
		virtual void SyncRender(){};

	protected:
		LinaGX::Instance*  m_lgx   = nullptr;
		EntityWorld*	   m_world = nullptr;
		ResourceManagerV2* m_rm	   = nullptr;

	private:
	};
} // namespace Lina
