#include "Panels/ProfilerPanel.hpp"

#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/WidgetsUtility.hpp"
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

#define MS_DISPLAY_TIME        .1
#define CURSORPOS_X_LABELS     12
#define CURSORPOS_XPERC_VALUES 0.30f

    Map<String, String> m_timerMSStorage;

    void ProfilerPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_show = false;
    }

    void ProfilerPanel::Draw()
    {
        if (m_show)
        {
            float                                cursorPosValues = ImGui::GetWindowSize().x * CURSORPOS_XPERC_VALUES;
            float                                cursorPosLabels = CURSORPOS_X_LABELS;
          // const HashMap<String, Time*>& map             = Time::GetTimerMap();
          //
          // if (Begin())
          // {
          //
          //     // Shadow.
          //     WidgetsUtility::IncrementCursorPosY(11);
          //
          //     float currentTime = (float)Time::GetElapsedTime();
          //
          //     bool displayMS = false;
          //     if (currentTime > m_lastMSDisplayTime + MS_DISPLAY_TIME)
          //     {
          //         m_lastMSDisplayTime = currentTime;
          //         displayMS           = true;
          //     }
          //
          //     WidgetsUtility::IncrementCursorPosX(12);
          //
          //     double            updateTime    = Engine::Get()->GetUpdateTime();
          //     const String updateTimeStr = "Update Time: " + TO_STRING(updateTime);
          //     WidgetsUtility::PropertyLabel(updateTimeStr.c_str());
          //
          //     double            renderTime    = Engine::Get()->GetRenderTime();
          //     const String renderTimeStr = "Render Time: " + TO_STRING(renderTime);
          //     WidgetsUtility::PropertyLabel(renderTimeStr.c_str());
          //
          //     double            frameTime    = Engine::Get()->GetFrameTime();
          //     const String frameTimeStr = "Frame Time: " + TO_STRING(frameTime);
          //     WidgetsUtility::PropertyLabel(frameTimeStr.c_str());
          //
          //     for (HashMap<String, Time*>::const_iterator it = map.begin(); it != map.end(); ++it)
          //     {
          //         String txt = "";
          //         txt             = it->first + " " + m_timerMSStorage[it->first] + " ms";
          //
          //         if (displayMS)
          //             m_timerMSStorage[it->first] = TO_STRING(it->second->GetDuration());
          //
          //         WidgetsUtility::IncrementCursorPosX(12);
          //         ImGui::Text(txt.c_str());
          //     }
          //
          //     displayMS = false;
          //
          //     WidgetsUtility::IncrementCursorPosX(12);
          //     WidgetsUtility::IncrementCursorPosY(12);
          //
          //     int   fps         = Engine::Get()->GetCurrentFPS();
          //     int   ups         = Engine::Get()->GetCurrentUPS();
          //     float rawDelta    = (float)Engine::Get()->GetRawDelta();
          //     float smoothDelta = (float)Engine::Get()->GetSmoothDelta();
          //
          //     static RollingBuffer fpsData;
          //     static RollingBuffer upsData;
          //     static RollingBuffer rawDeltaData;
          //     static RollingBuffer smoothDeltaData;
          //
          //     static float t = 0;
          //     t += ImGui::GetIO().DeltaTime;
          //     fpsData.AddPoint(t, (float)fps);
          //     upsData.AddPoint(t, (float)ups);
          //     rawDeltaData.AddPoint(t, rawDelta);
          //     smoothDeltaData.AddPoint(t, smoothDelta);
          //
          //     static float           history = 10.0f;
          //     static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels;
          //
          //     ImPlot::SetNextPlotLimitsX(0, history, ImGuiCond_Always);
          //     // ImPlot::SetNextPlotLimitsY(0, 500, ImGuiCond_Always);
          //
          //     if (ImPlot::BeginPlot("##fpsUPS", NULL, NULL, ImVec2(-1, 115), 0, rt_axis, rt_axis))
          //     {
          //
          //         String fpsLabel = "FPS " + TO_STRING(fps);
          //         String upsLabel = "UPS " + TO_STRING(ups);
          //         ImPlot::PushStyleColor(ImPlotCol_Line, ImGui::GetStyleColorVec4(ImGuiCol_Header));
          //         ImPlot::PlotLine(fpsLabel.c_str(), &fpsData.m_data[0].x, &fpsData.m_data[0].y, fpsData.m_data.size(), 0, 2 * sizeof(float));
          //         ImPlot::PopStyleColor();
          //
          //         ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 1.0, 0.0, 1.0));
          //         ImPlot::PlotLine(upsLabel.c_str(), &upsData.m_data[0].x, &upsData.m_data[0].y, upsData.m_data.size(), 0, 2 * sizeof(float));
          //         ImPlot::PopStyleColor();
          //
          //         ImPlot::EndPlot();
          //     }
          //
          //     WidgetsUtility::IncrementCursorPosX(12);
          //     WidgetsUtility::IncrementCursorPosY(12);
          //     static float deltaHistory   = 10.0f;
          //     static float deltaYLimitMin = 0.0f;
          //     static float deltaYLimitMax = 0.025f;
          //
          //     ImPlot::SetNextPlotLimitsX(0, deltaHistory, ImGuiCond_Always);
          //     // ImPlot::SetNextPlotLimitsY(deltaYLimitMin, deltaYLimitMax, ImGuiCond_Always);
          //
          //     if (ImPlot::BeginPlot("##deltas", NULL, NULL, ImVec2(-1, 115), 0, rt_axis, rt_axis))
          //     {
          //
          //         String rawDeltaLabel    = "Raw DT " + TO_STRING(rawDelta);
          //         String smoothDeltaLabel = "Smooth DT " + TO_STRING(smoothDelta);
          //         ImPlot::PushStyleColor(ImPlotCol_Line, ImGui::GetStyleColorVec4(ImGuiCol_Header));
          //         ImPlot::PlotLine(rawDeltaLabel.c_str(), &rawDeltaData.m_data[0].x, &rawDeltaData.m_data[0].y, rawDeltaData.m_data.size(), 0, 2 * sizeof(float));
          //         ImPlot::PopStyleColor();
          //         ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 1.0, 0.0, 1.0));
          //         ImPlot::PlotLine(smoothDeltaLabel.c_str(), &smoothDeltaData.m_data[0].x, &smoothDeltaData.m_data[0].y, smoothDeltaData.m_data.size(), 0, 2 * sizeof(float));
          //         ImPlot::PopStyleColor();
          //
          //         ImPlot::EndPlot();
          //     }
          //     End();
          // }
        }
    }
} // namespace Lina::Editor
