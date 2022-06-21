/*
Class: TextEditorPanel



Timestamp: 1/18/2022 9:27:55 PM
*/

#pragma once

#ifndef TextEditorPanel_HPP
#define TextEditorPanel_HPP

// Headers here.
#include "Panels/EditorPanel.hpp"
#include "Data/Map.hpp"
#include <Data/String.hpp>

namespace Lina
{
    namespace Utility
    {
        struct File;
    }
} // namespace Lina

namespace Lina::Editor
{

    struct EShortcut;

    class TextEditorPanel : public EditorPanel
    {

    public:
        TextEditorPanel()  = default;
        ~TextEditorPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;
        virtual void Close() override;

        void AddFile(Utility::File* file);

    private:
        void OnShortcut(const EShortcut& ev);
        void SaveCurrentFile(const String& text);
        void ReloadCurrentFile();

    private:
        float                                 m_currentFontSize = 1.0f;
        Map<Utility::File*, String> m_openFiles;
        Utility::File*                        m_currentFile = nullptr;
    };
} // namespace Lina::Editor

#endif
