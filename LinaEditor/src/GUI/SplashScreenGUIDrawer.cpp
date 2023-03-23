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
#include "Resources/Core/ResourceManager.hpp"
#include "Core/Theme.hpp"
#include "GUI/Utility/DrawerUtility.hpp"

namespace Lina::Editor
{
	SplashScreenGUIDrawer::SplashScreenGUIDrawer(Editor* editor, Lina::SurfaceRenderer* sf) : m_editor(editor), m_progressBar(sf), EditorGUIDrawer(sf)
	{
		m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->AddListener(this);

		m_currentResource				   = "Loading core resources...";
		m_progressBar.BackgroundColor	   = Theme::TC_Silent;
		m_progressBar.ForegroundStartColor = Theme::TC_CyanAccent;
		m_progressBar.ForegroundEndColor   = Theme::TC_PurpleAccent;
	}

	SplashScreenGUIDrawer::~SplashScreenGUIDrawer()
	{
		m_editor->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->RemoveListener(this);
	}

	void SplashScreenGUIDrawer::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		LOCK_GUARD(m_mtx);

		if (eventType & EVS_ResourceLoaded)
		{
			ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[1]);
			m_loadedResources++;
			m_currentResource = *static_cast<String*>(ev.pParams[0]);
			m_progress		  = static_cast<float>(m_loadedResources) / static_cast<float>(task->identifiers.size());
		}
	}

	void SplashScreenGUIDrawer::DrawGUI(int threadID)
	{
		const Vector2 size = m_surfaceRenderer->GetSwapchain()->GetSize();

		// Base image.
		{
			LinaVG::StyleOptions style;
			style.textureHandle = "Resources/Editor/Textures/SplashScreen.png"_hs;
			LinaVG::DrawRect(LV2(Vector2(0, 0)), LV2(size), style);
		}

		// Progress bar & copyright text.
		{
			const Vector2 progressBarStart = Vector2(size.x * 0.4f, size.y * 0.88f);
			const Vector2 progressBarSize  = Vector2(size.x - progressBarStart.x - size.x * 0.05f, size.y * 0.005f);
			const Vector2 txtPos		   = Vector2(progressBarStart.x, progressBarStart.y - progressBarSize.y * 10.0f);
			m_progressBar.Progress		   = m_progress;
			m_progressBar.Draw(progressBarStart, progressBarSize, txtPos, m_progress, m_currentResource.c_str());

			LinaVG::TextOptions copyrightText;
			copyrightText.font	= Theme::GetFont(FontType::AltEditor, m_surfaceRenderer->GetSwapchain()->GetWindowDPIScale());
			copyrightText.color = LV4(Theme::TC_VerySilent);

			Vector2		 copyrightTextLine = Vector2(size.x * 0.06f, size.y * 0.425f);
			const String versionText	   = "v" + TO_STRING(LINA_MAJOR) + "." + TO_STRING(LINA_MINOR) + "." + TO_STRING(LINA_PATCH) + " b" + TO_STRING(LINA_BUILD);
			const char*	 text			   = versionText.c_str();
			const char*	 text2			   = "Copyright (c) [2018-] Inan Evin";
			const char*	 text3			   = "https://github.com/inanevin/LinaEngine";
			LinaVG::DrawTextNormal(m_surfaceRenderer->GetSurfaceRendererIndex(), text, LV2(copyrightTextLine), copyrightText, 0.0f, 1);

			Vector2 textSize = FL2(LinaVG::CalculateTextSize(text, copyrightText));
			copyrightTextLine.y += textSize.y * 2.25f;
			LinaVG::DrawTextNormal(m_surfaceRenderer->GetSurfaceRendererIndex(), text2, LV2(copyrightTextLine), copyrightText, 0.0f, 1);

			textSize = FL2(LinaVG::CalculateTextSize(text2, copyrightText));
			copyrightTextLine.y += textSize.y * 2.25f;
			copyrightText.font	= Theme::GetFont(FontType::DefaultEditor, m_surfaceRenderer->GetSwapchain()->GetWindowDPIScale());
			copyrightText.color = LV4(Theme::TC_White);
			LinaVG::DrawTextNormal(m_surfaceRenderer->GetSurfaceRendererIndex(), text3, LV2(copyrightTextLine), copyrightText, 0.0f, 1);
		}
	}
} // namespace Lina::Editor
