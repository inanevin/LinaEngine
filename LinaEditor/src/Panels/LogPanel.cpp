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

#include "Panels/LogPanel.hpp"

#include "Core/Application.hpp"
#include "Core/GUILayer.hpp"
#include "Core/InputMappings.hpp"
#include "Core/WindowBackend.hpp"
#include "IconsFontAwesome5.h"
#include "Log/Log.hpp"
#include "Utility/EditorUtility.hpp"
#include "Widgets/WidgetsUtility.hpp"

#define MAX_BACKTRACE_SIZE 32

namespace Lina::Editor
{
    LogPanel::~LogPanel(){};

    void LogPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);

        // We set our dispatcher & subscribe in order to receive log events.
        Event::EventSystem::Get()->Connect<Event::ELog, &LogPanel::OnLog>(this);

        // Add icon buttons.
        // m_logLevelIconButtons.push_back(LogLevelIconButton("ll_debug", "Debug", ICON_FA_BUG, LogLevel::Debug, LOGPANEL_COLOR_DEBUG_DEFAULT, LOGPANEL_COLOR_DEBUG_HOVERED, LOGPANEL_COLOR_DEBUG_PRESSED));
        // m_logLevelIconButtons.push_back(LogLevelIconButton("ll_info", "Info", ICON_FA_INFO_CIRCLE, LogLevel::Info, LOGPANEL_COLOR_INFO_DEFAULT, LOGPANEL_COLOR_INFO_HOVERED, LOGPANEL_COLOR_INFO_PRESSED));
        // m_logLevelIconButtons.push_back(LogLevelIconButton("ll_trace", "Trace", ICON_FA_CLIPBOARD_LIST, LogLevel::Trace, LOGPANEL_COLOR_TRACE_DEFAULT, LOGPANEL_COLOR_TRACE_HOVERED, LOGPANEL_COLOR_TRACE_PRESSED));
        // m_logLevelIconButtons.push_back(LogLevelIconButton("ll_warn", "Warn", ICON_FA_EXCLAMATION_TRIANGLE, LogLevel::Warn, LOGPANEL_COLOR_WARN_DEFAULT, LOGPANEL_COLOR_WARN_HOVERED, LOGPANEL_COLOR_WARN_PRESSED));
        // m_logLevelIconButtons.push_back(LogLevelIconButton("ll_error", "Error", ICON_FA_TIMES_CIRCLE, LogLevel::Error, LOGPANEL_COLOR_ERR_DEFAULT, LOGPANEL_COLOR_ERR_HOVERED, LOGPANEL_COLOR_ERR_PRESSED));
        // m_logLevelIconButtons.push_back(LogLevelIconButton("ll_critical", "Critical", ICON_FA_SKULL_CROSSBONES, LogLevel::Critical, LOGPANEL_COLOR_CRIT_DEFAULT, LOGPANEL_COLOR_CRIT_HOVERED, LOGPANEL_COLOR_CRIT_PRESSED));
        //
        // // To be retrieved from editor settings file later on.
        // m_logLevelFlags = LogLevel::Critical | LogLevel::Debug | LogLevel::Trace | LogLevel::Info | LogLevel::Error | LogLevel::Warn;

        // Update icon colors depending on the chosen log levels
        for (int i = 0; i < m_logLevelIconButtons.size(); i++)
            m_logLevelIconButtons[i].UpdateColors(&m_logLevelFlags);
    }

    void LogPanel::Draw()
    {
        return;
        if (m_show)
        {
            if (Begin())
            {

                // Shadow.
                WidgetsUtility::IncrementCursorPosX(11);
                WidgetsUtility::IncrementCursorPosY(11);

                // Empty dump
                if (ImGui::Button("Clear", ImVec2(50, 29)))
                {
                    m_logDeque.clear();
                }
                ImGui::SameLine();

                WidgetsUtility::IncrementCursorPosX(3);

                // Save dump contents on a file.
                if (ImGui::Button("Save", ImVec2(50, 28)))
                {
                    std::string fullPath = "";
                    fullPath             = EditorUtility::SaveFile(".txt", Graphics::WindowBackend::Get()->GetNativeWindow());

                    if (fullPath.compare("") != 0)
                    {
                        size_t      lastIndex  = fullPath.find_last_of("/");
                        std::string folderPath = fullPath.substr(0, lastIndex);
                        std::string fileName   = fullPath.substr(lastIndex + 1);
                        // save later.
                    }
                }

                // Draw icon buttons.
                for (int i = 0; i < m_logLevelIconButtons.size(); i++)
                {
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 200 + 35 * i);
                    m_logLevelIconButtons[i].DrawButton(&m_logLevelFlags);
                }

                WidgetsUtility::IncrementCursorPosY(4);
                WidgetsUtility::IncrementCursorPosY(4);
                // Draw dump contents.
                if (ImGui::BeginChild("Log_", ImVec2(0, 0), false))
                {
                    WidgetsUtility::IncrementCursorPosY(11);
                    for (std::deque<LogDumpEntry>::iterator it = m_logDeque.begin(); it != m_logDeque.end(); it++)
                    {
                        //if (!(m_logLevelFlags & it->m_dump.m_level))
                        //    continue;

                        // ImGui::PushTextWrapPos(ImGui::GetWindowContentRegionMax().x - 5);

                        // Draw the level icon.
                        // WidgetsUtility::IncrementCursorPos(ImVec2(15, 0)); // for icons
                        //   WidgetsUtility::IncrementCursorPos(ImVec2(5, 0));

                        //// Draw the icon depending on the log level type also set the text color ready.
                        // if (it->m_dump.m_level == LogLevel::Critical)
                        //{
                        //	if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_SKULL_CROSSBONES, 0.6f, LOGPANEL_COLOR_CRIT_DEFAULT);
                        //	ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_CRIT_DEFAULT);
                        //}
                        // else if (it->m_dump.m_level == LogLevel::Debug)
                        //{
                        //	if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_BUG, 0.6f, LOGPANEL_COLOR_DEBUG_DEFAULT);
                        //	ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_DEBUG_DEFAULT);
                        //}
                        // else if (it->m_dump.m_level == LogLevel::Error)
                        //{
                        //	if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_TIMES_CIRCLE, 0.6f, LOGPANEL_COLOR_ERR_DEFAULT);
                        //	ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_ERR_DEFAULT);
                        //}
                        // else if (it->m_dump.m_level == LogLevel::Info)
                        //{
                        //	if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_INFO_CIRCLE, 0.6f, LOGPANEL_COLOR_INFO_DEFAULT);
                        //	ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_INFO_DEFAULT);
                        //}
                        // else if (it->m_dump.m_level == LogLevel::Trace)
                        //{
                        //	if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_CLIPBOARD_LIST, 0.6f, LOGPANEL_COLOR_TRACE_DEFAULT);
                        //	ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_TRACE_DEFAULT);
                        //}
                        // else if (it->m_dump.m_level == LogLevel::Warn)
                        //{
                        //	if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_EXCLAMATION_TRIANGLE, 0.6f, LOGPANEL_COLOR_WARN_DEFAULT);
                        //	ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_WARN_DEFAULT);
                        //}

                        // Draw the message text.
                        if (LOGPANEL_ICONSENABLED)
                            ImGui::SameLine();
                        WidgetsUtility::IncrementCursorPosY(-5);
                        ImGui::Text(it->m_dump.m_message.c_str());
                        // ImGui::PopStyleColor();
                        ImGui::PopTextWrapPos();

                        // Display the amount of calls for this same entry.
                        if (it->m_count != 0)
                        {
                            ImGui::SameLine();
                            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 35);
                            std::string text = "x " + std::to_string(it->m_count);
                            ImGui::Text(text.c_str());
                        }

                        if (LOGPANEL_ICONSENABLED)
                            WidgetsUtility::IncrementCursorPosY(6);
                    }
                }
                ImGui::EndChild();

                End();
            }
        }
    }

    void LogPanel::OnLog(const Event::ELog& dump)
    {
        LogDumpEntry entry(dump, 1);

        // Check if we have an entry with the same message and message type.
        std::deque<LogDumpEntry>::iterator it = std::find_if(m_logDeque.begin(), m_logDeque.end(), [&ce = entry](const LogDumpEntry& m) -> bool { return (ce.m_dump.m_message.compare(m.m_dump.m_message) == 0 && ce.m_dump.m_level == m.m_dump.m_level); });

        // If so, increase it's count so that we can display how many times the same entry is called.
        if (it != m_logDeque.end())
        {
            it->m_count++;
            return;
        }

        // Pop if exceeds max size.
        if (m_logDeque.size() == MAX_BACKTRACE_SIZE)
            m_logDeque.pop_front();

        m_logDeque.push_back(entry);
    }

    void LogLevelIconButton::UpdateColors(unsigned int* flags)
    {
        if (*flags & m_targetLevel)
        {
            m_usedColorDefault = m_colorDefault;
            m_usedColorHovered = m_colorHovered;
            m_usedColorPressed = m_colorPressed;
        }
        else
        {
            m_usedColorDefault = LOGPANEL_COLOR_ICONDEFAULT;
            m_usedColorHovered = LOGPANEL_COLOR_ICONHOVERED;
            m_usedColorPressed = LOGPANEL_COLOR_ICONPRESSED;
        }
    }

    void LogLevelIconButton::DrawButton(unsigned int* flags)
    {
        // if (WidgetsUtility::IconButton(m_id, m_icon, 0.0f, 1.0f, m_usedColorDefault, m_usedColorHovered, m_usedColorPressed))
        //{
        //	if (*flags & m_targetLevel)
        //	{
        //		*flags &= ~m_targetLevel;
        //		UpdateColors(flags);
        //	}
        //	else
        //	{
        //		*flags |= m_targetLevel;
        //		UpdateColors(flags);
        //	}
        //}

        if (ImGui::IsItemActive() || ImGui::IsItemHovered())
            ImGui::SetTooltip(m_tooltip);
    }

} // namespace Lina::Editor
