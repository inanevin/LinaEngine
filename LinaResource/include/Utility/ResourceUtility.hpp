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

#ifndef ResourceUtility_HPP
#define ResourceUtility_HPP

#include "Core/ResourceCommon.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    namespace Utility
    {
        struct Folder;
        struct File;
    } // namespace Utility
};    // namespace Lina

namespace Lina::Resources
{
    class ResourceUtility
    {
    public:
        static Utility::Folder*     s_rootFolder;

        static String& GetWorkingDire()
        {
            return s_workingDirectory;
        }

        static String& GetWorkingDirReplaced()
        {
            return s_workingDirectoryReplaced;
        }

    private:

        friend class ResourceStorage;
        friend class ResourcePackager;
        friend class StandaloneResourceLoader;
        friend class EditorResourceLoader;

        static String s_workingDirectory;
        static String s_workingDirectoryReplaced;

        /// <summary>
        /// Scans the given folder and fills it's child folder data
        /// </summary>
        /// <param name="root"></param>
        /// <param name="recursive"></param>
        /// <param name="totalFiles"></param>
        /// <param name="isRescan"></param>
        static void ScanFolder(Utility::Folder* root, bool recursive = true, int* totalFiles = nullptr, bool isRescan = false);

        /// <summary>
        /// Deletes the file and removes any resource tied with it.
        /// </summary>
        /// <param name="file"></param>
        static void DeleteResourceFile(Utility::File* file);

        /// <summary>
        /// Recursively search the target folder & return the path of the file with the given sid.
        /// </summary>
        /// <param name="folder"></param>
        /// <param name="sid"></param>
        /// <returns></returns>
        static String SearchFolderForSID(Utility::Folder* folder, StringID sid);

        /// <summary>
        /// Scans root folder structure & re-constructs.
        /// </summary>
        static void ScanRootFolder();

        /// <summary>
        ///
        /// </summary>
        static void InitializeWorkingDirectory();

        /// <summary>
        /// 
        /// </summary>
        static String PackageTypeToString(PackageType type);

    };
}; // namespace Lina::Resources

#endif
