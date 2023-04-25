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
#include "Graphics/Core/GfxManager.hpp"

namespace Lina
{
	void LevelManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Level Manager] -> Initialization.");
		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
	}

	void LevelManager::Shutdown()
	{
		LINA_TRACE("[Level Manager] -> Shutdown.");

		if (m_currentLevel)
			UninstallLevel(true);
	}

	void LevelManager::InstallLevel(const char* level)
	{
		ResourceManager* rm = (ResourceManager*)m_system->CastSubsystem(SubsystemType::ResourceManager);

		// First get the level & extract target resources.
		const StringID	   sid = TO_SIDC(level);
		ResourceIdentifier levelResource;
		rm->LoadResources({ResourceIdentifier(level, GetTypeID<Level>(), sid)});
		rm->WaitForAll();
		m_currentLevel = rm->GetResource<Level>(sid);

		// Leave already loaded resources that will still be used by next level.
		// Load others / unload unused.
		Vector<ResourceIdentifier> resourcesToUnload;
		Vector<ResourceIdentifier> resourcesToLoad;
		Vector<ResourceIdentifier> levelResourcesToLoad = m_currentLevel->GetUsedResources();
		Vector<IResource*>		   allResources			= rm->GetAllResources(true);

		for (auto& res : allResources)
		{
			// Never touch core resources, they are loaded once & alive until program termination.
			if (rm->IsPriorityResource(res->GetSID()) || rm->IsCoreResource(res->GetSID()) || res->IsUserManaged())
				continue;

			auto it = linatl::find_if(levelResourcesToLoad.begin(), levelResourcesToLoad.end(), [&](const ResourceIdentifier& id) { return id.sid == res->GetSID(); });

			// We'll unload if not gon be used.
			if (it == levelResourcesToLoad.end())
			{
				if (res->GetSID() != sid)
					resourcesToUnload.push_back(ResourceIdentifier{res->GetPath(), res->GetTID(), res->GetSID()});
			}
			else
			{
				// Already exists, don't load again.
				levelResourcesToLoad.erase(it);
			}
		}

		if (!resourcesToUnload.empty())
			rm->UnloadResources(resourcesToUnload);

		if (!resourcesToLoad.empty())
			rm->LoadResources(resourcesToLoad);
		rm->WaitForAll();
		m_currentLevel->Install();

		Event data;
		data.pParams[0] = static_cast<void*>(m_currentLevel);
		m_system->DispatchEvent(EVS_LevelInstalled, data);

		m_gfxManager->CreateWorldRenderer([this](WorldRenderer* rend) { m_currentLevel->SetWorldRenderer(rend); }, m_currentLevel->GetWorld(), Vector2(800, 600), WRM_None);
	}

	void LevelManager::UninstallLevel(bool immediate)
	{
		auto uninstall = [this]() {
			m_currentLevel->Uninstall();

			ResourceManager* rm = (ResourceManager*)m_system->CastSubsystem(SubsystemType::ResourceManager);

			ResourceIdentifier ident;
			ident.tid  = m_currentLevel->GetTID();
			ident.sid  = m_currentLevel->GetSID();
			ident.path = m_currentLevel->GetPath();
			rm->UnloadResources({ident});

			Event data;
			data.pParams[0] = static_cast<void*>(m_currentLevel);
			m_system->DispatchEvent(EVS_LevelUninstalled, data);
			m_currentLevel = nullptr;
		};

		m_gfxManager->DestroyWorldRenderer(uninstall, m_currentLevel->GetWorldRenderer(), immediate);

		if (immediate)
			uninstall();
	}

	void LevelManager::QueueLevel(const char* level)
	{
		m_queuedLevel		= level;
		m_queuedLevelExists = true;
	}

	void LevelManager::Simulate(float fixedDelta)
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->GetWorld()->Simulate(fixedDelta);
	}

	void LevelManager::Tick(float deltaTime)
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->GetWorld()->Tick(deltaTime);

		if (m_currentLevel == nullptr && m_queuedLevelExists)
		{
			InstallLevel(m_queuedLevel.c_str());
			m_queuedLevelExists = false;
			m_queuedLevel.clear();
		}
	}

	void LevelManager::WaitForSimulation()
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->GetWorld()->WaitForSimulation();
	}

} // namespace Lina
