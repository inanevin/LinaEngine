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

#include "Editor/Widgets/World/WorldDisplayer.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Application.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{

	void WorldDisplayer::Construct()
	{
		GetFlags().Set(WF_KEY_PASSTHRU | WF_MOUSE_PASSTHRU);
		GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		GetWidgetProps().rounding		  = 0.025f;
	}

	void WorldDisplayer::DisplayWorld(WorldRenderer* renderer)
	{
		if (renderer == nullptr)
		{
			GetWidgetProps().drawBackground = false;
			return;
		}

		m_worldRenderer					 = renderer;
		GetWidgetProps().drawBackground	 = true;
		GetWidgetProps().colorBackground = Color::White;
	}

	void WorldDisplayer::PreTick()
	{
		if (m_mouseConfined && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_1))
		{
			m_lgxWindow->FreeMouse();
			m_lgxWindow->SetMouseVisible(true);
			m_mouseConfined = false;
		}
	}

	void WorldDisplayer::Tick(float dt)
	{
		if (m_worldRenderer == nullptr)
			return;

		// Screen setup
		Screen& sc = m_worldRenderer->GetWorld()->GetScreen();
		sc.SetOwnerWindow(m_lgxWindow);
		sc.SetDisplaySize(m_rect.size);
		sc.SetDisplayPos(m_rect.pos);

		// Input setup
		const bool worldHasFocus = m_manager->GetControlsOwner() == this && m_lgxWindow->HasFocus();
		m_worldRenderer->GetWorld()->GetInput().SetIsActive(worldHasFocus);

		Texture* target				= m_worldRenderer->GetLightingPassOutput(Application::GetLGX()->GetCurrentFrameIndex());
		GetWidgetProps().rawTexture = target;
	}

	bool WorldDisplayer::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_isHovered && button == LINAGX_MOUSE_1 && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_manager->GrabControls(this);

			const LinaGX::LGXVector2ui center = {static_cast<uint32>(m_lgxWindow->GetMousePosition().x), static_cast<uint32>(m_lgxWindow->GetMousePosition().y)};
			m_lgxWindow->ConfineMouseToPoint(center);
			m_lgxWindow->SetMouseVisible(false);
			m_mouseConfined = true;
			return true;
		};

		if (m_isHovered)
		{
			m_manager->GrabControls(this);
			return true;
		}
		else
		{
			m_manager->ReleaseControls(this);
		}

		return false;
	}

} // namespace Lina::Editor
