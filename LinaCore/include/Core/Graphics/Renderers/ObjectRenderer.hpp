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

#include "FeatureRenderer.hpp"

namespace Lina
{
	class CompModel;
	class Shader;

	struct InstanceData
	{
		ResourceID materialID = 0;
		GPUEntity  entity	  = {};
	};

	struct PrimitiveDraw
	{
		ResourceID			 modelID		= 0;
		uint32				 meshIndex		= 0;
		uint32				 primitiveIndex = 0;
		Vector<InstanceData> instances;
	};

	struct PerShaderDraw
	{
		ResourceID			  shader = 0;
		Vector<PrimitiveDraw> primitives;
	};

	class ObjectRenderer : public FeatureRenderer
	{
	public:
		ObjectRenderer(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm) : FeatureRenderer(lgx, world, rm){};
		virtual ~ObjectRenderer() = default;

		virtual void OnComponentAdded(Component* comp) override;
		virtual void OnComponentRemoved(Component* comp) override;
		virtual void FetchRenderables() override;
		virtual void ProduceFrame(const Camera& mainCamera, float delta) override;

		virtual void RenderRecordPass(uint32 frameIndex, RenderPass& pass, RenderPassType type) override;
		virtual void RenderDrawPass(LinaGX::CommandStream* stream, uint32 frameIndex, RenderPass& pass, RenderPassType type) override;

		virtual void SyncRender() override;
		virtual void DropRenderFrame() override;

	private:
		Vector<CompModel*> m_compModels;

		Vector<PerShaderDraw> m_cpuDeferredDraws;
		Vector<PerShaderDraw> m_cpuForwardDraws;
		Vector<PerShaderDraw> m_renderDeferredDraws;
		Vector<PerShaderDraw> m_renderForwardDraws;
	};
} // namespace Lina
