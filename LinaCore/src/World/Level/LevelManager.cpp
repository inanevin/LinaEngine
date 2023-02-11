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
	void LevelManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Level Manager] -> Initialization.");
	}

	void LevelManager::Shutdown()
	{
		LINA_TRACE("[Level Manager] -> Shutdown.");
		UninstallLevel();
	}

	void LevelManager::InstallLevel(const char* level)
	{
		ResourceManager* rm = (ResourceManager*)m_system->GetSubsystem(SubsystemType::ResourceManager);

		// First get the level & extract target resources.
		const StringID	   sid = TO_SIDC(level);
		ResourceIdentifier levelResource;
		rm->LoadResources({ResourceIdentifier(level, GetTypeID<Level>(), sid)}, false);
		m_currentLevel = rm->GetResource<Level>(sid);

		// Leave already loaded resources that will still be used by next level.
		// Load others / unload unused.
		Vector<ResourceIdentifier> resourcesToUnload;
		Vector<ResourceIdentifier> resourcesToLoad;
		Vector<ResourceIdentifier> levelResourcesToLoad = m_currentLevel->GetUsedResources();
		Vector<IResource*>		   allResources			= rm->GetAllResources();

		for (auto& res : allResources)
		{
			// Never touch core resources, they are loaded once & alive until program termination.
			if (rm->IsCoreResource(res->GetSID()) || res->IsUserManaged())
				continue;

			auto it = linatl::find_if(levelResourcesToLoad.begin(), levelResourcesToLoad.end(), [&](const ResourceIdentifier& id) { return id.sid == res->GetSID(); });

			// We'll unload if not gon be used.
			if (it == levelResourcesToLoad.end())
				resourcesToUnload.push_back(ResourceIdentifier{res->GetPath(), res->GetTID(), res->GetSID()});
			else
			{
				// Already exists, don't load again.
				levelResourcesToLoad.erase(it);
			}
		}

		rm->UnloadResources(resourcesToUnload);
		rm->LoadResources(resourcesToLoad, true);
		m_currentLevel->Install(m_system);
	}

	void LevelManager::UninstallLevel()
	{
		if (m_currentLevel != nullptr)
		{
			m_currentLevel->Uninstall(m_system);
			m_currentLevel = nullptr;
		}
	}

	void LevelManager::Tick(float dt)
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->GetWorld()->Tick(dt);
	}

	void LevelManager::SyncData(float alpha)
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->GetWorld()->SyncData(alpha);
	}

	void LevelManager::WaitForSimulation()
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->GetWorld()->WaitForSimulation();
	}
} // namespace Lina
