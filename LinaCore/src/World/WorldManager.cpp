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

#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/System/System.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Graphics/GfxManager.hpp"

namespace Lina
{
	void WorldManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
	}

	void WorldManager::Shutdown()
	{
	}

	void WorldManager::CreateAndSaveNewWorld(const String& absolutePath)
	{
		EntityWorld world(nullptr, "", 0);
		OStream		stream;
		world.SaveToStream(stream);
		Serialization::SaveToFile(absolutePath.c_str(), stream);
		stream.Destroy();
	}

	void WorldManager::LoadWorld(const String& path)
	{
		LINA_ASSERT(m_loadedWorld == nullptr, "");

		auto*			   rm  = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		const auto		   sid = TO_SID(path);
		ResourceIdentifier ident(path, GetTypeID<EntityWorld>(), sid);
		rm->LoadResources({ident});
		rm->WaitForAll();
		m_loadedWorld = rm->GetResource<EntityWorld>(sid);

		// TEST
		m_loadedWorld->CreateEntity("Dummy Entity");
		m_loadedWorld->CreateEntity("Dummy Entity 2");
		Entity* e = m_loadedWorld->CreateEntity("Dummy Entity 3");
		e->AddChild(m_loadedWorld->CreateEntity("Child"));
	}

	void WorldManager::InstallLevel(const char* level)
	{
		// ResourceManager* rm = (ResourceManager*)m_system->CastSubsystem(SubsystemType::ResourceManager);

		// // First get the level & extract target resources.
		// const StringID	   sid = TO_SIDC(level);
		// ResourceIdentifier levelResource;
		// rm->LoadResources({ResourceIdentifier(level, GetTypeID<Level>(), sid)});
		// rm->WaitForAll();
		// m_currentLevel = rm->GetResource<Level>(sid);

		// // Leave already loaded resources that will still be used by next level.
		// // Load others / unload unused.
		// Vector<ResourceIdentifier> resourcesToUnload;
		// Vector<ResourceIdentifier> resourcesToLoad;
		// Vector<ResourceIdentifier> levelResourcesToLoad = m_currentLevel->GetUsedResources();
		// Vector<Resource*>		   allResources			= rm->GetAllResources(true);

		// for (auto& res : allResources)
		// {
		// 	// Never touch core resources, they are loaded once & alive until program termination.
		// 	if (res->IsPriorityResource() || res->IsCoreResource() || res->IsOwnedByUser())
		// 		continue;

		// 	auto it = linatl::find_if(levelResourcesToLoad.begin(), levelResourcesToLoad.end(), [&](const ResourceIdentifier& id) { return id.sid == res->GetSID(); });

		// 	// We'll unload if not gon be used.
		// 	if (it == levelResourcesToLoad.end())
		// 	{
		// 		if (res->GetSID() != sid)
		// 			resourcesToUnload.push_back(ResourceIdentifier{res->GetPath(), res->GetTID(), res->GetSID()});
		// 	}
		// 	else
		// 	{
		// 		// Already exists, don't load again.
		// 		levelResourcesToLoad.erase(it);
		// 	}
		// }

		// if (!resourcesToUnload.empty())
		// 	rm->UnloadResources(resourcesToUnload);

		// if (!resourcesToLoad.empty())
		// 	rm->LoadResources(resourcesToLoad);
		// rm->WaitForAll();
		// m_currentLevel->Install();

		// Event data;
		// data.pParams[0] = static_cast<void*>(m_currentLevel);
		// m_system->DispatchEvent(EVS_LevelInstalled, data);
	}

	void WorldManager::UninstallLevel(bool immediate)
	{
		//	auto uninstall = [this]() {
		//		Event data;
		//		data.pParams[0] = static_cast<void*>(m_currentLevel);
		//
		//		m_currentLevel->Uninstall();
		//
		//		ResourceManager* rm = (ResourceManager*)m_system->CastSubsystem(SubsystemType::ResourceManager);
		//
		//		// Unload level.
		//		ResourceIdentifier ident;
		//		ident.tid  = m_currentLevel->GetTID();
		//		ident.sid  = m_currentLevel->GetSID();
		//		ident.path = m_currentLevel->GetPath();
		//		rm->UnloadResources({ident});
		//
		//		m_system->DispatchEvent(EVS_LevelUninstalled, data);
		//		m_currentLevel = nullptr;
		//	};
		//
		//	if (immediate)
		//		uninstall();
	}

	void WorldManager::QueueLevel(const char* level)
	{
		// m_queuedLevel		= level;
		// m_queuedLevelExists = true;
	}

	void WorldManager::Simulate(float fixedDelta)
	{
	}

	void WorldManager::Tick(float deltaTime)
	{
		if (!m_loadedWorld)
			return;

		m_loadedWorld->Tick(deltaTime);

		// if (m_currentLevel != nullptr)
		// 	m_currentLevel->GetWorld()->Tick(deltaTime);

		// if (m_currentLevel == nullptr && m_queuedLevelExists)
		// {
		// 	InstallLevel(m_queuedLevel.c_str());
		// 	m_queuedLevelExists = false;
		// 	m_queuedLevel.clear();
		// }
	}

	void WorldManager::WaitForSimulation()
	{
		//	if (m_currentLevel != nullptr)
		//		m_currentLevel->GetWorld()->WaitForSimulation();
	}

} // namespace Lina
