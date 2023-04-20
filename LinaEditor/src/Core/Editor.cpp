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

#include "Core/Editor.hpp"
#include "Resources/Core/CommonResources.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Serialization/Serialization.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Serialization/Compressor.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "GUI/Drawers/GUIDrawerSplashScreen.hpp"
#include "GUI/Drawers/GUIDrawerMainWindow.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Core/Theme.hpp"
#include "GUI/Nodes/Panels/GUIPanelFactory.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "GUI/Drawers/GUIDrawerChildWindow.hpp"

namespace Lina::Editor
{
	void Editor::Initialize(const SystemInitializationInfo& initInfo)
	{
		Theme::s_resourceManagerInst = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_gfxManager				 = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_windowManager				 = m_system->CastSubsystem<WindowManager>(SubsystemType::WindowManager);
		m_input						 = m_system->CastSubsystem<Input>(SubsystemType::Input);
		m_payloadManager.Initialize();
	}

	void Editor::Shutdown()
	{
		for (auto [sid, drawer] : m_guiDrawers)
			delete drawer;

		m_payloadManager.Shutdown();
	}

	void Editor::PackageResources(const Vector<ResourceIdentifier>& identifiers)
	{
		ResourceManager* rm = static_cast<ResourceManager*>(m_system->CastSubsystem(SubsystemType::ResourceManager));

		HashMap<PackageType, Vector<ResourceIdentifier>> resourcesPerPackage;

		for (const auto& ident : identifiers)
		{
			PackageType pt = rm->GetPackageType(ident.tid);
			resourcesPerPackage[pt].push_back(ident);
		}

		for (auto [packageType, resources] : resourcesPerPackage)
		{
			const String packagePath = GGetPackagePath(packageType);
			OStream		 stream;
			stream.CreateReserve(1000);

			for (auto r : resources)
			{
				// Header
				stream << r.tid;
				stream << r.sid;

				const String metacachePath = ResourceManager::GetMetacachePath(r.path, r.sid);
				if (FileSystem::FileExists(metacachePath))
				{
					IStream		 cache = Serialization::LoadFromFile(metacachePath.c_str());
					const uint32 size  = static_cast<uint32>(cache.GetSize());
					stream << size;
					stream.WriteEndianSafe(cache.GetDataRaw(), cache.GetSize());
					cache.Destroy();
				}
				else
				{
					OStream outStream;
					outStream.CreateReserve(512);

					// Load into stream & destroy.
					IResource* res = static_cast<IResource*>(ReflectionSystem::Get().Resolve(r.tid).GetFunction<void*()>("CreateMock"_hs)());
					// res->LoadFromFile(r.path.c_str());
					// res->SaveToStream(outStream);
					// res->Flush();
					ReflectionSystem::Get().Resolve(r.tid).GetFunction<void(void*)>("DestroyMock"_hs)(static_cast<void*>(res));

					// Write stream to package & destroy.
					const uint32 size = static_cast<uint32>(outStream.GetCurrentSize());
					stream << size;
					stream.WriteRaw(outStream.GetDataRaw(), outStream.GetCurrentSize());
					outStream.Destroy();
				}
			}

			if (!FileSystem::FileExists("Resources/Packages"))
				FileSystem::CreateFolderInPath("Resources/Packages");

			Serialization::SaveToFile(packagePath.c_str(), stream);
		}
	}

	void Editor::BeginSplashScreen()
	{
		auto sf				  = m_gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN);
		m_guiDrawerMainWindow = new GUIDrawerSplashScreen(this, sf->GetSwapchain());
		sf->SetGUIDrawer(m_guiDrawerMainWindow);
	}

	void Editor::EndSplashScreen()
	{
		auto sf = m_gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN);
		delete m_guiDrawerMainWindow;
		m_guiDrawerMainWindow = new GUIDrawerMainWindow(this, sf->GetSwapchain());
		sf->SetGUIDrawer(m_guiDrawerMainWindow);
		m_guiDrawers[LINA_MAIN_SWAPCHAIN] = m_guiDrawerMainWindow;

		m_editorImages = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->GetResource<Texture>("Resources/Editor/Textures/EditorImages.png"_hs)->GetSheetItems(EDITOR_IMAGES_SHEET_COLUMNS, EDITOR_IMAGES_SHEET_ROWS);
	}

	void Editor::Tick()
	{
		m_payloadManager.Tick();

		// Window create & destroy requests.
		{
			if (!m_createWindowRequests.empty() || !m_deleteWindowRequests.empty())
				m_gfxManager->Join();

			for (auto& req : m_deleteWindowRequests)
			{
				m_windowManager->DestroyAppWindow(req.sid);
				auto it = m_guiDrawers.find(req.sid);

				delete it->second;
				m_guiDrawers.erase(it);
			}

			for (auto& req : m_createWindowRequests)
			{
				auto window = m_windowManager->CreateAppWindow(req.sid, req.title.c_str(), Vector2i::Zero, Vector2i(500, 500), SRM_DrawGUI);
				window->SetStyle(WindowStyle::Borderless);
				window->SetVisible(true);
				window->SetPos(Vector2i::Zero);

				auto surfaceRenderer = m_gfxManager->GetSurfaceRenderer(req.sid);
				auto guiDrawer		 = new GUIDrawerChildWindow(this, surfaceRenderer->GetSwapchain());
				surfaceRenderer->SetGUIDrawer(guiDrawer);
				m_guiDrawers[req.sid] = guiDrawer;

				auto targetDockArea = guiDrawer->GetFirstDockArea();
				auto createdPanel	= GUIPanelFactory::CreatePanel(req.panelType, targetDockArea, req.title, req.sid);
				targetDockArea->AddPanel(static_cast<GUINodePanel*>(createdPanel));
			}

			m_createWindowRequests.clear();
			m_deleteWindowRequests.clear();
		}

		// Controlling alpha of the current dragged window according to dock-hovers
		{
			if (m_draggedWindow)
			{
				bool dockPreviewActive = false;
				for (auto [sid, drawer] : m_guiDrawers)
				{
					if (drawer->GetDockPreview()->GetIsActive())
					{
						dockPreviewActive = true;
						break;
					}

					const auto& dockAreas = drawer->GetDockAreas();
					for (auto d : dockAreas)
					{
						if (d->GetDockPreview()->GetIsActive())
						{
							dockPreviewActive = true;
							break;
						}
					}
				}

				if (dockPreviewActive && !m_draggedWindow->GetIsTransparent())
					m_draggedWindow->SetAlpha(0.3f);
				else if (!dockPreviewActive && m_draggedWindow->GetIsTransparent())
					m_draggedWindow->SetAlpha(1.0f);
			}
		}
	}

	void Editor::OpenPanel(EditorPanel panel, const String& title, StringID sid)
	{
		auto wm		= m_system->CastSubsystem<WindowManager>(SubsystemType::WindowManager);
		auto gfxMan = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		const String   panelName = title;
		const StringID panelSID	 = sid;

		for (auto& [sid, guiDrawer] : m_guiDrawers)
		{
			if (guiDrawer == m_guiDrawerMainWindow)
				continue;

			GUINode* node = guiDrawer->FindNode(panelSID);
			if (node)
			{
				wm->GetWindow(sid)->BringToFront();
				return;
			}
		}

		CreateWindowRequest req = CreateWindowRequest{
			.panelType = panel,
			.sid	   = sid,
			.title	   = title,
		};

		m_createWindowRequests.push_back(req);
	}

	void Editor::CloseWindow(StringID sid)
	{
		DeleteWindowRequest req = {sid};
		m_deleteWindowRequests.push_back(req);
	}

	void Editor::OnWindowDrag(GUIDrawerBase* owner, bool isDragging)
	{
		if (!isDragging && m_draggedWindow && m_draggedWindow->GetIsTransparent())
			m_draggedWindow->SetAlpha(1.0f);

		// Only single dock area windows can be docked to others.
		if (!owner->GetDockAreas()[0]->GetIsAlone())
		{
			m_draggedWindow = isDragging ? owner->GetWindow() : nullptr;
			return;
		}

		GUIDrawerBase*	 drawerToDock	  = nullptr;
		GUINodeDockArea* splittedDockArea = nullptr;
		DockSplitType	 split			  = DockSplitType::None;

		for (auto [sid, drawer] : m_guiDrawers)
		{
			if (owner == drawer)
				continue;

			if (drawerToDock == nullptr)
			{
				split = drawer->GetDockPreview()->GetCurrentSplitType();
				if (split != DockSplitType::None)
				{
					drawerToDock = drawer;
				}
				else
				{
					const auto& dockAreas = drawer->GetDockAreas();
					for (auto d : dockAreas)
					{
						split = d->GetDockPreview()->GetCurrentSplitType();
						if (split != DockSplitType::None)
						{
							drawerToDock	 = drawer;
							splittedDockArea = d;
							break;
						}
					}
				}
			}

			drawer->SetDockPreviewEnabled(isDragging);
		}

		if (drawerToDock)
		{
			drawerToDock->SplitDockArea(splittedDockArea, split, owner->GetFirstDockArea()->GetFirstPanel());
			CloseWindow(m_draggedWindow->GetSID());
			m_draggedWindow = nullptr;
		}
		else
			m_draggedWindow = isDragging ? owner->GetWindow() : nullptr;
	}

} // namespace Lina::Editor
