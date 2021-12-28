/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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
    }

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
        /// Start packing the project Resource contents into a Lina Bundle.
        /// </summary>
        void PackageProject(const std::string& path, const std::string& name);

        /// <summary>
        /// Given a path to a Lina Bundle file, starts unpacking the file & loading resources inside.
        /// </summary>
        void ImportResourceBundle(const std::string& path, const std::string& name);

    private:
        friend class Engine;
        ResourceManager() = default;
        ~ResourceManager() = default;

        void Initialize(ApplicationInfo& appInfo);
        void AddAllResourcesToPack(std::vector<std::string>& resources, Utility::Folder& folder);
        void LoadEditorResources();
        void Shutdown();

    private:
        static ResourceManager* s_resourceManager;
        Event::EventSystem*     m_eventSys = nullptr;
        ApplicationInfo         m_appInfo;
        Packager                m_packager;
        Executor                m_executor;
        TaskFlow                m_taskflow;
        TaskFlow                m_taskflowLoop;
        Future<void>            m_future;
        Future<void>            m_futureLoop;

    private:
        ResourceBundle  m_bundle;
        ApplicationMode m_appMode = ApplicationMode::Editor;
    };
} // namespace Lina::Resources

#endif
