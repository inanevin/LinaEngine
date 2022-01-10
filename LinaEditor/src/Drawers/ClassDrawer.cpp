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

#include "Drawers/ClassDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "Core/PhysicsBackend.hpp"
#include <entt/meta/factory.hpp>
#include <entt/meta/node.hpp>
#include <entt/meta/resolve.hpp>
#include "IconsFontAwesome5.h"
#include <unordered_map>
using namespace entt::literals;

std::unordered_map<std::string, std::vector<entt::meta_data>> m_propertyList; // Category- property pair

namespace Lina::Editor
{
    bool ClassDrawer::DrawClass(TypeID tid, entt::meta_any& instance, bool drawHeader)
    {
        bool  anyPropertyChanged = false;
        auto& resolvedData       = entt::resolve(tid);

        if (resolvedData)
        {
            auto titleProperty = resolvedData.prop("Title"_hs);

            if (titleProperty)
            {
                auto title       = titleProperty.value().cast<const char*>();
                bool foldoutOpen = drawHeader ? WidgetsUtility::Header(tid, title) : true;

                if (foldoutOpen)
                {
                    for (auto data : resolvedData.data())
                    {

                        auto labelProperty = data.prop("Title"_hs);
                        auto typeProperty  = data.prop("Type"_hs);

                        if (!labelProperty || !typeProperty)
                            continue;

                        std::string category = std::string(data.prop("Category"_hs).value().cast<const char*>());

                        AddPropertyToDrawList(category, data);
                    }

                    anyPropertyChanged = FlushDrawList(title, resolvedData, instance);
                }
            }
        }

        return anyPropertyChanged;
    }

    void ClassDrawer::AddPropertyToDrawList(const std::string& category, entt::meta_data& data)
    {
        m_propertyList[category].push_back(data);
    }

    bool ClassDrawer::FlushDrawList(const std::string& id, entt::meta_type& owningType, entt::meta_any& instance)
    {
        bool anyPropertyChanged = false;

        for (auto& [category, propList] : m_propertyList)
        {
            bool show = true;

            if (category.compare("") != 0)
            {
                const std::string caretID = id + std::string(category);
                show                      = WidgetsUtility::CaretTitle(category.c_str(), caretID);
            }

            if (show)
            {
                for (auto& prop : propList)
                {
                    if (DrawProperty(owningType, prop, instance))
                        anyPropertyChanged = true;
                }
            }
            WidgetsUtility::IncrementCursorPosY(4);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
            WidgetsUtility::HorizontalDivider(-2.0f, 0.5f);
            ImGui::PopStyleColor();
        }

        m_propertyList.clear();

        return anyPropertyChanged;
    }

    bool ClassDrawer::DrawProperty(entt::meta_type& owningType, entt::meta_data& data, entt::meta_any& instance)
    {
        bool propertyChanged = false;

        auto labelProperty = data.prop("Title"_hs);
        auto typeProperty  = data.prop("Type"_hs);

        if (!labelProperty || !typeProperty)
            return false;

        const char* label                     = labelProperty.value().cast<const char*>();
        std::string type                      = std::string(typeProperty.value().cast<const char*>());
        auto        displayDependencyProperty = data.prop("Depends"_hs);

        if (displayDependencyProperty)
        {
            entt::hashed_string displayDependencyHash = displayDependencyProperty.value().cast<entt::hashed_string>();
            auto                dependantMember       = owningType.data(displayDependencyHash);

            if (dependantMember)
            {
                bool dependantMemberValue = dependantMember.get(instance).cast<bool>();
                if (!dependantMemberValue)
                    return false;
            }
        }

        std::string varLabelID = "##_" + std::string(label);
        WidgetsUtility::PropertyLabel(label);

        auto tooltipProperty = data.prop("Tooltip"_hs);
        if (tooltipProperty)
        {
            std::string tooltip = std::string(tooltipProperty.value().cast<const char*>());
            if (ImGui::IsItemHovered() && tooltip.compare("") != 0)
                WidgetsUtility::Tooltip(tooltip.c_str());
        }

        if (type.compare("Float") == 0)
        {
            float       variable = data.get(instance).cast<float>();
            const float prev     = variable;
            WidgetsUtility::DragFloat(varLabelID.c_str(), nullptr, &variable);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Int") == 0)
        {
            int       variable = data.get(instance).cast<int>();
            const int prev     = variable;
            WidgetsUtility::DragInt(varLabelID.c_str(), nullptr, &variable);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Vector2") == 0)
        {
            Vector2       variable = data.get(instance).cast<Vector2>();
            const Vector2 prev     = variable;
            WidgetsUtility::DragVector2(varLabelID.c_str(), &variable.x);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Vector3") == 0)
        {
            Vector3       variable = data.get(instance).cast<Vector3>();
            const Vector3 prev     = variable;
            WidgetsUtility::DragVector3(varLabelID.c_str(), &variable.x);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Vector4") == 0)
        {
            Vector4       variable = data.get(instance).cast<Vector4>();
            const Vector4 prev     = variable;
            WidgetsUtility::DragVector4(varLabelID.c_str(), &variable.x);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Color") == 0)
        {
            Color       variable = data.get(instance).cast<Color>();
            const Color prev     = variable;
            WidgetsUtility::ColorButton(varLabelID.c_str(), &variable.r);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Bool") == 0)
        {
            bool       variable = data.get(instance).cast<bool>();
            const bool prev     = variable;
            ImGui::Checkbox(varLabelID.c_str(), &variable);
            data.set(instance, variable);

            if (prev != variable)
                propertyChanged = true;
        }
        else if (type.compare("Material") == 0)
        {
            auto               handle = data.get(instance).cast<Resources::ResourceHandle<Graphics::Material>>();
            const StringIDType prev   = handle.m_sid;
            WidgetsUtility::ResourceSelectionMaterial(&handle);
            data.set(instance, handle);

            if (prev != handle.m_sid)
                propertyChanged = true;
        }
        else if (type.compare("MaterialArray") == 0)
        {
        }

        return propertyChanged;
    }

} // namespace Lina::Editor