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
Class: CommonUtility



Timestamp: 12/25/2021 1:24:11 PM
*/

#pragma once

#ifndef CommonUtility_HPP
#define CommonUtility_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Data/Vector.hpp"
#include <Data/String.hpp>
#include <filesystem>

namespace Lina::Utility
{
    struct Folder;

    struct DirectoryItem
    {
        DirectoryItem()  = default;
        ~DirectoryItem() = default;

        String                          m_fullPath   = "";
        String                          m_name       = ""; // name without extension
        Folder*                         m_parent     = nullptr;
        TypeID                          m_typeID     = 0;
        StringIDType                    m_sid        = 0;
        bool                            m_isRenaming = false;
        std::filesystem::file_time_type m_lastWriteTime;
    };

    struct File : public DirectoryItem
    {
        File()  = default;
        ~File() = default;

        String m_folderPath = "";
        String m_extension  = "";
        String m_fullName   = ""; // name with extension
    };

    struct Folder : public DirectoryItem
    {
        Folder() = default;
        ~Folder();

        Vector<File*>   m_files;
        Vector<Folder*> m_folders;
        bool            m_isOpen = false;
    };
} // namespace Lina::Utility

#endif
