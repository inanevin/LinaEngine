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
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{
	Vector<PanelLog::LogLevelData> PanelLog::s_logLevels;

	void PanelLog::Construct()
	{
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

		Dropdown* logLevelDD = m_manager->Allocate<Dropdown>("LogLevel");
		logLevelDD->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_TOTAL_CHILDREN | WF_SIZE_ALIGN_Y);
		logLevelDD->SetAlignedSizeX(1.5f);
		logLevelDD->SetAlignedPosY(0.0f);
		logLevelDD->SetAlignedSizeY(1.0f);
		logLevelDD->GetText()->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::LogLevels));

		logLevelDD->GetProps().onAddItems = [this](Popup* popup) {
			const size_t sz = s_logLevels.size();
			for (size_t i = 0; i < sz; i++)
			{
				const LogLevelData& data = s_logLevels[i];
				popup->AddToggleItem(data.title, data.show, static_cast<int32>(i));
			}
		};

		logLevelDD->GetProps().onSelected = [this](int32 idx, String& newTitle) -> bool {
			s_logLevels[idx].show = !s_logLevels[idx].show;
			UpdateTextVisibility();
			return s_logLevels[idx].show;
		};
		topLayout->AddChild(logLevelDD);

		InputField* search = m_manager->Allocate<InputField>("Search");
		search->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		search->SetAlignedPosY(0.0f);
		search->SetAlignedSize(Vector2(0.0f, 1.0f));
		search->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
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

		DirectionalLayout* bottomLayout = m_manager->Allocate<DirectionalLayout>("BottomLayout");
		bottomLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		bottomLayout->SetAlignedPosX(0.0f);
		bottomLayout->SetAlignedSizeX(1.0f);
		bottomLayout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		bottomLayout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		panelLayout->AddChild(bottomLayout);
		AddChild(panelLayout);

		Button* clearButton = m_manager->Allocate<Button>("Clear");
		clearButton->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		clearButton->SetAlignedPosY(0.0f);
		clearButton->SetAlignedSizeY(1.0f);
		clearButton->RemoveText();
		clearButton->CreateIcon(ICON_TRASH);
		clearButton->GetProps().onClicked = [this]() {
			m_logLayout->DeallocAllChildren();
			m_logLayout->RemoveAllChildren();
			m_logScroll->CheckScroll();
			m_logScroll->ScrollToStart();
		};
		bottomLayout->AddChild(clearButton);

		InputField* console = m_manager->Allocate<InputField>("Console");
		console->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		console->SetAlignedPosY(0.0f);
		console->SetAlignedSize(Vector2(0.0f, 1.0f));
		console->GetProps().placeHolderText = Locale::GetStr(LocaleStr::EnterConsoleCommand);
		bottomLayout->AddChild(console);

		Log::AddLogListener(this);

		if (s_logLevels.empty())
		{
			s_logLevels.push_back({
				.level = LogLevel::Info,
				.show  = true,
				.color = Color::White,
				.title = Locale::GetStr(LocaleStr::Info),
			});

			s_logLevels.push_back({
				.level = LogLevel::Warn,
				.show  = true,
				.color = Theme::GetDef().accentWarn,
				.title = Locale::GetStr(LocaleStr::Warning),
			});

			s_logLevels.push_back({
				.level = LogLevel::Error,
				.show  = true,
				.color = Theme::GetDef().accentError,
				.title = Locale::GetStr(LocaleStr::Error),
			});

			s_logLevels.push_back({
				.level = LogLevel::Trace,
				.show  = false,
				.color = Theme::GetDef().accentSecondary,
				.title = Locale::GetStr(LocaleStr::Trace),
			});
		}
	}

	void PanelLog::Destruct()
	{
		Log::RemoveLogListener(this);
	}

	void PanelLog::PreTick()
	{
		LOCK_GUARD(Log::GetLogMutex());

		for (Widget* w : m_newLogs)
			m_logLayout->AddChildRequest(w);
		m_newLogs.clear();
	}

	void PanelLog::SaveLayoutToStream(OStream& stream)
	{
		const size_t sz = s_logLevels.size();
		for (size_t i = 0; i < sz; i++)
			stream << s_logLevels[i].show;
	}

	void PanelLog::LoadLayoutFromStream(IStream& stream)
	{
		const size_t sz = s_logLevels.size();
		for (size_t i = 0; i < sz; i++)
			stream >> s_logLevels[i].show;
	}

	void PanelLog::OnLog(LogLevel level, const char* msg)
	{
		const size_t sz	 = s_logLevels.size();
		size_t		 idx = 0;
		for (size_t i = 0; i < sz; i++)
		{
			const LogLevelData& data = s_logLevels[i];
			if (data.level == level)
			{
				idx = i;
				if (!data.show)
					return;
			}
		}

		if (!m_searchStr.empty())
		{
			const String lowerStr = UtilStr::ToLower(msg);
			if (lowerStr.find(m_searchStr) == String::npos)
				return;
		}

		LogLevelData& data = s_logLevels[idx];

		const String sysTime	 = "[" + FileSystem::GetSystemTimeStr() + "] ";
		const String levelHeader = sysTime + "[" + data.title + "] ";

		Text* text = m_manager->Allocate<Text>();
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
		const Vector<Widget*> texts = m_logLayout->GetChildren();

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

			text->GetFlags().Set(WF_HIDE, !targetLevel->show || searchStrNotOK);
		}
	}

} // namespace Lina::Editor
