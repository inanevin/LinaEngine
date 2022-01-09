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

/*
Class: LevelPanel

Displays the final image of the scene generated via the engine. Is used as a main viewport for editing levels.

Timestamp: 6/5/2020 6:51:29 PM
*/

#pragma once

#ifndef LevelPanel_HPP
#define LevelPanel_HPP

#include "Core/CommonECS.hpp"
#include "Panels/EditorPanel.hpp"

namespace Lina
{
    namespace Event
    {
        struct ELevelUninstalled;
        struct EKeyCallback;
        struct EMouseButtonCallback;
    } // namespace Event
} // namespace Lina

namespace Lina::Editor
{
    struct EEntitySelected;
    struct EEntityUnselected;
    struct ETransformGizmoChanged;
    struct ETransformPivotChanged;
    struct EShortcut;

    class LevelPanel : public EditorPanel
    {

    public:
        enum class DrawMode
        {
            FinalImage,
            ShadowMap
        };

        LevelPanel()          = default;
        virtual ~LevelPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

        void        EntitySelected(const EEntitySelected& ev);
        void        Unselected(const EEntityUnselected& ev);
        void        LevelUninstalled(const Event::ELevelUninstalled& ev);
        void        ProcessInput();
        void        DrawGizmos();
        inline void SetDrawMode(DrawMode mode)
        {
            m_drawMode = mode;
        }
        inline bool IsFocused()
        {
            return m_isFocused;
        }
        inline bool IsHovered()
        {
            return m_isHovered;
        }

        inline int GetTransformOperation()
        {
            return m_transformOperation;
        }
        inline int GetIsGizmoGlobal()
        {
            return m_isTransformModeGlobal;
        }

        bool m_shouldShowGizmos = false;

    private:
        void OnTransformGizmoChanged(const ETransformGizmoChanged& ev);
        void OnTransformPivotChanged(const ETransformPivotChanged& ev);
        void OnShortcut(const EShortcut& ev);
        void OnKeyCallback(const Event::EKeyCallback& ev);
        void OnMouseButtonCallback(const Event::EMouseButtonCallback& ev);

    private:
        int         m_transformOperation    = 0;
        bool        m_isTransformModeGlobal = true;
        float       m_borderAlpha           = 0.0f;
        bool        m_isFocused             = true;
        bool        m_isHovered             = false;
        ECS::Entity m_selectedEntity        = entt::null;
        DrawMode    m_drawMode              = DrawMode::FinalImage;
    };
} // namespace Lina::Editor

#endif
