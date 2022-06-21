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
