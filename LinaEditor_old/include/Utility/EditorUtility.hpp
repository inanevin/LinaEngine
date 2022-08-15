/*
Class: EditorUtility

Various utility functions for the editor.

Timestamp: 5/9/2020 1:22:23
*/

#pragma once

#ifndef EditorUtility_HPP
#define EditorUtility_HPP

#include <Data/String.hpp>

namespace Lina::Editor
{
    class EditorUtility
    {

    public:
        EditorUtility() = default;
        ~EditorUtility() = default;

        static bool        CreateFolderInPath(const String& path);
        static bool        DeleteDirectory(const String& path);
        static String RemoveExtensionFromFilename(const String& filename);
        static String OpenFile(const char* filter, void* window);
        static String SaveFile(const char* filter, void* window);
        static String SelectPath(void* window);
        static bool        ChangeFilename(const char* folderPath, const char* oldName, const char* newName);

    private:
    };
} // namespace Lina::Editor

#endif
