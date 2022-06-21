/*
Class: FileWatcher



Timestamp: 3/21/2022 8:45:01 PM
*/

#pragma once

#ifndef FileWatcher_HPP
#define FileWatcher_HPP

// Headers here.
#include "EventSystem/MainLoopEvents.hpp"
#include <filesystem>

namespace Lina
{
    enum class FileWatchStatus
    {
        None,
        Created,
        Modified,
        Erased
    };

    class FileWatcher
    {

    public:
        FileWatcher(){};
        ~FileWatcher(){};

        void Initialize(const String& directory, float interval, FileWatchStatus targetStatus);

    public:
        std::function<void(FileWatchStatus status, const String& fullPath)> m_changeCallback;

    private:
        void OnTick(const Event::ETick&);
        bool Contains(const String& key);
        void ReplaceAndCall(FileWatchStatus status, const String& path);

    private:
        String                                                      m_directory        = "";
        float                                                            m_totalTime        = 0.0f;
        float                                                            m_interval         = 0.0f;
        float                                                            m_lastCheckTime    = 0.0f;
        FileWatchStatus                                                  m_targetStatus     = FileWatchStatus::None;
        HashMap<String, std::filesystem::file_time_type> m_paths;
    };
} // namespace Lina

#endif
