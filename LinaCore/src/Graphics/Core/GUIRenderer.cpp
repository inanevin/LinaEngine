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

#include "Graphics/Core/GUIRenderer.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{

	GUIRenderer::GUIRenderer(GfxManager* gfxMan, StringID ownerSid)
	{
	}

	GUIRenderer::~GUIRenderer()
	{
	}

	void GUIRenderer::FeedGradient(LinaVG::GradientDrawBuffer* buf)
	{
	}

	void GUIRenderer::FeedTextured(LinaVG::TextureDrawBuffer* buf)
	{
	}

	void GUIRenderer::FeedDefault(LinaVG::DrawBuffer* buf)
	{
	}

	void GUIRenderer::FeedSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
	{
	}

	void GUIRenderer::FeedSDFText(LinaVG::SDFTextDrawBuffer* buf)
	{
	}

	void GUIRenderer::AllocateMaterials()
	{
	}

	void GUIRenderer::UpdateProjection()
	{
	}

} // namespace Lina
