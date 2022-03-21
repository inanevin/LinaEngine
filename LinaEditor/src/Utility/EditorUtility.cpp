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

#include "Utility/EditorUtility.hpp"

#include "Log/Log.hpp"
#include "Rendering/Material.hpp"

#include <cereal/archives/binary.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <sys/stat.h>

#ifdef LINA_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <commdlg.h>
#endif

namespace Lina::Editor
{
    bool EditorUtility::CreateFolderInPath(const std::string& path)
    {
        return std::filesystem::create_directory(path);
    }

    bool EditorUtility::DeleteDirectory(const std::string& path)
    {
        return false;
    }

    std::string EditorUtility::RemoveExtensionFromFilename(const std::string& fileName)
    {
        size_t lastindex = fileName.find_last_of(".");
        return fileName.substr(0, lastindex);
    }

    std::string EditorUtility::OpenFile(const char* filter, void* window)
    {
#ifdef LINA_PLATFORM_WINDOWS
        OPENFILENAMEA ofn;
        CHAR          szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize  = sizeof(OPENFILENAME);
        ofn.hwndOwner    = glfwGetWin32Window((GLFWwindow*)window);
        ofn.lpstrFile    = szFile;
        ofn.nMaxFile     = sizeof(szFile);
        ofn.lpstrFilter  = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            std::string replacedPath = ofn.lpstrFile;
            std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
            return replacedPath;
        }

#endif
        return std::string();
    }

    std::string EditorUtility::SaveFile(const char* filter, void* window)
    {

#ifdef LINA_PLATFORM_WINDOWS
        OPENFILENAMEA ofn;
        CHAR          szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize  = sizeof(OPENFILENAME);
        ofn.hwndOwner    = glfwGetWin32Window((GLFWwindow*)window);
        ofn.lpstrFile    = szFile;
        ofn.nMaxFile     = sizeof(szFile);
        ofn.lpstrFilter  = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            std::string replacedPath = ofn.lpstrFile;
            std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
            return replacedPath;
        }
#endif
        return std::string();
    }

    std::string EditorUtility::SelectPath(void* window)
    {

#ifdef LINA_PLATFORM_WINDOWS
        OPENFILENAMEA ofn;
        CHAR          szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize  = sizeof(OPENFILENAME);
        ofn.hwndOwner    = glfwGetWin32Window((GLFWwindow*)window);
        ofn.lpstrFile    = szFile;
        ofn.nMaxFile     = sizeof(szFile);
        ofn.lpstrFilter  = ".exe";
        ofn.nFilterIndex = 1;
        ofn.Flags        = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            std::string replacedPath = ofn.lpstrFile;
            std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
            return replacedPath;
        }

#endif
        return std::string();
    }

    bool EditorUtility::ChangeFilename(const char* folderPath, const char* oldName, const char* newName)
    {
        std::string oldPathStr = std::string(folderPath) + std::string(oldName);
        std::string newPathStr = std::string(folderPath) + std::string(newName);

        /*	Deletes the file if exists */
        if (std::rename(oldPathStr.c_str(), newPathStr.c_str()) != 0)
        {
            LINA_ERR("Can not rename file! Folder Path: {0}, Old Name: {1}, New Name: {2}", folderPath, oldName, newName);
            return false;
        }

        return true;
    }

} // namespace Lina::Editor
