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

#include "Editor/Widgets/Panel/PanelPerformance.hpp"
#include "Editor/Widgets/Compound/TabRow.hpp"
#include "Editor/Widgets/Compound/Tab.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina::Editor
{

	void PanelPerformance::Construct()
	{
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		AddChild(layout);

		TabRow* tabRow = m_manager->Allocate<TabRow>("TabRow");
		tabRow->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		tabRow->SetAlignedPosX(0.0f);
		tabRow->SetAlignedSizeX(1.0f);
		tabRow->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		tabRow->SetCanCloseTabs(false);
		tabRow->GetProps().drawBackground = true;

		tabRow->GetProps().onSelectionChanged = [this](Widget* w) { SelectContent(w); };

		layout->AddChild(tabRow);

		DirectionalLayout* cpu = BuildContentLayout("CPU");
		BuildContentsCPU(cpu);

		DirectionalLayout* gpu = BuildContentLayout("GPU");
		BuildContentsGPU(gpu);

		DirectionalLayout* mem = BuildContentLayout(Locale::GetStr(LocaleStr::Memory));

		tabRow->AddTab(cpu);
		tabRow->AddTab(gpu);
		tabRow->AddTab(mem);
		m_tabContents.push_back(cpu);
		m_tabContents.push_back(gpu);
		m_tabContents.push_back(mem);
		m_layout = layout;
		m_tabRow = tabRow;
		SelectContent(0);
	}

	void PanelPerformance::Destruct()
	{
		for (auto* widget : m_tabContents)
		{
			if (widget == m_currentContent)
				continue;
			m_manager->Deallocate(widget);
		}
	}
	PanelLayoutExtra PanelPerformance::GetExtraLayoutData()
	{
		PanelLayoutExtra extra = {};
		extra.f[0]			   = static_cast<float>(UtilVector::IndexOf(m_tabContents, m_currentContent));
		return extra;
	}

	void PanelPerformance::SetExtraLayoutData(const PanelLayoutExtra& data)
	{
		SelectContent(static_cast<int32>(data.f[0]));
	}

	void PanelPerformance::Draw(int32 threadIndex)
	{
		Panel::Draw(threadIndex);
		Widget::Draw(threadIndex);
	}

	void PanelPerformance::SelectContent(Widget* w)
	{
		if (m_currentContent)
			m_layout->RemoveChild(m_currentContent);

		m_currentContent = w;
		m_layout->AddChild(m_currentContent);
		m_tabRow->SetSelected(w);
	}

	void PanelPerformance::SelectContent(int32 index)
	{
		SelectContent(m_tabContents[index]);
	}

	DirectionalLayout* PanelPerformance::BuildContentLayout(const String& name)
	{
		DirectionalLayout* content = m_manager->Allocate<DirectionalLayout>(name);
		content->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		content->SetAlignedPosX(0.0f);
		content->SetAlignedSize(Vector2(1.0f, 0.0f));
		content->GetProps().direction = DirectionOrientation::Vertical;
		content->GetChildMargins()	  = TBLR::Eq(Theme::GetDef().baseIndent);
		content->SetChildPadding(Theme::GetDef().baseIndent);
		return content;
	}

	void PanelPerformance::BuildContentsCPU(DirectionalLayout* parent)
	{
		Text* fps = m_manager->Allocate<Text>("FPS");
		fps->GetFlags().Set(WF_POS_ALIGN_X);
		fps->SetAlignedPosX(0.0f);
		fps->SetTickHook([fps](float delta) {
			fps->GetProps().text = "FPS: " + TO_STRING(SystemInfo::GetMeasuredFPS());
			fps->CalculateTextSize();
		});

		parent->AddChild(fps);

		Text* frameMS = m_manager->Allocate<Text>("FrameMS");
		frameMS->GetFlags().Set(WF_POS_ALIGN_X);
		frameMS->SetAlignedPosX(0.0f);
		frameMS->SetTickHook([frameMS](float delta) {
			frameMS->GetProps().text = Locale::GetStr(LocaleStr::FrameTime) + ": " + TO_STRING(SystemInfo::GetDeltaTimeF() * 1000.0f) + " (ms)";
			frameMS->CalculateTextSize();
		});

		parent->AddChild(frameMS);
	}

	void PanelPerformance::BuildContentsGPU(DirectionalLayout* parent)
	{
#ifdef LINA_DEBUG

		Text* dc = m_manager->Allocate<Text>("DrawCalls");
		dc->GetFlags().Set(WF_POS_ALIGN_X);
		dc->SetAlignedPosX(0.0f);
		dc->SetTickHook([dc](float delta) {
			uint32 totalDrawCalls = 0;
			uint32 totalTris	  = 0;

			const auto& drawCalls = Profiler::Get()->GetPrevFrame().drawCalls;

			for (const auto& [category, vec] : drawCalls)
			{
				totalDrawCalls += static_cast<uint32>(vec.size());

				for (const auto& call : vec)
					totalTris += call.tris;
			}

			dc->GetProps().text = Locale::GetStr(LocaleStr::DrawCalls) + ": " + TO_STRING(totalDrawCalls) + " (" + TO_STRING(totalTris) + "Tris)";
			dc->CalculateTextSize();
		});

		parent->AddChild(dc);
#endif
	}

	void PanelPerformance::BuildContentsMemory(DirectionalLayout* parent)
	{
	}
} // namespace Lina::Editor
