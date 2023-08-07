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

#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Graphics/Core/GUIRenderer.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/SystemInfo.hpp"

namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, StringID sid, const Vector2ui& initialSize) : m_gfxManager(man), m_sid(sid)
	{
		m_gfxManager->GetSystem()->AddListener(this);
		m_guiRenderer = new GUIRenderer(man, sid);
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		delete m_guiRenderer;

		m_gfxManager->GetSystem()->RemoveListener(this);
	}

	void SurfaceRenderer::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_WindowResized)
		{
		}
	}

	void SurfaceRenderer::SetGUIDrawer(IGUIDrawer* drawer)
	{
		m_guiDrawer = drawer;
		m_guiDrawer->SetSurfaceRenderer(this);
	}

	void SurfaceRenderer::Render(int surfaceRendererIndex, uint32 frameIndex)
	{
	}

	void SurfaceRenderer::Present()
	{
	}

} // namespace Lina
