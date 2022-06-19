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
