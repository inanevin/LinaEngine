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
Class: ResourceSelectorPanel



Timestamp: 1/2/2022 4:40:26 PM
*/

#pragma once

#ifndef ResourceSelectorPanel_HPP
#define ResourceSelectorPanel_HPP

// Headers here.
#include "Panels/EditorPanel.hpp"
#include "Core/RenderBackendFwd.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    namespace Resources
    {
        class ResourceStorage;
    }
    namespace Utility
    {
        struct Folder;
        struct File;
    } // namespace Utility
} // namespace Lina
namespace Lina::Editor
{
    class ResourceSelectorPanel : public EditorPanel
    {

    public:
        ResourceSelectorPanel()  = default;
        ~ResourceSelectorPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

        inline void SetCurrentTypeID(TypeID tid, const std::string& typeStr)
        {
            m_currentFileType = tid;
            m_resourceStr     = typeStr;
        }

        StringIDType m_selectedResource = 0;

    private:
        void DrawTop();
        void DrawBottom();
        void DrawFilesInFolder(Utility::Folder* folder);

    private:
        bool                        m_firstLaunch      = true;
        TypeID                      m_currentFileType  = 0;
        Resources::ResourceStorage* m_storage          = nullptr;
        Utility::File*              m_selectedFile     = nullptr;
        Utility::Folder*            m_root             = nullptr;
        Graphics::Window*           m_window           = nullptr;
        std::string                 m_searchFilter     = "";
        std::string                 m_resourceStr      = "";
    };
} // namespace Lina::Editor

#endif
