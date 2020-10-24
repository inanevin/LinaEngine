/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Core/GUILayer.hpp"
#include "Core/Application.hpp"
#include "Utility/Log.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Rendering/RenderEngine.hpp"
#include "World/DefaultLevel.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/EditorApplication.hpp"
#include "Utility/EditorUtility.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/implot/implot.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "IconsMaterialDesign.h"


#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static ImGuiTreeNodeFlags s_baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static bool s_showIMGUIDemo;
static bool s_setDockspaceLayout = true;
static bool s_physicsDebugEnabled = false;
static bool s_dockWindowInit = true;
static const char* s_saveLevelDialogID = "id_saveLevel";
static const char* s_loadLevelDialogID = "id_loadLevel";

namespace LinaEditor
{

	GUILayer::~GUILayer()
	{
		LINA_CLIENT_TRACE("[Destructor] -> GUI Layer ({0})", typeid(*this).name());
	}

	void GUILayer::Attach()
	{
		LINA_CLIENT_INFO("Editor GUI Layer Attached");

		// Listen to menu bar clicked events.
		EditorApplication::GetEditorDispatcher().SubscribeAction<MenuBarItems>("##linaeditor_menubarclicked", LinaEngine::Action::ActionType::MenuItemClicked, std::bind(&GUILayer::DispatchMenuBarClickedAction, this, std::placeholders::_1));

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Add default font.
		io.Fonts->AddFontFromFileTTF("resources/editor/fonts/Mukta-Medium.ttf", 20.0f, NULL);

		// merge in icons from Font Awesome
		static const ImWchar icons_rangesFA[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		static const ImWchar icons_rangesFK[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
		static const ImWchar icons_rangesMD[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };

		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;

		io.Fonts->AddFontFromFileTTF("resources/editor/fonts/FontAwesome/fa-solid-900.ttf", 20.0f, &icons_config, icons_rangesFA);
		io.Fonts->AddFontFromFileTTF("resources/editor/fonts/ForkAwesome/forkawesome-webfont.ttf", 30.0f, &icons_config, icons_rangesFK);
		io.Fonts->AddFontFromFileTTF("resources/editor/fonts/MaterialIcons/MaterialIcons-Regular.ttf", 30.0f, &icons_config, icons_rangesMD);

		// Setup configuration flags.
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		GLFWwindow* window = static_cast<GLFWwindow*>(LinaEngine::Application::GetAppWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		// Setup Dear ImGui style
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = ImGui::GetStyle().Colors;
		style.AntiAliasedFill = false;
		style.WindowRounding = 0.0f;
		style.TabRounding = 3.0f;
		style.ChildRounding = 0.0f;
		style.PopupRounding = 3.0f;
		style.FrameRounding = 3.0f;
		style.ScrollbarRounding = 5.0f;
		style.FramePadding = ImVec2(0, 5);
		style.WindowPadding = ImVec2(0, 0);
		style.ItemInnerSpacing = ImVec2(8, 4);
		style.GrabRounding = 6.0f;
		style.ChildBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.17f, 0.18f, 0.54f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.17f, 0.17f, 0.18f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.34f, 0.46f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.46f, 0.34f, 0.47f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.17f, 0.17f, 0.18f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.17f, 0.17f, 0.18f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.45f, 0.28f, 0.46f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.61f);

		ImPlot::GetStyle().AntiAliasedLines = true;

		s_setDockspaceLayout = true;

		// Set GUI draw params.
		m_drawParameters.useScissorTest = false;
		m_drawParameters.useDepthTest = true;
		m_drawParameters.useStencilTest = true;
		m_drawParameters.primitiveType = Graphics::PrimitiveType::PRIMITIVE_TRIANGLES;
		m_drawParameters.faceCulling = Graphics::FaceCulling::FACE_CULL_BACK;
		m_drawParameters.sourceBlend = Graphics::BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_drawParameters.destBlend = Graphics::BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		m_drawParameters.shouldWriteDepth = true;
		m_drawParameters.depthFunc = Graphics::DrawFunc::DRAW_FUNC_LEQUAL;
		m_drawParameters.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
		m_drawParameters.stencilComparisonVal = 0;
		m_drawParameters.stencilTestMask = 0xFF;
		m_drawParameters.stencilWriteMask = 0xFF;
		m_drawParameters.stencilFail = Graphics::StencilOp::STENCIL_KEEP;
		m_drawParameters.stencilPass = Graphics::StencilOp::STENCIL_REPLACE;
		m_drawParameters.stencilPassButDepthFail = Graphics::StencilOp::STENCIL_KEEP;
		m_drawParameters.scissorStartX = 0;
		m_drawParameters.scissorStartY = 0;
		m_drawParameters.scissorWidth = 0;
		m_drawParameters.scissorHeight = 0;

		m_ecsPanel.Setup();
		m_headerPanel.Setup();
		m_logPanel.Setup();
		m_profilerPanel.Setup();
		m_propertiesPanel.Setup();
		m_scenePanel.Setup();
		m_resourcesPanel.Setup();

		// Imgui first frame initialization.
		Render();

		LinaEngine::Application::GetEngineDispatcher().SubscribeAction<LinaEngine::World::Level*>("##guilayer_level_init", LinaEngine::Action::ActionType::LevelInitialized,
			std::bind(&GUILayer::LevelInstalled, this, std::placeholders::_1));
	}

	void GUILayer::Detach()
	{
		LINA_CLIENT_INFO("Editor GUI Layer Detached");

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		ImPlot::DestroyContext();
	}

	void GUILayer::Render()
	{
		// Set draw params first.
		LinaEngine::Application::GetRenderEngine().SetDrawParameters(m_drawParameters);
	
		//Setup
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		m_headerPanel.Draw();
		DrawCentralDockingSpace();

		m_resourcesPanel.Draw();
		m_ecsPanel.Draw();
		m_scenePanel.Draw();
		m_logPanel.Draw();
		m_profilerPanel.Draw();
		m_propertiesPanel.Draw();

		if (s_showIMGUIDemo)
			ImGui::ShowDemoWindow(&s_showIMGUIDemo);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUILayer::DispatchMenuBarClickedAction(const MenuBarItems& item)
	{
		// File
		if (item == MenuBarItems::NewProject)
		{

		}
		else if (item == MenuBarItems::LoadProject)
		{

		}
		else if (item == MenuBarItems::SaveProject)
		{

		}

		// Edit

		// View

		// Level
		else if (item == MenuBarItems::NewLevelData)
		{
			// Create a new level.
			m_currentLevel = new DefaultLevel();
			LinaEngine::Application::GetApp().InstallLevel(*m_currentLevel);
			LinaEngine::Application::GetApp().InitializeLevel(*m_currentLevel);
		}
		else if (item == MenuBarItems::SaveLevelData)
		{
			if (m_currentLevel != nullptr)
			{
				std::string fullPath = "";
				fullPath = EditorUtility::SaveFile(".linaleveldata", LinaEngine::Application::GetAppWindow().GetNativeWindow());

				if (fullPath.compare("") != 0)
				{
					size_t lastIndex = fullPath.find_last_of("/");
					std::string folderPath = fullPath.substr(0, lastIndex);
					std::string fileName = fullPath.substr(lastIndex + 1);
					m_currentLevel->SerializeLevelData(folderPath, fileName);
				}
			}
		}
		else if (item == MenuBarItems::LoadLevelData)
		{
			if (m_currentLevel != nullptr)
			{
				std::string fullPath = "";
				fullPath = EditorUtility::OpenFile(".linaleveldata", LinaEngine::Application::GetAppWindow().GetNativeWindow());

				if (fullPath.compare("") != 0)
				{
					size_t lastIndex = fullPath.find_last_of("/");
					std::string folderPath = fullPath.substr(0, lastIndex);
					std::string fileName = EditorUtility::RemoveExtensionFromFilename(fullPath.substr(lastIndex + 1));
					m_currentLevel->DeserializeLevelData(folderPath, fileName);
				}
			}
		}

		// Panels.
		else if (item == MenuBarItems::ECSPanel)
			m_ecsPanel.Open();
		else if (item == MenuBarItems::ScenePanel)
			m_scenePanel.Open();
		else if (item == MenuBarItems::ResourcesPanel)
			m_resourcesPanel.Open();
		else if (item == MenuBarItems::PropertiesPanel)
			m_propertiesPanel.Open();
		else if (item == MenuBarItems::LogPanel)
			m_logPanel.Open();
		else if (item == MenuBarItems::ImGuiPanel)
			s_showIMGUIDemo = true;

		// Debug
		else if (item == MenuBarItems::DebugViewPhysics)
			LinaEngine::Application::GetPhysicsEngine().SetDebugDraw(s_physicsDebugEnabled);

		else if (item == MenuBarItems::DebugViewShadows)
			m_scenePanel.SetDrawMode(LinaEditor::ScenePanel::DrawMode::ShadowMap);

		else if (item == MenuBarItems::DebugViewNormal)
			m_scenePanel.SetDrawMode(LinaEditor::ScenePanel::DrawMode::FinalImage);

	}

	void GUILayer::Refresh()
	{
		m_ecsPanel.Refresh();
	}

	void GUILayer::LevelInstalled(LinaEngine::World::Level* level)
	{
		m_currentLevel = level;
	}

	void GUILayer::DrawFPSCounter(int corner)
	{
		// FIXME-VIEWPORT: Select a default viewport
		const float DISTANCE = 10.0f;
		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();   // Instead of using viewport->Pos we use GetWorkPos() to avoid menu bars, if any!
			ImVec2 work_area_size = viewport->GetWorkSize();
			ImVec2 window_pos = ImVec2((corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

		if (ImGui::Begin("FPSOverlay", NULL, window_flags))
		{
			ImGui::Text("FPS Overlay");
			ImGui::Separator();
			std::string fpsText = std::to_string(LinaEngine::Application::GetApp().GetCurrentFPS()) + " Frames per second";
			ImGui::Text(fpsText.c_str());
		}
		ImGui::End();
	}

	void GUILayer::DrawCentralDockingSpace()
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoCloseButton;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 size = ImVec2(viewport->GetWorkSize().x, viewport->GetWorkSize().y - GLOBAL_DOCKSPACE_BEGIN);
			ImVec2 pos = ImVec2(viewport->GetWorkPos().x, viewport->GetWorkPos().y + GLOBAL_DOCKSPACE_BEGIN);
			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(HEADER_COLOR_BG.r, HEADER_COLOR_BG.g, HEADER_COLOR_BG.b, HEADER_COLOR_BG.a));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, GLOBAL_FRAMEPADDING_WINDOW);
		ImGui::Begin("DockSpace", NULL, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

			ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);

			if (s_setDockspaceLayout)
			{
				s_setDockspaceLayout = false;
				Vector2 screenSize = LinaEngine::Application::GetAppWindow().GetSize();
				ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
				ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(screenSize.x, screenSize.y));

				ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
				ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, NULL, &dock_main_id);
				ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
				ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

				ImGui::DockBuilderDockWindow(RESOURCES_ID, dock_id_prop);
				ImGui::DockBuilderDockWindow(ECS_ID, dock_id_prop);
				ImGui::DockBuilderDockWindow(SCENE_ID, dock_main_id);
				ImGui::DockBuilderDockWindow(LOG_ID, dock_id_bottom);
				ImGui::DockBuilderDockWindow(PROFILER_ID, dock_id_bottom);
				ImGui::DockBuilderDockWindow(PROPERTIES_ID, dock_id_right);
				ImGui::DockBuilderFinish(dockspace_id);

			}

		}

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::Begin("Background", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
		ImGui::End();
		ImGui::PopStyleColor();
	}
}