/*
Class: ToolbarItem



Timestamp: 12/27/2021 12:05:24 AM
*/

#pragma once

#ifndef ToolbarItem_HPP
#define ToolbarItem_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"
#include "Math/Vector.hpp"
#include "Data/Vector.hpp"

namespace Lina::Editor
{

    enum class ToolbarItemType
    {
        None = 0,

        MoveHandle   = 1,
        RotateHandle = 2,
        ScaleHandle  = 3,

        GlobalLocalHandle = 10,
        PivotHandle       = 11,

        PlayButton = 20,
        StopButton = 21,
        SkipButton = 22,
    };

    class ToolbarItem;

    class ToolbarGroup
    {
    public:
        ToolbarGroup(const Vector2& buttonSize)
            : m_buttonSize(buttonSize)
        {
        }

        virtual ~ToolbarGroup();

        void AddItem(ToolbarItem* item);
        void Draw();

    private:
        Vector2                     m_buttonSize = Vector2::Zero;
        Vector<ToolbarItem*> m_items;
    };

    class ToolbarItem
    {

    public:
        ToolbarItem(const char* icon, const char* tooltip, bool useToggle = false, bool startToggled = false, ToolbarItemType type = ToolbarItemType::None)
            : m_icon(icon), m_tooltip(tooltip), m_useToggle(useToggle), m_toggled(startToggled), m_type(type){};

        ToolbarItem() = default;
        ~ToolbarItem() = default;

        void Draw(const Vector2& size);
        bool m_toggled = false;

    private:
        const char*     m_tooltip   = nullptr;
        const char*     m_icon      = nullptr;
        bool            m_useToggle = false;
        ToolbarItemType m_type      = ToolbarItemType::None;
    };
} // namespace Lina::Editor

#endif
