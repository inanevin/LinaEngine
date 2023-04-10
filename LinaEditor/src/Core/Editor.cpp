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
#include "GUI/SplashScreenGUIDrawer.hpp"
#include "GUI/MainWindowGUIDrawer.hpp"
#include "GUI/Nodes/GUINodeDockArea.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	void Editor::Initialize(const SystemInitializationInfo& initInfo)
	{
		Theme::s_resourceManagerInst = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	void Editor::Shutdown()
	{
		for (auto [sid, drawer] : m_guiDrawers)
			delete drawer;

		delete m_mainWindowGUIDrawer;
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
		auto sf				  = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN);
		m_mainWindowGUIDrawer = new SplashScreenGUIDrawer(this, sf->GetSwapchain());
		sf->SetGUIDrawer(m_mainWindowGUIDrawer);
	}

	void Editor::EndSplashScreen()
	{
		auto sf = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager)->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN);
		delete m_mainWindowGUIDrawer;
		m_mainWindowGUIDrawer = new MainWindowGUIDrawer(this, sf->GetSwapchain());
		sf->SetGUIDrawer(m_mainWindowGUIDrawer);

		m_editorImages = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager)->GetResource<Texture>("Resources/Editor/Textures/EditorImages.png"_hs)->GetSheetItems(EDITOR_IMAGES_SHEET_COLUMNS, EDITOR_IMAGES_SHEET_ROWS);
	}

	void Editor::OpenPanel(EditorPanel panel)
	{
		auto wm		= m_system->CastSubsystem<WindowManager>(SubsystemType::WindowManager);
		auto gfxMan = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		auto it = m_openPanels.find(panel);
		if (it != m_openPanels.end())
		{
			const StringID existing = it->second->GetSID();
			wm->GetWindow(existing)->BringToFront();
			return;
		}

		gfxMan->Join();

		StringID sid  = 0;
		String	 name = "";

		switch (panel)
		{
		case EditorPanel::DebugResourceView:
			sid	 = "ResourceViewer"_hs;
			name = "ResourceViewer";
			break;
		case EditorPanel::Entities:
			sid	 = "Entities"_hs;
			name = "Entities";
			break;
		case EditorPanel::Level:
			sid	 = "Level"_hs;
			name = "Level";
			break;
		case EditorPanel::Properties:
			sid	 = "Properties"_hs;
			name = "Properties";
			break;
		case EditorPanel::ContentBrowser:
			sid	 = "ContentBrowser"_hs;
			name = "Content Browser";
		case EditorPanel::Hierarchy:
			sid	 = "Hierarchy"_hs;
			name = "Hierarchy";
		default:
			LINA_NOTIMPLEMENTED;
		}

		auto window = wm->CreateAppWindow(sid, name.c_str(), Vector2i::Zero, Vector2i(500, 500), SRM_DrawGUI);
		window->SetStyle(WindowStyle::Borderless);
		window->SetVisible(true);
		window->SetPos(Vector2i::Zero);

		auto surfaceRenderer = gfxMan->GetSurfaceRenderer(sid);
		auto guiDrawer		 = new EditorGUIDrawer(this, surfaceRenderer->GetSwapchain(), panel, sid);
		gfxMan->GetSurfaceRenderer(sid)->SetGUIDrawer(guiDrawer);
		m_guiDrawers[sid]	= guiDrawer;
		m_openPanels[panel] = guiDrawer;

		guiDrawer->GetFirstDockArea()->AddNewPanel(panel);
	}

	void Editor::ClosePanel(EditorPanel panel)
	{
		auto		   guiDrawer = m_openPanels[panel];
		const StringID sid		 = guiDrawer->GetSID();
		auto		   gfxMan	 = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		auto		   wm		 = m_system->CastSubsystem<WindowManager>(SubsystemType::WindowManager);

		gfxMan->DestroySurfaceRenderer(sid);
		wm->DestroyAppWindow(sid);
		delete guiDrawer;
		m_openPanels.erase(m_openPanels.find(panel));
	}

} // namespace Lina::Editor
