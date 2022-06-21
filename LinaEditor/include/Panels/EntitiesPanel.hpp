/*
Class: EntitiesPanel

Draws all the alive entities in the current loaded level.

Timestamp: 5/23/2020 4:15:06 PM
*/
#pragma once

#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include "Core/CommonECS.hpp"
#include "Panels/EditorPanel.hpp"
#include <EventSystem\InputEvents.hpp>

namespace Lina
{
    namespace World
    {
        class Level;
    }

    namespace Event
    {
        struct ELevelInstalled;
        struct EKeyCallback;
    } // namespace Event

} // namespace Lina

namespace Lina::Editor
{
    class LevelPanel;
    class PropertiesPanel;
    class Menu;
    struct EShortcut;

    class EntitiesPanel : public EditorPanel
    {

    public:
        EntitiesPanel() = default;
        virtual ~EntitiesPanel();

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;
        void         DrawEntityNode(int id, ECS::Entity entity);

    private:
        void OnLevelInstall(const Event::ELevelInstalled& event);
        void OnShortcut(const EShortcut& event);
        void OnKeyCallback(const Event::EKeyCallback& event);

    private:
        ECS::Entity     m_selectedEntity;
        Menu* m_rightClickMenu = nullptr;
    };
} // namespace Lina::Editor

#endif
