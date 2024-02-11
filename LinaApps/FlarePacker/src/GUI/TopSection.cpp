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
#include "Common/GUI/WidgetAllocator.hpp"
#include "Common/GUI/Widgets/Layout/Row.hpp"
#include "Common/GUI/Widgets/Text/Text.hpp"

namespace Lina
{

	void TopSection::Initialize(Application* app)
	{
		auto* rm	  = app->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_iconFont	  = rm->GetResource<Font>(FLP_ICON_FONT_SID);
		m_defaultFont = rm->GetResource<Font>(Theme::GetDefaultFontSID());
	}

	Widget* TopSection::BuildTitleRow(Widget* parent)
	{
		Row* titleRow = parent->Allocate<Row>();

		// Title text.
		Text* title		= titleRow->Allocate<Text>();
		title->contents = {
			.text = "Flare Packer",
			.font = m_defaultFont->GetLinaVGFont(parent->GetWindow()->GetDPIScale()),
		};

		// Title icon
		Text* icon	   = titleRow->Allocate<Text>();
		icon->contents = {
			.text = FLP_ICON_LINA_LOGO,
			.font = m_iconFont->GetLinaVGFont(parent->GetWindow()->GetDPIScale()),
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
		auto drawRect = [](const Vector2& textPos, const Vector2& textSize) {
			LinaVG::StyleOptions style;
			style.color = LinaVG::Vec4(0, 0.5f, 0.0f, 1.0f);

			LinaVG::DrawRect(0, Vector2(textPos.x, textPos.y - textSize.y).AsLVG(), Vector2(textPos.x + textSize.x, textPos.y).AsLVG(), style);
		};

		float				 baselineY = 200.0f;
		LinaVG::StyleOptions lineOpts;
		lineOpts.color = LinaVG::Vec4(1, 0, 0, 1);
		LinaVG::DrawLine(0, Vector2(0, baselineY).AsLVG(), Vector2(window->GetSize().x, baselineY).AsLVG(), lineOpts);

		LinaVG::TextOptions textOpts;
		textOpts.font = m_defaultFont->GetLinaVGFont(window->GetDPIScale());

		Vector2 textPos	 = Vector2(60, baselineY);
		Vector2 textSize = LinaVG::CalculateTextSize(0, "A", textOpts);
		LinaVG::DrawTextNormal(0, "D", textPos.AsLVG(), textOpts);
		drawRect(textPos, textSize);

		LinaVG::SDFTextOptions sdf;
		sdf.font		= m_iconFont->GetLinaVGFont(window->GetDPIScale());
		sdf.sdfSoftness = 0.3f / static_cast<float>(sdf.font->m_size);

		Vector2 sdfPos	= Vector2(125, baselineY);
		Vector2 sdfSize = LinaVG::CalculateTextSize(0, "A", sdf);
		LinaVG::DrawTextSDF(0, "D", sdfPos.AsLVG(), sdf);
		drawRect(sdfPos, sdfSize);

		return nullptr;
		Row* titleBar  = WidgetAllocator::Get().Allocate<Row>(threadIndex, window);
		titleBar->base = {
			.pos  = Vector2::Zero,
			.size = Vector2(static_cast<float>(window->GetSize().x), 0.0f),
		};

		titleBar->contents = {
			.mainAlignment	= MainAlignment::SpaceBetween,
			.crossAlignment = CrossAlignment::Center,
			.widthFit		= Fit::Default,
			.heightFit		= Fit::FromChildren,
		};
		titleBar->children = {BuildTitleRow(titleBar)};
		return titleBar;
	}
} // namespace Lina
