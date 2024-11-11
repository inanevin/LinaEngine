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

#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"

namespace Lina::Editor
{
	void PanelResourceBrowser::Construct()
	{
		DirectionalLayout* vertical = m_manager->Allocate<DirectionalLayout>();
		vertical->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		vertical->GetProps().direction			= DirectionOrientation::Vertical;
		vertical->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		vertical->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		AddChild(vertical);

		DirectionalLayout* header = m_manager->Allocate<DirectionalLayout>();
		header->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		header->SetAlignedPosX(0.0f);
		header->SetAlignedSizeX(1.0f);
		header->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		header->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		vertical->AddChild(header);

		InputField* searchField = m_manager->Allocate<InputField>();
		searchField->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		searchField->SetAlignedPosY(0.0f);
		searchField->SetAlignedSize(Vector2(0.5f, 1.0f));
		searchField->GetProps().usePlaceHolder	= true;
		searchField->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
		header->AddChild(searchField);

		ResourceDirectoryBrowser* dirBrowser = m_manager->Allocate<ResourceDirectoryBrowser>();
		dirBrowser->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		dirBrowser->SetAlignedPosX(0.0f);
		dirBrowser->SetAlignedSize(Vector2(1.0f, 0.0f));
		vertical->AddChild(dirBrowser);
		m_resourceBrowser = dirBrowser;

		searchField->GetProps().onEdited = [dirBrowser](const String& str) {
			// TODO: Update search string for directory browser.
			dirBrowser->RefreshDirectory();
		};
	}

	void PanelResourceBrowser::SaveLayoutDefaults(OStream& stream)
	{
	}
} // namespace Lina::Editor
