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

#include "GUI/TopSection.hpp"
#include "FlarePackerCommon.hpp"

#include "Core/Application.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

#include "Common/GUI/Theme.hpp"
#include "Common/GUI/Widgets/WidgetManager.hpp"
#include "Common/GUI/Widgets/Layout/Row.hpp"
#include "Common/GUI/Widgets/Text/Text.hpp"
#include "Common/GUI/Widgets/Composite/WindowButtons.hpp"

namespace Lina
{

	void TopSection::Initialize(Application* app)
	{
		auto* rm	  = app->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_iconFont	  = rm->GetResource<Font>(FLP_ICON_FONT_SID);
		m_defaultFont = rm->GetResource<Font>(Theme::GetDefaultFontSID());
	}

	/*
		Widget* TopSection::BuildTitleRow(Widget* parent)
		{
			Row* titleRow = parent->Allocate<Row>();

			// Title text.
			Text* title		= titleRow->Allocate<Text>();
			title->contents = {
				.text  = "Flare Packer",
				.font  = m_defaultFont->GetLinaVGFont(parent->GetWindow()->GetDPIScale()),
				.color = Theme::GetColorForeground0(),
			};

			// Title icon
			Text* icon	   = titleRow->Allocate<Text>();
			icon->contents = {
				.text  = FLP_ICON_LINA_LOGO,
				.font  = m_iconFont->GetLinaVGFont(parent->GetWindow()->GetDPIScale()),
				.color = Theme::GetColorForeground0(),
			};

			const float indent = Theme::GetIndent(parent->GetWindow());

			// Sub-title row
			titleRow->contents = {
				.mainAlignment	= MainAlignment::Free,
				.crossAlignment = CrossAlignment::Center,
				.margins		= {.left = indent, .top = indent, .bottom = indent},
				.padding		= Theme::GetIndent(parent->GetWindow()),
				.widthFit		= Fit::FromChildren,
				.heightFit		= Fit::FromChildren,
			};
			titleRow->children = {icon, title};
			return titleRow;
		}

		Widget* TopSection::Draw(LinaGX::Window* window, int32 threadIndex)
		{
			Row* titleBar  = WidgetAllocator::Get().Allocate<Row>(threadIndex, window);
			titleBar->base = {
				.pos  = Vector2::Zero,
				.size = Vector2(static_cast<float>(window->GetSize().x), 0.0f),
			};

			titleBar->contents = {
				.mainAlignment	= MainAlignment::SpaceBetween,
				.crossAlignment = CrossAlignment::Center,
				.widthFit		= Fit::Fixed,
				.heightFit		= Fit::FromChildren,
				.background		= {.startColor = Theme::GetColorBackground0(), .endColor = Theme::GetColorBackground0()},
			};

			Row* windowButtons = titleBar->Allocate<Row>();



			titleBar->children = {BuildTitleRow(titleBar), windowButtons)};
			return titleBar;
		}

	 */
} // namespace Lina
