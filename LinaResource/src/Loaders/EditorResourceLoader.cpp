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

#include "Loaders/EditorResourceLoader.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Utility/ResourceUtility.hpp"
#include "Core/ResourceStorage.hpp"
#include "JobSystem/JobSystem.hpp"
#include "Data/DataCommon.hpp"
#include "Core/PlatformMacros.hpp"

namespace Lina::Resources
{
    void EditorResourceLoader::LoadResource(TypeID tid, const String& path)
    {
        if (ResourceStorage::Get()->Exists(tid, StringID(path.c_str()).value()))
            return;

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{ .title = "Loading File", .totalFiles = 1 });
        LoadSingleResourceFromFile(tid, path);
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{ .currentResource = path });
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>();
    }

    void EditorResourceLoader::LoadLevelResources(const HashMap<TypeID, HashSet<String>>& resourceMap)
    {

        ResourceStorage*                 storage = ResourceStorage::Get();
        HashMap<TypeID, HashSet<String>> toLoad;

        // 1: Take a look at all the existing resources in storage, if it doesn't exist in the current level's resources we are loading, unload it.
        // 2: Iterate all the resourceMap we are about to load, find the one's that are not existing right now, add them to a seperate map.
        // 3: Load the separate map.

        UnloadUnusedResources(resourceMap);

        // Now iterate the resourceMap again, find the non-existing resources,
        for (auto pair : resourceMap)
        {
            const HashSet<String> set = resourceMap.at(pair.first);
            for (auto str : set)
            {
                if (!storage->Exists(pair.first, StringID(str.c_str()).value()))
                    toLoad[pair.first].insert(str);
            }
        }

        int totalCount = 0;
        for (auto& pair : toLoad)
            totalCount += static_cast<int>(pair.second.size());

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{.title = "Loading Level Resources", .totalFiles = totalCount});

#ifdef LINA_MT

        Vector<Pair<TypeID, String>> toLoadVec;

        for (const auto& [tid, set] : toLoad)
        {
            for (const auto& str : set)
                toLoadVec.push_back(linatl::make_pair(tid, str));
        }

        Taskflow taskflow;
        taskflow.for_each(toLoadVec.begin(), toLoadVec.end(), [&](const Pair<TypeID, String>& p) {
            LoadSingleResourceFromFile(p.first, p.second);
            Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = p.second});
        });
        JobSystem::Get()->Run(taskflow).wait();

#else
        for (const auto& [tid, set] : toLoad)
        {
            for (const auto& str : set)
            {
                LoadSingleResourceFromFile(tid, str);
                Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = str});
            }
        }
#endif
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>();
    }

} // namespace Lina::Resources
