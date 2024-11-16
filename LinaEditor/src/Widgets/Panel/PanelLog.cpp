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

#include "Editor/Widgets/Panel/PanelLog.hpp"
#include "Editor/EditorLocale.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Editor.hpp"

namespace Lina::Editor
{
#define MAX_LOGS 180

	void PanelLog::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* panelLayout = m_manager->Allocate<DirectionalLayout>("PanelLayout");
		panelLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		panelLayout->SetAlignedPos(Vector2::Zero);
		panelLayout->SetAlignedSize(Vector2::One);
		panelLayout->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		panelLayout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		panelLayout->GetProps().direction		   = DirectionOrientation::Vertical;

		DirectionalLayout* topLayout = m_manager->Allocate<DirectionalLayout>("TopLayout");
		topLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		topLayout->SetAlignedPosX(0.0f);
		topLayout->SetAlignedSizeX(1.0f);
		topLayout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		topLayout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		panelLayout->AddChild(topLayout);

		Button* clearButton				  = CommonWidgets::BuildIconButton(this, ICON_TRASH);
		clearButton->GetProps().onClicked = [this]() {
			m_logLayout->DeallocAllChildren();
			m_logLayout->RemoveAllChildren();
		};
		topLayout->AddChild(clearButton);

		Dropdown* logLevelDD = m_manager->Allocate<Dropdown>("LogLevel");
		logLevelDD->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		logLevelDD->SetFixedSizeX(Theme::GetDef().baseItemWidth);
		logLevelDD->SetAlignedPosY(0.0f);
		logLevelDD->SetAlignedSizeY(1.0f);
		logLevelDD->GetText()->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::LogLevels));

		logLevelDD->GetProps().onAddItems = [this](Popup* popup) {
			const size_t sz = m_logLevels.size();
			for (size_t i = 0; i < sz; i++)
			{
				const LogLevelData& data = m_logLevels[i];

				const bool show = m_editor->GetSettings().GetSettingsPanelLog().logLevelMask.IsSet(data.mask);
				popup->AddToggleItem(data.title, show, static_cast<int32>(i));
			}
		};

		logLevelDD->GetProps().onSelected = [this](int32 idx, String& newTitle) -> bool {
			const LogLevelData& data = m_logLevels[idx];

			const bool show = m_editor->GetSettings().GetSettingsPanelLog().logLevelMask.IsSet(data.mask);
			m_editor->GetSettings().GetSettingsPanelLog().logLevelMask.Set(data.mask, !show);
			m_editor->SaveSettings();

			UpdateTextVisibility();
			m_logScroll->ScrollToEnd();
			return !show;
		};
		topLayout->AddChild(logLevelDD);

		InputField* search = m_manager->Allocate<InputField>("Search");
		search->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		search->SetAlignedPosY(0.0f);
		search->SetAlignedSize(Vector2(0.0f, 1.0f));
		search->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
		search->GetProps().placeHolderIcon = ICON_SEARCH;
		search->GetProps().onEdited		   = [this](const String& str) {
			   m_searchStr = UtilStr::ToLower(str);
			   UpdateTextVisibility();
		};

		topLayout->AddChild(search);

		ScrollArea* logScroll = m_manager->Allocate<ScrollArea>("LogScroll");
		logScroll->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		logScroll->SetAlignedPosX(0.0f);
		logScroll->SetAlignedSize(Vector2(1.0f, 0.0f));
		logScroll->GetProps().direction					 = DirectionOrientation::Vertical;
		logScroll->GetProps().tryKeepAtEnd				 = true;
		logScroll->GetWidgetProps().drawBackground		 = true;
		logScroll->GetWidgetProps().colorBackground		 = Theme::GetDef().background0;
		logScroll->GetWidgetProps().rounding			 = 0.0f;
		logScroll->GetWidgetProps().outlineThickness	 = 0.0f;
		logScroll->GetWidgetProps().dropshadow.enabled	 = true;
		logScroll->GetWidgetProps().dropshadow.isInner	 = true;
		logScroll->GetWidgetProps().dropshadow.direction = Direction::Top;
		logScroll->GetWidgetProps().dropshadow.steps	 = Theme::GetDef().baseDropShadowSteps;
		panelLayout->AddChild(logScroll);
		m_logScroll = logScroll;

		DirectionalLayout* logArea = m_manager->Allocate<DirectionalLayout>("LogArea");
		logArea->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		logArea->SetAlignedPos(Vector2::Zero);
		logArea->SetAlignedSize(Vector2::One);
		logArea->GetProps().direction		   = DirectionOrientation::Vertical;
		logArea->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		logArea->GetWidgetProps().childPadding = Theme::GetDef().baseIndentInner;
		logArea->GetWidgetProps().clipChildren = true;
		logScroll->AddChild(logArea);
		m_logLayout = logArea;

		AddChild(panelLayout);

		Log::AddLogListener(this);

		if (m_logLevels.empty())
		{
			m_logLevels.push_back({
				.mask  = LOG_LEVEL_INFO,
				.color = Color::White,
				.title = Locale::GetStr(LocaleStr::Info),
			});

			m_logLevels.push_back({
				.mask  = LOG_LEVEL_WARNING,
				.color = Theme::GetDef().accentWarn,
				.title = Locale::GetStr(LocaleStr::Warning),
			});

			m_logLevels.push_back({
				.mask  = LOG_LEVEL_ERROR,
				.color = Theme::GetDef().accentError,
				.title = Locale::GetStr(LocaleStr::Error),
			});

			m_logLevels.push_back({
				.mask  = LOG_LEVEL_TRACE,
				.color = Theme::GetDef().accentSecondary,
				.title = Locale::GetStr(LocaleStr::Trace),
			});
		}
	}

	void PanelLog::Destruct()
	{
		for (Widget* w : m_newLogs)
			m_manager->Deallocate(w);
		m_newLogs.clear();
		Log::RemoveLogListener(this);
	}

	void PanelLog::PreTick()
	{
		LOCK_GUARD(Log::GetLogMutex());

		uint32 childSz = m_logLayout->GetChildren().size();

		for (Widget* w : m_newLogs)
		{
			m_logLayout->AddChildRequest(w);
			childSz++;
			if (childSz > MAX_LOGS)
				m_manager->AddToKillList(m_logLayout->GetChildren().at(0));
		}
		m_newLogs.clear();
	}

	void PanelLog::OnLog(LogLevel level, const char* msg)
	{
		const Bitmask32& currentMask = m_editor->GetSettings().GetSettingsPanelLog().logLevelMask;

		const size_t sz	 = m_logLevels.size();
		size_t		 idx = 0;
		for (size_t i = 0; i < sz; i++)
		{
			const LogLevelData& data = m_logLevels[i];
			if (data.mask == level)
			{
				idx				= i;
				const bool show = currentMask.IsSet(data.mask);

				if (!show)
					return;
			}
		}

		if (!m_searchStr.empty())
		{
			const String lowerStr = UtilStr::ToLower(msg);
			if (lowerStr.find(m_searchStr) == String::npos)
				return;
		}

		LogLevelData& data = m_logLevels[idx];

		const String sysTime	 = "[" + FileSystem::GetSystemTimeStr() + "] ";
		const String levelHeader = sysTime + "[" + data.title + "] ";

		Text* text				   = m_manager->Allocate<Text>();
		text->GetProps().isDynamic = true;
		text->GetFlags().Set(WF_POS_ALIGN_X);
		text->SetAlignedPosX(0.0f);
		text->UpdateTextAndCalcSize(levelHeader + msg);
		text->GetProps().color = data.color;
		text->SetUserData(&data);
		text->GetProps().fetchWrapFromParent = true;
		m_newLogs.push_back(text);
	}

	void PanelLog::UpdateTextVisibility()
	{
		const Vector<Widget*> texts		  = m_logLayout->GetChildren();
		const Bitmask32&	  currentMask = m_editor->GetSettings().GetSettingsPanelLog().logLevelMask;

		for (Widget* text : texts)
		{
			LogLevelData* targetLevel = static_cast<LogLevelData*>(text->GetUserData());

			bool searchStrNotOK = false;

			if (!m_searchStr.empty())
			{
				const String& str	   = static_cast<Text*>(text)->GetProps().text;
				const String  strLower = UtilStr::ToLower(str);
				searchStrNotOK		   = strLower.find(m_searchStr) == String::npos;
			}

			text->GetFlags().Set(WF_HIDE, !currentMask.IsSet(targetLevel->mask) || searchStrNotOK);
		}
	}

} // namespace Lina::Editor
