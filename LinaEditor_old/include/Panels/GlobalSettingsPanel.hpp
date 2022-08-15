/*
Class: GlobalSettingsPanel

Responsible for drawing global settings like render settings, game settings etc.

Timestamp: 10/30/2020 9:37:42 AM
*/

#pragma once

#ifndef GlobalSettingsPanel_HPP
#define GlobalSettingsPanel_HPP

#include "Panels/EditorPanel.hpp"

namespace Lina
{
    namespace World
    {
        class Level;
    }

} // namespace Lina

namespace Lina::Editor
{
    class GlobalSettingsPanel : public EditorPanel
    {

    public:
        GlobalSettingsPanel() = default;
        ~GlobalSettingsPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

    };
} // namespace Lina::Editor

#endif
