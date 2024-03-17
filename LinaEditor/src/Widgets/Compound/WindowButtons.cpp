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

#include "Editor/Widgets/Compound/WindowButtons.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/System/System.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{
	void WindowButtons::Construct()
	{
		Button* min	  = Allocate<Button>();
		Button* max	  = Allocate<Button>();
		Button* close = Allocate<Button>();

		min->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		min->GetText()->GetProps().text		 = ICON_MINIMIZE;
		min->GetText()->GetProps().textScale = 0.5f;
		min->GetProps().colorDefaultStart	 = Theme::GetDef().black;
		min->GetProps().colorDefaultEnd		 = Theme::GetDef().black;
		min->GetProps().colorHovered		 = Theme::GetDef().background4;
		min->GetProps().colorPressed		 = Theme::GetDef().background0;
		min->GetProps().rounding			 = 0.0f;
		min->GetProps().outlineThickness	 = 0.0f;

		min->GetProps().onClicked = [this]() { m_lgxWindow->Minimize(); };

		max->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		max->GetText()->GetProps().text		 = m_lgxWindow->GetIsMaximized() ? ICON_RESTORE : ICON_MAXIMIZE;
		max->GetText()->GetProps().textScale = 0.5f;
		max->GetProps().colorDefaultStart	 = Theme::GetDef().black;
		max->GetProps().colorDefaultEnd		 = Theme::GetDef().black;
		max->GetProps().colorHovered		 = Theme::GetDef().background4;
		max->GetProps().colorPressed		 = Theme::GetDef().background0;
		max->GetProps().rounding			 = 0.0f;
		max->GetProps().outlineThickness	 = 0.0f;
		max->GetProps().onClicked			 = [this, max]() {
			   if (m_lgxWindow->GetIsMaximized())
				   m_lgxWindow->Restore();
			   else
				   m_lgxWindow->Maximize();

			   max->GetText()->GetProps().text = m_lgxWindow->GetIsMaximized() ? ICON_RESTORE : ICON_MAXIMIZE;
			   max->GetText()->CalculateTextSize();
		};

		close->GetText()->GetProps().font	   = Theme::GetDef().iconFont;
		close->GetText()->GetProps().text	   = ICON_XMARK;
		close->GetText()->GetProps().textScale = 0.5f;
		close->GetProps().colorDefaultStart	   = Theme::GetDef().accentPrimary0;
		close->GetProps().colorDefaultEnd	   = Theme::GetDef().accentPrimary0;
		close->GetProps().colorHovered		   = Theme::GetDef().accentPrimary2;
		close->GetProps().colorPressed		   = Theme::GetDef().accentPrimary1;
		close->GetProps().rounding			   = 0.0f;
		close->GetProps().outlineThickness	   = 0.0f;
		close->GetProps().onClicked			   = [this]() {
			   if (m_lgxWindow->GetSID() == LINA_MAIN_SWAPCHAIN)
			   {
				   m_system->CastSubsystem<Editor>(SubsystemType::Editor)->RequestExit();
			   }
			   else
				   m_lgxWindow->Close();
		};

		AddChild(min, max, close);
	}

	void WindowButtons::Tick(float delta)
	{
		const float childWidth = GetSizeX() / 3.0f;

		float x = m_rect.pos.x;

		for (auto* c : m_children)
		{
			c->SetSizeX(childWidth);
			c->SetSizeY(GetSizeY());
			c->SetPos(Vector2(x, m_rect.pos.y));
			x += childWidth;
		}
	}

} // namespace Lina::Editor
