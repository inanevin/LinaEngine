#include "Utility/FileWatcher.hpp"
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

    void FileWatcher::OnTick(const Event::ETick& ev)
    {
        m_totalTime += ev.m_deltaTime;

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