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
#include "Core/Graphics/GfxManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{

	void WorldDisplayer::Construct()
	{
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

	void WorldDisplayer::Tick(float dt)
	{
		if (m_worldRenderer == nullptr)
			return;

		Screen& sc = m_worldRenderer->GetWorld()->GetScreen();
		sc.SetOwnerWindow(m_lgxWindow);
		sc.SetDisplaySize(m_rect.size);

		m_worldRenderer->GetWorld()->GetFlags().Set(WORLD_FLAGS_UNFOCUSED, m_manager->GetControlsOwner() == this);

		Texture* target = m_worldRenderer->GetLightingPassOutput(Editor::Get()->GetGfxManager()->GetLGX()->GetCurrentFrameIndex());

		GetWidgetProps().rawTexture = target;
	}

	bool WorldDisplayer::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_manager->GrabControls(this);
			return true;
		}

		return false;
	}

} // namespace Lina::Editor
