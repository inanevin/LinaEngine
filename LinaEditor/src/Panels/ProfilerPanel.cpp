/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Panels/ProfilerPanel.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/Timer.hpp"
#include "imgui/imgui.h"
#include "imgui/implot/implot.h"

namespace Lina::Editor
{

	struct RollingBuffer
	{
		float m_span;

		ImVector<ImVec2> m_data;

		RollingBuffer()
		{
			m_span = 10.0f;
			m_data.reserve(2000);
		}

		void AddPoint(float x, float y)
		{
			float xmod = fmodf(x, m_span);
			if (!m_data.empty() && xmod < m_data.back().x)
				m_data.shrink(0);
			m_data.push_back(ImVec2(xmod, y));
		}
	};



#define MS_DISPLAY_TIME .1
#define CURSORPOS_X_LABELS 12
#define CURSORPOS_XPERC_VALUES 0.30f


	std::map<std::string, std::string> m_timerMSStorage;

	void ProfilerPanel::Initialize(const char* id)
	{
		EditorPanel::Initialize(id);
		m_show = false;
	}

	void ProfilerPanel::Draw()
	{
		if (m_show)
		{
			float cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
			float cursorPosLabels = CURSORPOS_X_LABELS;
			const std::map<std::string, Lina::Timer*>& map = Lina::Timer::GetTimerMap();

			ImGui::Begin(m_id, NULL, m_windowFlags);
			WidgetsUtility::WindowTitlebar(m_id);
			if (!CanDrawContent()) return;


			// Shadow.
			WidgetsUtility::IncrementCursorPosY(11);


			float currentTime = Engine::Get()->GetElapsedTime();

			bool displayMS = false;
			if (currentTime > m_lastMSDisplayTime + MS_DISPLAY_TIME)
			{
				m_lastMSDisplayTime = currentTime;
				displayMS = true;
			}

			WidgetsUtility::IncrementCursorPosX(12);

			double updateTime = Lina::Engine::Get()->GetUpdateTime();
			const std::string updateTimeStr = "Update Time: " + std::to_string(updateTime);
			WidgetsUtility::PropertyLabel(updateTimeStr.c_str());

			double renderTime = Lina::Engine::Get()->GetRenderTime();
			const std::string renderTimeStr = "Render Time: " + std::to_string(renderTime);
			WidgetsUtility::PropertyLabel(renderTimeStr.c_str());

			double frameTime = Lina::Engine::Get()->GetFrameTime();
			const std::string frameTimeStr = "Frame Time: " + std::to_string(frameTime);
			WidgetsUtility::PropertyLabel(frameTimeStr.c_str());



			for (std::map<std::string, Lina::Timer*>::const_iterator it = map.begin(); it != map.end(); ++it)
			{
				std::string txt = "";
				txt = it->first + " " + m_timerMSStorage[it->first] + " ms";

				if (displayMS)
					m_timerMSStorage[it->first] = std::to_string(it->second->GetDuration());

				WidgetsUtility::IncrementCursorPosX(12);
				ImGui::Text(txt.c_str());
			}

			displayMS = false;

			WidgetsUtility::IncrementCursorPosX(12);
			WidgetsUtility::IncrementCursorPosY(12);

			int fps = Lina::Engine::Get()->GetCurrentFPS();
			int ups = Lina::Engine::Get()->GetCurrentUPS();
			float rawDelta = Lina::Engine::Get()->GetRawDelta();
			float smoothDelta = Lina::Engine::Get()->GetSmoothDelta();

			static RollingBuffer fpsData;
			static RollingBuffer upsData;
			static RollingBuffer rawDeltaData;
			static RollingBuffer smoothDeltaData;

			static float t = 0;
			t += ImGui::GetIO().DeltaTime;
			fpsData.AddPoint(t, (float)fps);
			upsData.AddPoint(t, (float)ups);
			rawDeltaData.AddPoint(t, rawDelta);
			smoothDeltaData.AddPoint(t, smoothDelta);


			static float history = 10.0f;
			static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels;

			ImPlot::SetNextPlotLimitsX(0, history, ImGuiCond_Always);
			//ImPlot::SetNextPlotLimitsY(0, 500, ImGuiCond_Always);

			if (ImPlot::BeginPlot("##fpsUPS", NULL, NULL, ImVec2(-1, 115), 0, rt_axis, rt_axis)) {

				std::string fpsLabel = "FPS " + std::to_string(fps);
				std::string upsLabel = "UPS " + std::to_string(ups);
				ImPlot::PushStyleColor(ImPlotCol_Line, ImGui::GetStyleColorVec4(ImGuiCol_Header));
				ImPlot::PlotLine(fpsLabel.c_str(), &fpsData.m_data[0].x, &fpsData.m_data[0].y, fpsData.m_data.size(), 0, 2 * sizeof(float));
				ImPlot::PopStyleColor();

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 1.0, 0.0, 1.0));
				ImPlot::PlotLine(upsLabel.c_str(), &upsData.m_data[0].x, &upsData.m_data[0].y, upsData.m_data.size(), 0, 2 * sizeof(float));
				ImPlot::PopStyleColor();

				ImPlot::EndPlot();

			}

			WidgetsUtility::IncrementCursorPosX(12);
			WidgetsUtility::IncrementCursorPosY(12);
			static float deltaHistory = 10.0f;
			static float deltaYLimitMin = 0.0f;
			static float deltaYLimitMax = 0.025f;

		
			ImPlot::SetNextPlotLimitsX(0, deltaHistory, ImGuiCond_Always);
			//ImPlot::SetNextPlotLimitsY(deltaYLimitMin, deltaYLimitMax, ImGuiCond_Always);
		
			if (ImPlot::BeginPlot("##deltas", NULL, NULL, ImVec2(-1, 115), 0, rt_axis, rt_axis)) {

				std::string rawDeltaLabel = "Raw DT " + std::to_string(rawDelta);
				std::string smoothDeltaLabel = "Smooth DT " + std::to_string(smoothDelta);
				ImPlot::PushStyleColor(ImPlotCol_Line, ImGui::GetStyleColorVec4(ImGuiCol_Header));
				ImPlot::PlotLine(rawDeltaLabel.c_str(), &rawDeltaData.m_data[0].x, &rawDeltaData.m_data[0].y, rawDeltaData.m_data.size(), 0, 2 * sizeof(float));
				ImPlot::PopStyleColor();
				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 1.0, 0.0, 1.0));
				ImPlot::PlotLine(smoothDeltaLabel.c_str(), &smoothDeltaData.m_data[0].x, &smoothDeltaData.m_data[0].y, smoothDeltaData.m_data.size(), 0, 2 * sizeof(float));
				ImPlot::PopStyleColor();

				ImPlot::EndPlot();

			}
			ImGui::End();

		}
	}
}