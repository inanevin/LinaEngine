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

#include "GUI/EditorPayloadManager.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "GUI/Drawers/GUIDrawerPayload.hpp"
#include "GUI/Nodes/GUINode.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "GUI/Nodes/Panels/GUIPanelFactory.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Data/RenderData.hpp"

namespace Lina::Editor
{
	EditorPayloadManager::EditorPayloadManager(Editor* editor) : m_editor(editor)
	{
	}

	void EditorPayloadManager::Initialize()
	{
		m_windowManager = m_editor->GetSystem()->CastSubsystem<WindowManager>(SubsystemType::WindowManager);
		m_input			= m_editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);
		m_gfxManager	= m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		m_window = m_windowManager->CreateAppWindow(EDITOR_PAYLOAD_WINDOW_SID, "EditorPayloadWindow", Vector2i::Zero, Vector2i(800, 600), SRM_DrawGUI);
		m_window->SetStyle(WindowStyle::BorderlessNoResize);
		m_window->SetVisible(false);
		m_window->SetAlpha(0.8f);
		m_window->SetInputPassthrough(true);

		auto surfaceRenderer = m_gfxManager->GetSurfaceRenderer(EDITOR_PAYLOAD_WINDOW_SID);
		m_guiDrawer			 = new GUIDrawerPayload(m_editor, surfaceRenderer->GetSwapchain());
		surfaceRenderer->SetGUIDrawer(m_guiDrawer);
	}

	void EditorPayloadManager::Tick()
	{
		if (m_currentPayloadMeta.type == PayloadType::EPL_None)
			return;

		if (!m_window->GetIsVisible())
			m_window->SetVisible(true);

		const Vector2i windowPos = m_input->GetMousePositionAbs() + Vector2i(m_currentPayloadMeta.delta);
		m_window->SetPos(windowPos);
		m_window->BringToFront();

		if (!m_input->GetMouseButton(LINA_MOUSE_0))
		{
			Event ev	  = Event();
			ev.pParams[0] = &m_currentPayloadMeta;
			m_editor->DispatchEvent(EVE_PayloadDropped, ev);

			const auto& drawers = m_editor->GetGUIDrawers();

			for (auto [sid, drawer] : drawers)
				drawer->OnPayloadEnded(m_currentPayloadMeta.type);

			m_currentPayloadMeta = PayloadMeta();
			m_window->SetVisible(false);
		}
	}

	void EditorPayloadManager::Shutdown()
	{
		delete m_guiDrawer;
		m_windowManager->DestroyAppWindow(EDITOR_PAYLOAD_WINDOW_SID);
	}

	void EditorPayloadManager::CreatePayload(PayloadType type, const Vector2i& windowSize, const Vector2i& delta, void* userData)
	{
		m_currentPayloadMeta.type  = type;
		m_currentPayloadMeta.data  = userData;
		m_currentPayloadMeta.delta = delta;
		m_window->AddSizeRequest(windowSize);

		Event ev	  = Event();
		ev.pParams[0] = &m_currentPayloadMeta;
		m_editor->DispatchEvent(EVE_PayloadCreated, ev);

		const auto& drawers = m_editor->GetGUIDrawers();

		for (auto [sid, drawer] : drawers)
			drawer->OnPayloadCreated(type, userData);
	}

} // namespace Lina::Editor
