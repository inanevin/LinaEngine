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
#include "Core/Timer.hpp"
#include "imgui/imgui.h"

namespace LinaEditor
{
#define MAX_DEQUE_SIZE 100

    void ProfilerPanel::Setup()
    {

    }

    void ProfilerPanel::Draw(float frameTime)
    {
        if (m_show)
        {
            if (m_fpsDeque.size() == MAX_DEQUE_SIZE)
                m_fpsDeque.pop_front();

            int fps = LinaEngine::Application::GetApp().GetCurrentFPS();
            LINA_CLIENT_TRACE("{0}", fps);
            m_fpsDeque.push_back(fps);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
            ImGui::SetNextWindowBgAlpha(1.0f);
            WidgetsUtility::IncrementCursorPosY(12);

            const std::map<std::string, LinaEngine::Timer*>& map = LinaEngine::Timer::GetTimerMap();

            if (ImGui::Begin("Profiler", &m_show, flags))
            {
                // Shadow.
                WidgetsUtility::DrawShadowedLine(5);
                WidgetsUtility::IncrementCursorPosX(11);
                WidgetsUtility::IncrementCursorPosY(11);

                for (std::map<std::string, LinaEngine::Timer*>::const_iterator it = map.begin(); it != map.end(); ++it)
                {
                    WidgetsUtility::IncrementCursorPosX(12);
                    auto a = it->second->GetDuration();
                    std::string txt = it->first + " " + std::to_string(a) + " ms";
                    ImGui::Text(txt.c_str());
               
                }  

                WidgetsUtility::IncrementCursorPosX(12);
                WidgetsUtility::IncrementCursorPosY(12);

                static int values_offset = 0;
                values_offset = (values_offset + 1) % MAX_DEQUE_SIZE;
                ImGui::PlotLines("##fps", &m_fpsDeque[0], m_fpsDeque.size() * sizeof(float), values_offset, 0, -1.0f, 1.0f, ImVec2(0, 80.0f));
            }
            ImGui::End();

        }
    }
}