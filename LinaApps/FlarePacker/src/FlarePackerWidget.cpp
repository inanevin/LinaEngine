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

#include "FlarePackerWidget.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Theme.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

#include "Common/System/System.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

#include "FlarePackerCommon.hpp"

namespace Lina
{
	void FlarePackerWidget::Construct()
	{
		auto* rm = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		m_titleRow		= Allocate<DirectionalLayout>();
		Text* titleText = Allocate<Text>();
		Text* iconText	= Allocate<Text>();

		m_titleRow->SetProps({
			.padding = Theme::GetDef().baseIndent,
		});

		titleText->SetProps({
			.text = "Flare Packer",
			.font = rm->GetResource<Font>(Theme::GetDef().defaultFont),
		});

		iconText->SetProps({
			.text = FLP_ICON_LINA_LOGO,
			.font = rm->GetResource<Font>(FLP_ICON_FONT_SID),
		});

		m_titleRow->AddChild(iconText);
		m_titleRow->AddChild(titleText);
		AddChild(m_titleRow);
	}

	void FlarePackerWidget::Tick(float delta)
	{
		m_rect.size = Vector2(static_cast<float>(m_window->GetSize().x), static_cast<float>(m_window->GetSize().y));
		m_titleRow->SetPos(Vector2::Zero);
		Widget::Tick(delta);
	}

	void FlarePackerWidget::Draw(int32 threadIndex)
	{
		Widget::Draw(threadIndex);
	}

} // namespace Lina
