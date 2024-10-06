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
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Graphics/WorldRendererExtEditor.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Application.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
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

	bool Editor::FillResourceCustomMeta(ResourceID id, OStream& stream)
	{
		if (id == EDITOR_FONT_ROBOTO_ID || id == EDITOR_FONT_ROBOTO_BOLD_ID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 14, .dpiLimit = 1.0f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
				.isSDF	= false,

			};
			stream << customMeta;
			return true;
		}

		if (id == EDITOR_FONT_PLAY_BIG_ID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 20, .dpiLimit = 10.1f}, {.size = 20, .dpiLimit = 1.8f}, {.size = 20, .dpiLimit = 10.0f}},
				.isSDF	= false,

			};
			stream << customMeta;
			return true;
		};
		// NOTE: 160, 380 is the glyph range for nunito sans

		if (id == EDITOR_SHADER_GUI_ID)
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

			meta.drawIndirectEnabled = true;
			stream << meta;
			return true;
		}
		/*
				if (id == DEFAULT_SHADER_GUI3D_ID)
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
		*/
		if (id == EDITOR_SHADER_DEFAULT_OBJECT_ID)
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

			meta.drawIndirectEnabled = true;
			stream << meta;
			return true;
		}

		if (id == EDITOR_SHADER_DEFAULT_LIGHTING_ID)
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
			meta.drawIndirectEnabled = false;
			stream << meta;
			return true;
		}

		if (id == EDITOR_SHADER_DEFAULT_SKY_ID)
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

			meta.drawIndirectEnabled = false;
			stream << meta;
			return true;
		}

		if (id == EDITOR_FONT_ICON_ID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 32, .dpiLimit = 10.0f}},
				.isSDF	= true,
			};
			stream << customMeta;
			return true;
		}

		if (id == EDITOR_FONT_PLAY_ID || id == EDITOR_FONT_PLAY_BOLD_ID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
				.isSDF	= false,
			};
			stream << customMeta;
			return true;
		}

		if (id == EDITOR_SHADER_LINES_ID)
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

			meta.drawIndirectEnabled = false;
			stream << meta;
			return true;
		}
		return false;
	}

	void Editor::PreInitialize()
	{
		m_editorRenderer.Initialize(this);

		ResourceDefinitionList priorityResources;
		priorityResources.insert({EDITOR_SHADER_GUI_ID, EDITOR_SHADER_GUI_PATH, GetTypeID<Shader>()});
		priorityResources.insert({EDITOR_TEXTURE_LINA_LOGO_ID, EDITOR_TEXTURE_LINA_LOGO_PATH, GetTypeID<Texture>()});
		priorityResources.insert({EDITOR_TEXTURE_LINA_LOGO_BOTTOM_ID, EDITOR_TEXTURE_LINA_LOGO_BOTTOM_PATH, GetTypeID<Texture>()});
		priorityResources.insert({EDITOR_TEXTURE_LINA_LOGO_LEFT_ID, EDITOR_TEXTURE_LINA_LOGO_LEFT_PATH, GetTypeID<Texture>()});
		priorityResources.insert({EDITOR_TEXTURE_LINA_LOGO_RIGHT_ID, EDITOR_TEXTURE_LINA_LOGO_RIGHT_PATH, GetTypeID<Texture>()});
		priorityResources.insert({EDITOR_FONT_ICON_ID, EDITOR_FONT_ICON_PATH, GetTypeID<Font>()});
		priorityResources.insert({EDITOR_FONT_ROBOTO_ID, EDITOR_FONT_ROBOTO_PATH, GetTypeID<Font>()});
		priorityResources.insert({EDITOR_FONT_ROBOTO_BOLD_ID, EDITOR_FONT_ROBOTO_BOLD_PATH, GetTypeID<Font>()});
		priorityResources.insert({EDITOR_FONT_PLAY_BIG_ID, EDITOR_FONT_PLAY_BIG_PATH, GetTypeID<Font>()});
		const ResourceList resources = m_resourceManagerV2.LoadResourcesFromFile(this, priorityResources, [](uint32 loaded, const ResourceDef& def) {});
		m_editorRenderer.OnResourcesLoaded(resources);
	}

	void Editor::Initialize()
	{
		s_editor							  = this;
		Theme::GetDef().iconFont			  = EDITOR_FONT_ICON_ID;
		Theme::GetDef().defaultFont			  = EDITOR_FONT_ROBOTO_ID;
		Theme::GetDef().defaultBoldFont		  = EDITOR_FONT_ROBOTO_BOLD_ID;
		Theme::GetDef().altFont				  = EDITOR_FONT_PLAY_BOLD_ID;
		Theme::GetDef().altBigFont			  = EDITOR_FONT_PLAY_BIG_ID;
		Theme::GetDef().iconDropdown		  = ICON_ARROW_DOWN;
		Theme::GetDef().iconCircleFilled	  = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconSliderHandle	  = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconColorWheelPointer = ICON_CIRCLE;

		m_atlasManager.Initialize(this);
		m_windowPanelManager.Initialize(this);

		m_mainWindow		   = m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_primaryWidgetManager = &m_windowPanelManager.GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		Widget* lock	 = m_windowPanelManager.LockAllForegrounds(m_mainWindow, Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
		Widget* prog	 = CommonWidgets::BuildGenericPopupProgress(lock, Locale::GetStr(LocaleStr::LoadingEngine), true);
		Text*	progText = static_cast<Text*>(prog->FindChildWithDebugName("Progress"));
		lock->AddChild(prog);

		Taskflow tf;

		tf.emplace([this, progText]() {
			progText->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::LoadingSettings));

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

			progText->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::LoadingCoreResources));
			ResourceDefinitionList coreResources;
			coreResources.insert({EDITOR_TEXTURE_CHECKERED_ID, EDITOR_TEXTURE_CHECKERED_PATH, GetTypeID<Texture>()});
			coreResources.insert({EDITOR_TEXTURE_PROTOTYPE_DARK_ID, EDITOR_TEXTURE_PROTOTYPE_DARK_PATH, GetTypeID<Texture>()});
			coreResources.insert({EDITOR_FONT_PLAY_ID, EDITOR_FONT_PLAY_PATH, GetTypeID<Font>()});
			coreResources.insert({EDITOR_FONT_PLAY_BOLD_ID, EDITOR_FONT_PLAY_BOLD_PATH, GetTypeID<Font>()});
			HashSet<Resource*> resources = m_resourceManagerV2.LoadResourcesFromFile(this, coreResources, [progText](uint32 loaded, const ResourceDef& current) { progText->UpdateTextAndCalcSize(current.name); });

			progText->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::GeneratingAtlases));
			TextureAtlas* atlas0 = m_atlasManager.AddCustomAtlas("Resources/Editor/Textures/Atlas/ProjectIcons/", "ProjectIcons"_hs, Vector2ui(2048, 2048));

			resources.insert(atlas0->GetRaw());

			TaskRunner::QueueTask([this, resources]() {
				Application::GetLGX()->Join();
				m_editorRenderer.OnResourcesLoaded(resources);
				m_windowPanelManager.UnlockAllForegrounds();

				// Resize window to work dims.
				m_mainWindow->SetPosition(m_mainWindow->GetMonitorInfoFromWindow().workTopLeft);
				m_mainWindow->AddSizeRequest(m_mainWindow->GetMonitorWorkSize());

				// Insert editor root.
				Widget* root = m_primaryWidgetManager->GetRoot();
				m_editorRoot = root->GetWidgetManager()->Allocate<EditorRoot>("EditorRoot");
				m_editorRoot->Initialize();
				root->AddChild(m_editorRoot);

				// Launch project
				m_projectManager.Initialize(this);
				m_settings.GetLayout().ApplyStoredLayout();
			});
		});

		m_executor.RunMove(tf);
	}

	void Editor::OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& size)
	{
		m_app->GetLGX()->Join();
		m_windowPanelManager.OnWindowSizeChanged(window, size);
	}
	void Editor::PreTick()
	{
		TaskRunner::Poll();
		m_resourceManagerV2.Poll();
		m_projectManager.PreTick();
		m_windowPanelManager.PreTick();
		m_editorRenderer.PreTick();
	}

	void Editor::Tick(float delta)
	{
		m_editorRenderer.Tick(delta);
	}

	void Editor::Render()
	{
		m_editorRenderer.Render();
	}

	void Editor::PreShutdown()
	{
		m_editorRenderer.Shutdown();
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
		m_settings.GetLayout().StoreLayout();
		m_settings.SaveToFile();
	}

	void Editor::CreateWorldRenderer(EntityWorld* world)
	{
		WorldRenderer* wr = new WorldRenderer(world, m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->GetSize());
		m_editorRenderer.AddWorldRenderer(wr);
		m_worldRenderers[world] = wr;
	}

	void Editor::DestroyWorldRenderer(EntityWorld* world)
	{
		auto it = m_worldRenderers.find(world);
		LINA_ASSERT(it != m_worldRenderers.end(), "");
		WorldRenderer* wr = it->second;
		m_editorRenderer.RemoveWorldRenderer(wr);
		delete wr;
		m_worldRenderers.erase(it);
	}

	WorldRenderer* Editor::GetWorldRenderer(EntityWorld* world)
	{
		return m_worldRenderers.at(world);
	}

} // namespace Lina::Editor
