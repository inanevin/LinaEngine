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

#include "GUI/SplashScreenGUIDrawer.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Core/ISwapchain.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/CommonResources.hpp"

namespace Lina::Editor
{
	SplashScreenGUIDrawer::SplashScreenGUIDrawer(Editor* editor, Lina::SurfaceRenderer* sf) : m_editor(editor), EditorGUIDrawer(sf)
	{
		m_editor->GetSystem()->AddListener(this);
	}

	SplashScreenGUIDrawer::~SplashScreenGUIDrawer()
	{
		m_editor->GetSystem()->RemoveListener(this);
	}

	void SplashScreenGUIDrawer::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		LOCK_GUARD(m_mtx);

		if (eventType & EVS_ResourceLoaded)
		{
			ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[1]);
			m_loadedResources++;
			m_progress = static_cast<float>(m_loadedResources) / static_cast<float>(task->identifiers.size());
		}
	}

	void SplashScreenGUIDrawer::DrawGUI(int threadID)
	{
		return;
		const Vector2 size = m_surfaceRenderer->GetSwapchain()->GetSize();

		// Base image.
		LinaVG::StyleOptions style;
		style.textureHandle = "Resources/Editor/Textures/SplashScreen.png"_hs;
		//LinaVG::DrawRect(LV2(Vector2(0, 0)), LV2(size), style);

		style.color = LV4(Vector4(1,0,0,1));
		style.textureHandle = 0;
	//	LinaVG::DrawRect(LV2(Vector2(0, 0)), LV2(Vector2(50,50)), style, 0.0f, 2);

		//style.color = LV4(Vector4(1,1,0,1));
		//LinaVG::DrawRect(LV2(Vector2(0, 0)), LV2(Vector2(500,50)), style, 0.0f, 1);

		// Progress bar.
		const Vector2 progressBarStart = Vector2(size.x * 0.35f, size.y * 0.8f);
		const Vector2 progressBarSize  = Vector2(size.x - progressBarStart.x - size.x * 0.15f, size.y * 0.05f);
		//m_progressBar.Draw(progressBarStart, progressBarSize, m_progress);
	}
} // namespace Lina::Editor
