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

#include "World/Level/LevelManager.hpp"
#include "World/Level/Level.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "System/ISystem.hpp"
#include "Data/CommonData.hpp"

namespace Lina
{
    void LevelManager::Initialize()
    {
    }

    void LevelManager::Shutdown()
    {
    }

    void LevelManager::InstallLevel(const char* level)
    {
        // First get the level & extract target resources.
        const StringID     sid = TO_SIDC(level);
        ResourceIdentifier levelResource;
        ResourceManager::Get().LoadResources({ResourceIdentifier(level, GetTypeID<Level>(), sid)}, false);
        m_currentLevel = ResourceManager::Get().GetResourceWrapper<Level>(sid);

        // Leave already loaded resources that will still be used by next level.
        // Load others / unload unused.
        Vector<ResourceIdentifier>       resourcesToUnload;
        Vector<ResourceIdentifier>       resourcesToLoad;
        Vector<ResourceIdentifier>       levelResourcesToLoad = m_currentLevel.Get().GetUsedResources();
        Vector<ObjectWrapper<IResource>> allResources         = ResourceManager::Get().GetAllResources();

        for (auto& resWrapper : allResources)
        {
            auto& res = resWrapper.Get();

            // Never touch statics, they are loaded once & alive until program termination.
            if (ResourceManager::Get().GetPackageType(res.GetTID()) == PackageType::Static)
                continue;

            auto it = linatl::find_if(levelResourcesToLoad.begin(), levelResourcesToLoad.end(), [&](const ResourceIdentifier& id) { return id.sid == res.GetSID(); });

            // We'll unload if not gon be used.
            if (it == levelResourcesToLoad.end())
                resourcesToUnload.push_back(ResourceIdentifier{res.GetPath(), res.GetTID(), res.GetSID()});
            else
            {
                // Already exists, don't load again.
                levelResourcesToLoad.erase(it);
            }
        }

        ResourceManager::Get().UnloadResources(resourcesToUnload);
        ResourceManager::Get().LoadResources(resourcesToLoad, true);
        m_currentLevel.Get().Install(m_system);
    }

    void LevelManager::UninstallLevel()
    {
        m_currentLevel.Get().Uninstall(m_system);
    }

    void LevelManager::Tick(float dt)
    {
        if (m_currentLevel.IsValid())
        {
            Event data;
            data.fParams[0] = dt;
            m_system->DispatchGameEvent(EVG_Tick, data);
            m_system->DispatchGameEvent(EVG_PostTick, data);
        }
    }
} // namespace Lina
