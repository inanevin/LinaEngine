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

#include "Utility/FileWatcher.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/EventSystem.hpp"

namespace Lina
{
    void FileWatcher::Initialize(const String& directory, float interval, FileWatchStatus targetStatus)
    {
        m_directory     = directory;
        m_interval      = interval;
        m_targetStatus  = targetStatus;
        m_lastCheckTime = 0.0f;

        Event::EventSystem::Get()->Connect<Event::ETick, &FileWatcher::OnTick>(this);

        for (auto& file : std::filesystem::recursive_directory_iterator(m_directory.c_str()))
            m_paths[String(file.path().string().c_str())] = std::filesystem::last_write_time(file);
    }

    void FileWatcher::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::ETick>(this);
    }

    void FileWatcher::OnTick(const Event::ETick& ev)
    {
        m_totalTime += ev.deltaTime;

        if (m_totalTime > m_lastCheckTime + m_interval)
        {
            m_lastCheckTime = m_totalTime;

            auto it = m_paths.begin();

            while (it != m_paths.end())
            {
                if (!std::filesystem::exists(it->first.c_str()))
                {
                    ReplaceAndCall(FileWatchStatus::Erased, it->first);
                    it = m_paths.erase(it);
                }
                else
                    it++;
            }

            for (auto& file : std::filesystem::recursive_directory_iterator(m_directory.c_str()))
            {
                auto currentLWT = std::filesystem::last_write_time(file);

                if (!Contains(file.path().string().c_str()))
                {
                    m_paths[file.path().string().c_str()] = currentLWT;
                    ReplaceAndCall(FileWatchStatus::Created, file.path().string().c_str());
                }
                else
                {
                    if (m_paths[file.path().string().c_str()] != currentLWT)
                    {
                        m_paths[file.path().string().c_str()] = currentLWT;
                        ReplaceAndCall(FileWatchStatus::Modified, file.path().string().c_str());
                    }
                }
            }
        }
    }

    bool FileWatcher::Contains(const String& key)
    {
        return m_paths.find(key) != m_paths.end();
    }

    void FileWatcher::ReplaceAndCall(FileWatchStatus status, const String& path)
    {
        String targetPath = path;
        std::replace(targetPath.begin(), targetPath.end(), '\\', '/');
        m_changeCallback(status, targetPath);
    }

} // namespace Lina