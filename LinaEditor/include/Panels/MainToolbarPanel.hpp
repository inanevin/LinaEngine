/*
Class: MainToolbarPanel



Timestamp: 12/17/2021 11:58:19 AM
*/

#pragma once

#ifndef MainToolbarPanel_HPP
#define MainToolbarPanel_HPP

// Headers here.
#include "Panels/EditorPanel.hpp"

namespace Lina::Editor
{
    struct ETransformGizmoChanged;
    struct ETransformPivotChanged;

    class MainToolbarPanel : public EditorPanel
    {

    public:
        MainToolbarPanel() = default;
        virtual ~MainToolbarPanel();
        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw();
        void         DrawFooter();

    private:
        void OnTransformGizmoChanged(const ETransformGizmoChanged& ev);
        void OnTransformPivotChanged(const ETransformPivotChanged& ev);

    private:
        int  m_toggledTransformSelection = 0;
        bool m_currentGizmoGlobal;
    };
} // namespace Lina::Editor

#endif
