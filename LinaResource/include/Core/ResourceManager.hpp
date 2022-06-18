/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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
Class: PhysicsEnginePhysX

Responsible for managing all asset resources, both offline & in runtime

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef ResourceEngine_HPP
#define ResourceEngine_HPP

#include "Core/CommonApplication.hpp"
#include "JobSystem/JobSystem.hpp"
#include "ResourceBundle.hpp"
#include "Utility/Packager.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    class Engine;

    namespace Utility
    {
        struct Folder;
    }

    namespace Event
    {
        class EventSystem;
        struct ERequestResourceReload;
    } // namespace Event

} // namespace Lina

namespace Lina::Resources
{

    class ResourceManager
    {

    public:
        static ResourceManager* Get()
        {
            return s_resourceManager;
        }
        static ResourceProgressData s_currentProgressData;
        static void                 ResetProgress();
        static void                 TriggerResourceUpdatedEvent();

        /// <summary>
        /// StartFrame packing the project Resource contents into a Lina Bundle.
        /// </summary>
        void PackageProject(const std::string& path, const std::string& name);

        /// <summary>
        /// Given a path to a Lina Bundle file, starts unpacking the file & loading resources inside.
        /// </summary>
        void ImportResourceBundle(const std::string& path, const std::string& name);

        /// <summary>
        /// !! Root folder will be nullptr during Standalone builds, which are required to run through the package import system instead of a file system.
        /// </summary>
        inline Utility::Folder* GetRootFolder()
        {
            return m_rootFolder;
        }

        /// <summary>
        /// Working the working directory as its returned from filesystem.
        /// </summary>
        /// <returns></returns>
        inline const std::string& GetWorkingDir()
        {
            return m_workingDirectory;
        }

        /// <summary>
        /// Returns the modified working directory. (replaced the \\ with / from the filesystem path)
        /// </summary>
        /// <returns></returns>
        inline const std::string& GetWorkingDirReplaced()
        {
            return m_workingDirectoryReplaced;
        }

    private:
        friend class Engine;
        ResourceManager()  = default;
        ~ResourceManager() = default;

        void Initialize(ApplicationInfo& appInfo);
        void AddAllResourcesToPack(std::vector<std::string>& resources, Utility::Folder* folder);
        void LoadEditorResources();
        void OnRequestResourceReload(const Event::ERequestResourceReload& ev);
        void Shutdown();

    private:
        static ResourceManager* s_resourceManager;
        Event::EventSystem*     m_eventSys = nullptr;
        ApplicationInfo         m_appInfo;
        Packager                m_packager;
        ResourceBundle          m_bundle;
        Utility::Folder*        m_rootFolder               = nullptr;
        ApplicationMode         m_appMode                  = ApplicationMode::Editor;
        std::string             m_workingDirectory         = "";
        std::string             m_workingDirectoryReplaced = "";
    };
} // namespace Lina::Resources

#endif
