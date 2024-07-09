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
#include "Core/Graphics/Renderers/WorldRenderer.hpp"

#include "Core/Components/MeshComponent.hpp"
#include "Core/Components/WidgetComponent.hpp"
#include "Core/Components/CameraComponent.hpp"
#include "Core/Components/SimpleFlightMovement.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void WorldManager::Initialize(const SystemInitializationInfo& initInfo)
	{
	}

	void WorldManager::Shutdown()
	{
		m_activeWorlds.clear();
	}

	void WorldManager::SaveEmptyWorld(const String& absolutePath)
	{
		// EntityWorld world("", 0);
		// OStream		stream;
		// world.SaveToStream(stream);
		// Serialization::SaveToFile(absolutePath.c_str(), stream);
		// stream.Destroy();
	}

	// void WorldManager::ResizeWorldTexture(EntityWorld* world, const Vector2ui& newSize)
	// {
	// 	if (newSize.x == 0 || newSize.y == 0)
	// 		return;
	//
	// 	WorldRenderer* renderer = world->GetRenderer();
	//
	// 	if (renderer->GetSize() == newSize)
	// 		return;
	//
	// 	m_gfxManager->Join();
	// 	renderer->Resize(newSize);
	// }

	void WorldManager::InstallWorld(const String& path)
	{
		// m_gfxManager->Join();
		//
		// if (m_mainWorld)
		//	UninstallMainWorld();
		//
		// const auto sid = TO_SID(path);
		//// ResourceIdentifier ident(path, GetTypeID<EntityWorld>(), sid);
		//// m_rm->LoadResources({ident});
		//// m_rm->WaitForAll();
		//// m_mainWorld = m_rm->GetResource<EntityWorld>(sid);
		//
		// m_mainWorld = new EntityWorld("Test", 0);
		//
		// m_activeWorlds.push_back(m_mainWorld);
		//
		//// m_mainWorld->SetSkyMaterial(m_rm->GetResource<Material>(DEFAULT_MATERIAL_SKY_SID));
		//
		// for (auto* l : m_listeners)
		//	l->OnWorldInstalled(m_mainWorld);
		//
		// const float lim = 50.0f;

		// Entity* cameraEntity = m_mainWorld->CreateEntity("Camera");
		// cameraEntity->SetPosition(Vector3(0, 0, -20));
		// CameraComponent* camera = m_mainWorld->AddComponent<CameraComponent>(cameraEntity);
		// m_mainWorld->SetActiveCamera(camera);
		// SimpleFlightMovement* movement = m_mainWorld->AddComponent<SimpleFlightMovement>(cameraEntity);

		// Entity*		   test = m_mainWorld->CreateEntity("Cube");
		// MeshComponent* mesh = m_mainWorld->AddComponent<MeshComponent>(test);
		// mesh->SetMesh("Resources/Core/Models/Cube.glb"_hs, 0);
		// mesh->SetMaterial(DEFAULT_MATERIAL_OBJECT_SID);
		// test->SetPosition(Vector3(0, 0, 0));

		// Entity*		   plane	 = m_mainWorld->CreateEntity("Ground");
		// MeshComponent* planeMesh = m_mainWorld->AddComponent<MeshComponent>(plane);
		// planeMesh->SetMesh("Resources/Core/Models/Plane.glb"_hs, 0);
		// planeMesh->SetMaterial(DEFAULT_MATERIAL_OBJECT_SID);
		// plane->SetPosition(Vector3(0, 0, 0));
		// plane->SetScale(Vector3(100, 1, 100));

		//
		// for (uint32 i = 0; i < 200; i++)
		// {
		// 	Entity*		   test = m_mainWorld->CreateEntity("Cube");
		// 	MeshComponent* mesh = m_mainWorld->AddComponent<MeshComponent>(test);
		// 	mesh->SetMesh("Resources/Core/Models/Duck.glb"_hs, 0);
		// 	mesh->SetMaterial(DEFAULT_MATERIAL_OBJECT_SID);
		// 	test->SetPosition(Vector3(Math::RandF(-lim, lim), Math::RandF(-lim, lim), Math::RandF(-lim, lim)));
		// 	test->AddRotation(Vector3(Math::RandF(-180, 180), Math::RandF(-180, 180), Math::RandF(-180, 180)));
		// }

		{
			// Entity*			 text	= m_mainWorld->CreateEntity("Text");
			// WidgetComponent* widget = m_mainWorld->AddComponent<WidgetComponent>(text);
			// widget->SetWidget("EditorGizmo_BB"_hs);
			//
			// text->SetPosition(Vector3(0, 5, 0));
			// text->SetScale(Vector3(1, 1, 1));
			// text->SetRotationAngles(Vector3(0, 0, 0));
		}

		// for (uint32 i = 0; i < 50; i++)
		// {
		// 	Entity*		   test = m_mainWorld->CreateEntity("Cube");
		// 	MeshComponent* mesh = m_mainWorld->AddComponent<MeshComponent>(test);
		// mesh->SetMesh("Resources/Core/Models/duck.glb"_hs, 0);
		// 	mesh->SetMaterial(DEFAULT_MATERIAL_OBJECT_SID);
		// 	test->SetPosition(Vector3(Math::RandF(-lim, lim), Math::RandF(-lim, lim), Math::RandF(-lim, lim)));
		// 	test->AddRotation(Vector3(Math::RandF(-180, 180), Math::RandF(-180, 180), Math::RandF(-180, 180)));
		// }

		// loading the world resources, unloading the current worlds resources...
	}

	void WorldManager::UninstallMainWorld()
	{
		// if (m_mainWorld == nullptr)
		// 	return;
		//
		// for (auto* l : m_listeners)
		// 	l->OnWorldUninstalling(m_mainWorld);
		//
		// m_activeWorlds.erase(linatl::find_if(m_activeWorlds.begin(), m_activeWorlds.end(), [this](EntityWorld* w) -> bool { return w == m_mainWorld; }));
		//
		// auto* rm = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		// // ResourceIdentifier ident(m_mainWorld->GetPath(), GetTypeID<EntityWorld>(), m_mainWorld->GetSID());
		// // rm->UnloadResources({ident});
		// delete m_mainWorld;
		// m_mainWorld = nullptr;
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

	void WorldManager::AddWorld(EntityWorld* world)
	{
		m_activeWorlds.push_back(world);
	}

	void WorldManager::RemoveWorld(EntityWorld* world)
	{
		m_activeWorlds.erase(linatl::find_if(m_activeWorlds.begin(), m_activeWorlds.end(), [world](EntityWorld* w) -> bool { return w == world; }));
	}

	void WorldManager::PreTick()
	{
		if (m_activeWorlds.size() == 1)
			m_activeWorlds[0]->PreTick();
		else
		{
			Taskflow tf;
			tf.for_each(m_activeWorlds.begin(), m_activeWorlds.end(), [](EntityWorld* world) { world->PreTick(); });
			m_system->GetMainExecutor()->RunAndWait(tf);
		}
	}

	void WorldManager::Tick(float delta)
	{
		if (m_activeWorlds.size() == 1)
			m_activeWorlds[0]->Tick(delta);
		else
		{
			Taskflow tf;
			tf.for_each(m_activeWorlds.begin(), m_activeWorlds.end(), [delta](EntityWorld* world) { world->Tick(delta); });
			m_system->GetMainExecutor()->RunAndWait(tf);
		}
	}

} // namespace Lina
