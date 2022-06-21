/*
Class: EntitiesPanel

Draws all the alive entities in the current loaded level.

Timestamp: 5/23/2020 4:15:06 PM
*/
#pragma once

#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "Core/CommonECS.hpp"
#include "Panels/EditorPanel.hpp"

namespace Lina
{
    namespace ECS
    {
		class System;
    }
} // namespace Lina

namespace Lina::Editor
{
    class LevelPanel;
    class PropertiesPanel;
    class MenuBarElement;
    struct EShortcut;

    class SystemsPanel : public EditorPanel
    {

    public:
        SystemsPanel() = default;
        virtual ~SystemsPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

    private:
		void DrawSystem(const ECS::System* system, const String& pipeline);
    };
} // namespace Lina::Editor

#endif
