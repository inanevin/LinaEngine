#include "Widgets/MenuButton.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/GUILayer.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include <iostream>

namespace Lina::Editor
{
    MenuBarElement::~MenuBarElement()
    {
        if (m_ownsChildren)
        {
            for (auto* child : m_children)
                delete child;
        }

        m_children.clear();
    }

    Menu::~Menu()
    {
        for (auto* element : m_elements)
            delete element;

        m_elements.clear();
        ;
    }
    void Menu::AddElement(MenuBarElement* elem)
    {
        m_elements.push_back(elem);
    }

    void Menu::Draw()
    {
        if (m_elements.size() > 0)
        {
            for (int i = 0; i < m_elements.size(); i++)
            {
                auto* element = m_elements[i];
                element->Draw();

                if (i < m_elements.size() - 1)
                {
                    const int nextElement = i + 1;
                    if (element->m_groupID != m_elements[nextElement]->m_groupID)
                    {
                        ImGui::Separator();
                    }
                }
            }
        }
    }

    void MenuBarElement::AddChild(MenuBarElement* child)
    {
        m_children.push_back(child);
    }

    void MenuBarElement::Draw()
    {
        const String emptyLabel     = "      ";
        const float       emptyLabelSize = ImGui::CalcTextSize(emptyLabel.c_str()).x;
        const String itemStr        = emptyLabel + String(m_title);
        const ImVec2      cursorPos      = ImVec2(ImGui::GetCursorScreenPos().x + 16, ImGui::GetCursorScreenPos().y);

        if (m_children.size() > 0)
        {
            // Makes the upcoming sub-menu seperated properly from the current menu.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, ImGui::GetStyle().ItemInnerSpacing.y));
            if (ImGui::BeginMenu(itemStr.c_str()))
            {
                for (int i = 0; i < m_children.size(); i++)
                {
                    m_children[i]->Draw();

                    if (i < m_children.size() - 1)
                    {
                        const int nextElement = i + 1;
                        if (m_children[i]->m_groupID != m_children[nextElement]->m_groupID)
                        {
                            ImGui::Separator();
                        }
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::PopStyleVar();
        }
        else
        {
            if (m_type == MenuBarElementType::None && m_children.size() == 0)
                ImGui::BeginDisabled();

            if(ImGui::MenuItem(itemStr.c_str()))
                Event::EventSystem::Get()->Trigger<EMenuBarElementClicked>(EMenuBarElementClicked{static_cast<uint8>(m_type)});

            if (m_type == MenuBarElementType::None && m_children.size() == 0)
                ImGui::EndDisabled();
        }

        const float textSize     = ImGui::CalcTextSize(itemStr.c_str()).x;
        float       textAddition = 0.0f;
        if (textSize > 100 * GUILayer::Get()->GetDPIScale())
            textAddition = 30.0f * GUILayer::Get()->GetDPIScale();

        bool arrowExists = m_children.size() > 0;
        if (m_tooltip != nullptr && String(m_tooltip).compare("") != 0)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(textAddition + 110 * GUILayer::Get()->GetDPIScale());
            if (m_tooltipIsIcon)
            {
                WidgetsUtility::IconSmall(m_tooltip);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                ImGui::Text(m_tooltip);
                ImGui::PopStyleColor();
            }
        }

        if (arrowExists)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(textAddition + 175 * GUILayer::Get()->GetDPIScale());
            WidgetsUtility::IconSmall(ICON_FA_CARET_RIGHT);
        }

        ImGui::SameLine();
        ImGui::InvisibleButton(emptyLabel.c_str(), ImVec2((emptyLabelSize - ImGui::GetStyle().ItemSpacing.x) * GUILayer::Get()->GetDPIScale(), 5 * GUILayer::Get()->GetDPIScale()));
    }
} // namespace Lina::Editor
