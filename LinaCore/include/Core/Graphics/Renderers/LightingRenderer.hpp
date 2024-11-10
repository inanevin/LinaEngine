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
	struct LightDraw
	{
		ResourceID materialID = 0;
	};

	class LightingRenderer : public FeatureRenderer
	{
	public:
		LightingRenderer(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm) : FeatureRenderer(lgx, world, rm){};
		virtual ~LightingRenderer() = default;

		virtual void ProduceFrame(const Camera& mainCamera, float delta) override;
		virtual void RenderDrawLighting(LinaGX::CommandStream* stream) override;
		virtual void SyncRender() override;

	private:
		LightDraw m_cpuDraw	   = {};
		LightDraw m_renderDraw = {};
	};
} // namespace Lina
