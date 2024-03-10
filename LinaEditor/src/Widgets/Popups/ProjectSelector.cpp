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

#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/Compound/VerticalIconTabs.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Editor/CommonEditor.hpp"

namespace Lina::Editor
{
	void ProjectSelector::Construct()
	{
		m_iconTabs						= Allocate<VerticalIconTabs>("VerticalIconTabs");
		m_iconTabs->GetProps().icons	= {ICON_CHECK, ICON_CHECK};
		m_iconTabs->GetProps().selected = 0;
		AddChild(m_iconTabs);

		m_monitorSize = Vector2(static_cast<float>(m_lgxWindow->GetMonitorSize().x), static_cast<float>(m_lgxWindow->GetMonitorSize().y));
	}

	void ProjectSelector::Tick(float delta)
	{
		SetSize(Vector2(m_monitorSize.x * 0.15f, m_monitorSize.x * 0.05f));
		SetPos(Vector2(static_cast<float>(m_lgxWindow->GetSize().x) * 0.5f - GetHalfSizeX(), static_cast<float>(m_lgxWindow->GetSize().y) * 0.5f - GetHalfSizeY()));
		Widget::SetIsHovered();

		m_iconTabs->SetDrawOrder(m_drawOrder + 1);
		m_iconTabs->SetPos(m_rect.pos);
		m_iconTabs->SetSizeX(GetSizeX() * 0.1f);
		m_iconTabs->SetSizeY(GetSizeY());
		m_iconTabs->Tick(delta);
	}

	void ProjectSelector::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		m_iconTabs->Draw(threadIndex);
	}

	bool ProjectSelector::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		return true;
	}
} // namespace Lina::Editor
