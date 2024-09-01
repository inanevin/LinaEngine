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

#include "Editor/Editor.hpp"
#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Graphics/WorldRendererExtEditor.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	SystemInitializationInfo Lina_GetInitInfo()
	{
		LinaGX::MonitorInfo monitor = LinaGX::Window::GetPrimaryMonitorInfo();

		const uint32 w = monitor.size.x / 4;
		const uint32 h = static_cast<uint32>(static_cast<float>(w) * (static_cast<float>(monitor.size.y) / static_cast<float>(monitor.size.x)));

		LinaGX::VSyncStyle vsync;
		vsync.dx12Vsync	  = LinaGX::DXVsync::None;
		vsync.vulkanVsync = LinaGX::VKVsync::None;

		return SystemInitializationInfo{
			.appName					 = "Lina Editor",
			.windowWidth				 = w,
			.windowHeight				 = h,
			.windowStyle				 = LinaGX::WindowStyle::BorderlessApplication,
			.vsyncStyle					 = vsync,
			.allowTearing				 = true,
			.appDelegate				 = new Lina::Editor::Editor(),
			.resourceManagerUseMetacache = false,
			.clearColor					 = Theme::GetDef().background0,
		};
	}
} // namespace Lina

namespace Lina::Editor
{
	Editor* Editor::s_editor = nullptr;

	bool Editor::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (sid == DEFAULT_FONT_SID || sid == DEFAULT_FONT_BOLD_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 14, .dpiLimit = 1.0f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
				.isSDF	= false,

			};
			stream << customMeta;
			return true;
		}

		if (sid == BIG_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 20, .dpiLimit = 10.1f}, {.size = 20, .dpiLimit = 1.8f}, {.size = 20, .dpiLimit = 10.0f}},
				.isSDF	= false,

			};
			stream << customMeta;
			return true;
		};

		// NOTE: 160, 380 is the glyph range for nunito sans

		if (sid == DEFAULT_TEXTURE_LINALOGO)
		{
			Texture::Metadata meta = {};

			stream << meta;
			return true;
		}

		if (sid == DEFAULT_SHADER_GUI_SID)
		{
			Shader::Metadata meta;

			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::UNDEFINED,
				.targets	  = {{.format = DEFAULT_RT_FORMAT}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
			};

			meta.variants["Swapchain"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::UNDEFINED,
				.targets	  = {{.format = DEFAULT_SWAPCHAIN_FORMAT}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
			};

			meta.drawIndirectEnabled		  = true;
			meta.descriptorSetAllocationCount = 1;
			stream << meta;
			return true;
		}

		if (sid == "Resources/Editor/Shaders/GUI/EditorGUI.linashader"_hs)
		{
			Shader::Metadata meta;

			meta.variants["Swapchain"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.depthFormat  = LinaGX::Format::UNDEFINED,
				.targets	  = {{.format = DEFAULT_SWAPCHAIN_FORMAT}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
			};

			meta.drawIndirectEnabled		  = true;
			meta.descriptorSetAllocationCount = 1;
			stream << meta;
			return true;
		}

		if (sid == DEFAULT_SHADER_GUI3D_SID)
		{
			Shader::Metadata meta;

			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable		 = false,
				.blendSrcFactor		 = LinaGX::BlendFactor::SrcAlpha,
				.blendDstFactor		 = LinaGX::BlendFactor::OneMinusSrcAlpha,
				.blendColorOp		 = LinaGX::BlendOp::Add,
				.blendSrcAlphaFactor = LinaGX::BlendFactor::One,
				.blendDstAlphaFactor = LinaGX::BlendFactor::One,
				.blendAlphaOp		 = LinaGX::BlendOp::Add,
				.depthTest			 = true,
				.depthWrite			 = true,
				.targets			 = {{.format = DEFAULT_RT_FORMAT}},
				.cullMode			 = LinaGX::CullMode::None,
				.frontFace			 = LinaGX::FrontFace::CCW,
			};

			meta.drawIndirectEnabled		  = true;
			meta.descriptorSetAllocationCount = 1;
			stream << meta;
			return true;
		}

		if (sid == DEFAULT_SHADER_OBJECT_SID)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = true,
				.depthTest	  = true,
				.depthWrite	  = true,
				.targets	  = {{.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}, {.format = DEFAULT_RT_FORMAT}},
				.cullMode	  = LinaGX::CullMode::Back,
				.frontFace	  = LinaGX::FrontFace::CW,
			};

			meta.descriptorSetAllocationCount = 1;
			meta.drawIndirectEnabled		  = true;
			meta.materialSize				  = sizeof(GPUMaterialDefaultObject);
			stream << meta;
			return true;
		}

		if (sid == DEFAULT_SHADER_DEFERRED_LIGHTING_SID)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthTest	  = false,
				.depthWrite	  = false,
				.targets	  = {{.format = DEFAULT_RT_FORMAT}},
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CW,
			};
			meta.descriptorSetAllocationCount = 1;
			meta.drawIndirectEnabled		  = false;
			meta.materialSize				  = 0;
			stream << meta;
			return true;
		}

		if (sid == DEFAULT_SHADER_SKY_SID)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable	   = true,
				.depthTest		   = true,
				.depthWrite		   = false,
				.targets		   = {{.format = DEFAULT_RT_FORMAT}},
				.depthOp		   = LinaGX::CompareOp::Equal,
				.cullMode		   = LinaGX::CullMode::Back,
				.frontFace		   = LinaGX::FrontFace::CW,
				.depthBiasEnable   = true,
				.depthBiasConstant = 5.0f,
			};

			meta.descriptorSetAllocationCount = 1;
			meta.drawIndirectEnabled		  = false;
			meta.materialSize				  = sizeof(GPUMaterialDefaultSky);
			stream << meta;
			return true;
		}

		if (sid == ICON_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 32, .dpiLimit = 10.0f}},
				.isSDF	= true,
			};
			stream << customMeta;
			return true;
		}

		if (sid == ALT_FONT_SID || sid == ALT_FONT_BOLD_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
				.isSDF	= false,
			};
			stream << customMeta;
			return true;
		}

		if (sid == "Resources/Editor/Shaders/Lines.linashader"_hs)
		{
			Shader::Metadata meta;
			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = false,

				.depthTest	= true,
				.depthWrite = true,
				.targets	= {{.format = DEFAULT_RT_FORMAT}},
				.cullMode	= LinaGX::CullMode::None,
				.frontFace	= LinaGX::FrontFace::CCW,
			};

			meta.drawIndirectEnabled		  = false;
			meta.descriptorSetAllocationCount = 1;
			stream << meta;
			return true;
		}
		return false;
	}

	void Editor::PreInitialize()
	{
		m_editorRenderer.Initialize(this);

		const String metacachePath = FileManager::GetMetacachePath();

		if (!FileSystem::FileOrPathExists(metacachePath))
			FileSystem::CreateFolderInPath(metacachePath);

		Vector<ResourceIdentifier> priorityResources;
		priorityResources.push_back({"Resources/Editor/Shaders/GUI/EditorGUI.linashader", GetTypeID<Shader>()});
		priorityResources.push_back({"Resources/Editor/Textures/LinaLogoTitle.png", GetTypeID<Texture>()});
		priorityResources.push_back({ICON_FONT_PATH, GetTypeID<Font>()});
		priorityResources.push_back({DEFAULT_FONT_PATH, GetTypeID<Font>()});
		priorityResources.push_back({DEFAULT_FONT_BOLD_PATH, GetTypeID<Font>()});
		m_resourceManagerV2.LoadResourcesFromFile(this, 0, priorityResources, FileManager::GetMetacachePath());
		m_resourceManagerV2.WaitForAll();
	}

	void Editor::Initialize()
	{
		s_editor							  = this;
		Theme::GetDef().iconFont			  = ICON_FONT_SID;
		Theme::GetDef().defaultFont			  = DEFAULT_FONT_SID;
		Theme::GetDef().altFont				  = ALT_FONT_BOLD_SID;
		Theme::GetDef().iconDropdown		  = ICON_ARROW_DOWN;
		Theme::GetDef().iconCircleFilled	  = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconSliderHandle	  = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconColorWheelPointer = ICON_CIRCLE;

		m_resourceManagerV2.AddListener(this);
		m_worldManager = m_app->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager);
		m_gfxManager   = m_app->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		m_fileManager.Initialize(this);
		m_atlasManager.Initialize(this);
		m_windowPanelManager.Initialize(this);

		m_mainWindow		   = m_gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_primaryWidgetManager = &m_windowPanelManager.GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		// Push splash
		Widget*		  root	 = m_primaryWidgetManager->GetRoot();
		SplashScreen* splash = root->GetWidgetManager()->Allocate<SplashScreen>();
		splash->Initialize();
		root->AddChild(splash);

		// Load editor settings or create and save empty if non-existing.
		const String userDataFolder = FileSystem::GetUserDataFolder() + "Editor/";
		const String settingsPath	= userDataFolder + "EditorSettings.linameta";
		m_settings.SetPath(settingsPath);
		if (!FileSystem::FileOrPathExists(userDataFolder))
			FileSystem::CreateFolderInPath(userDataFolder);

		if (FileSystem::FileOrPathExists(settingsPath))
			m_settings.LoadFromFile();
		else
			m_settings.SaveToFile();

		Vector<ResourceIdentifier> priorityResources;
		priorityResources.push_back({"Resources/Editor/Shaders/Lines.linashader", GetTypeID<Shader>()});
		priorityResources.push_back({"Resources/Editor/Textures/Checkered.png", GetTypeID<Texture>()});
		priorityResources.push_back({ALT_FONT_PATH, GetTypeID<Font>()});
		priorityResources.push_back({ALT_FONT_BOLD_PATH, GetTypeID<Font>()});
		priorityResources.push_back({BIG_FONT_PATH, GetTypeID<Font>()});
		m_resourceManagerV2.LoadResourcesFromFile(this, RLID_CORE_RES, priorityResources, FileManager::GetMetacachePath());

		// Async load core resources.
		// Vector<ResourceIdentifier> list;
		// const String			   fullPathBase = FileSystem::GetRunningDirectory() + "/";
		// list.push_back({fullPathBase + DEFAULT_SHADER_DEFERRED_LIGHTING_PATH, DEFAULT_SHADER_DEFERRED_LIGHTING_PATH, // GetTypeID<Shader>()});
		// list.push_back({fullPathBase + "Resources/Core/Models/Plane.glb", "Resources/Core/Models/Plane.glb", GetTypeID<Model>()});
		// list.push_back({fullPathBase + "Resources/Core/Models/Cube.glb", "Resources/Core/Models/Cube.glb", GetTypeID<Model>()});
		// list.push_back({fullPathBase + "Resources/Core/Models/Sphere.glb", "Resources/Core/Models/Sphere.glb", GetTypeID<Model>()});
		// list.push_back({fullPathBase + "Resources/Core/Models/SkyCube.glb", "Resources/Core/Models/SkyCube.glb", GetTypeID<Model>()});
		// list.push_back({fullPathBase + ALT_FONT_PATH, ALT_FONT_PATH, GetTypeID<Font>()});
		// list.push_back({fullPathBase + BIG_FONT_PATH, BIG_FONT_PATH, GetTypeID<Font>()});
		// list.push_back({fullPathBase + ALT_FONT_BOLD_PATH, ALT_FONT_BOLD_PATH, GetTypeID<Font>()});
		// list.push_back({fullPathBase + "Resources/Editor/Textures/LinaLogoTitleHorizontal.png", // "Resources/Editor/Textures/LinaLogoTitleHorizontal.png", GetTypeID<Texture>()});
		// list.push_back({fullPathBase + "Resources/Editor/Shaders/Lines.linashader", "Resources/Editor/Shaders/Lines.linashader", // GetTypeID<Shader>()});
		// m_rm->LoadResourcesFromFile(RLID_CORE_RES, list, metacachePath);
		// m_coreResourcesOK = true;
	}

	void Editor::OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& size)
	{
		m_gfxManager->Join();
		m_windowPanelManager.OnWindowSizeChanged(window, size);
	}
	void Editor::PreTick()
	{
		m_resourceManagerV2.Poll();

		if (m_editorRoot == nullptr && m_coreResourcesOK)
		{
			CoreResourcesLoaded();
		}

		m_windowPanelManager.PreTick();
		m_fileManager.PreTick();
		m_editorRenderer.PreTick();
	}

	void Editor::Tick(float delta)
	{
		m_editorRenderer.Tick(delta);
	}

	void Editor::Render(uint32 frameIndex)
	{
		m_editorRenderer.Render(frameIndex);
	}

	void Editor::CoreResourcesLoaded()
	{
		// Remove splash
		Widget* root   = m_primaryWidgetManager->GetRoot();
		Widget* splash = Widget::GetWidgetOfType<SplashScreen>(root);
		root->RemoveChild(splash);
		root->GetWidgetManager()->Deallocate(splash);

		// Resize window to work dims.
		m_mainWindow->SetPosition(m_mainWindow->GetMonitorInfoFromWindow().workTopLeft);
		m_mainWindow->AddSizeRequest(m_mainWindow->GetMonitorWorkSize());

		// Insert editor root.
		m_editorRoot = root->GetWidgetManager()->Allocate<EditorRoot>("EditorRoot");
		m_editorRoot->Initialize();
		root->AddChild(m_editorRoot);

		// Launch project
		m_projectManager.Initialize(this);
		m_settings.GetLayout().ApplyStoredLayout();

		// const String& lastWorldPath = m_settings.GetLastWorldAbsPath();
		// if (FileSystem::FileOrPathExists(lastWorldPath))
		// {
		//     EntityWorld* world = new EntityWorld();
		// 	m_worldManager->InstallWorld(lastWorldPath);
		// 	CreateWorldRenderer(m_worldManager->GetMainWorld());
		// }
	}

	void Editor::PreShutdown()
	{
		m_resourceManagerV2.RemoveListener(this);
		m_editorRenderer.Shutdown();
		// DestroyWorldRenderer(m_worldManager->GetMainWorld());
		m_worldManager->UninstallMainWorld();
		m_fileManager.Shutdown();
		m_atlasManager.Shutdown();
		m_windowPanelManager.Shutdown();
		m_projectManager.Shutdown();

		m_resourceManagerV2.Shutdown();
	}

	void Editor::RequestExit()
	{
		SaveSettings();
		m_app->Quit();
	}

	void Editor::SaveSettings()
	{
		auto* loadedWorld = m_app->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->GetMainWorld();
		if (loadedWorld)
			m_settings.SetLastWorldAbsPath(loadedWorld->GetPath());

		m_settings.GetLayout().StoreLayout();
		m_settings.SaveToFile();
	}

	void Editor::CreateWorldRenderer(EntityWorld* world)
	{
		WorldRenderer* wr = new WorldRenderer(world, m_gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->GetSize());
		m_editorRenderer.AddWorldRenderer(wr);
		m_worldRenderers[world] = wr;
	}

	void Editor::DestroyWorldRenderer(EntityWorld* world)
	{
		auto it = m_worldRenderers.find(world);
		LINA_ASSERT(it != m_worldRenderers.end(), "");
		WorldRenderer* wr = it->second;
		m_editorRenderer.RemoveWorldRenderer(wr);
		// m_gfxManager->RemoveRenderer(wr);
		delete wr;
		m_worldRenderers.erase(it);
	}

	WorldRenderer* Editor::GetWorldRenderer(EntityWorld* world)
	{
		return m_worldRenderers.at(world);
	}

	void Editor::OnResourceLoadEnded(int32 taskID, const Vector<Resource*>& resources)
	{
		if (m_coreResourcesOK)
			return;

		if (taskID == RLID_CORE_RES)
		{
			m_coreResourcesOK = true;
		}
	}

} // namespace Lina::Editor
