/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: UILayer

*/


#include "World/Level.hpp"
#include "Core/GUILayer.hpp"
#include "Core/Application.hpp"
#include "Panels/ECSPanel.hpp"
#include "Panels/MaterialPanel.hpp"
#include "Panels/ResourcesPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/LogPanel.hpp"
#include "Panels/HeaderPanel.hpp"
#include "Utility/Log.hpp"
#include "Utility/EditorUtility.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/EditorCommon.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Rendering/Texture.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Rendering/RenderEngine.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "IconsFontAwesome5.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"


#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static bool showIMGUIDemo;
static bool setDockspaceLayout = true;
static bool physicsDebugEnabled = false;
static bool dockWindowInit = true;
static const char* saveLevelDialogID = "id_saveLevel";
static const char* loadLevelDialogID = "id_loadLevel";

#define DOCKSPACE_BEGIN 60

namespace LinaEditor
{
	void GUILayer::OnEvent()
	{

	}

	GUILayer::~GUILayer()
	{
		LINA_CLIENT_TRACE("[Destructor] -> GUI Layer ({0})", typeid(*this).name());

		for (int i = 0; i < m_panels.size(); i++)
			delete m_panels[i];
	}

	void GUILayer::DrawSplash()
	{
		// Set draw params first.
		m_renderEngine->SetDrawParameters(m_drawParameters);

		//Setup
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Text("saaaaaaaaa");

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUILayer::OnAttach()
	{
		LINA_CLIENT_INFO("Editor GUI Layer Attached");

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Add default font.
		io.Fonts->AddFontFromFileTTF("resources/fonts/Mukta-Medium.ttf", 20.0f, NULL);
		
		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		static const ImWchar icons_ranges2[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config; 
		ImFontConfig icons_config2; 
		icons_config.MergeMode = true; 
		icons_config.PixelSnapH = true;
		icons_config2.MergeMode = true;
		icons_config2.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF("resources/fonts/FontAwesome/fa-solid-900.ttf", 20.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF("resources/fonts/ForkAwesome/forkawesome-webfont.ttf", 30.0f, &icons_config, icons_ranges);
		
		// Setup configuration flags.
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();


		GLFWwindow* window = static_cast<GLFWwindow*>(m_appWindow->GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4* colors = ImGui::GetStyle().Colors;
		style.AntiAliasedFill = false;
		style.WindowRounding = 0.0f;
		style.TabRounding = 3.0f;
		style.ChildRounding = 0.0f;
		style.PopupRounding = 3.0f;
		style.FrameRounding = 0.0f;
		style.ScrollbarRounding = 5.0f;
		style.FramePadding = ImVec2(0, 5);
		style.WindowPadding = ImVec2(0, 0);
		style.ItemInnerSpacing = ImVec2(8, 4);
		style.GrabRounding = 6.0f;
		style.ChildBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.17f, 0.18f, 0.54f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.17f, 0.17f, 0.18f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
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
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.67f, 0.30f, 0.44f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.36f, 0.48f, 1.00f);
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
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);




		// setup panels, windows etc.
		m_ecsPanel = new ECSPanel(Vector2::Zero, Vector2(700, 600), *this);
		m_materialPanel = new MaterialPanel(Vector2::Zero, Vector2(700, 600), *this);
		m_resourcesPanel = new ResourcesPanel(Vector2::Zero, Vector2(700, 200), *this);
		m_scenePanel = new ScenePanel(Vector2::Zero, m_scenePanelSize, *this);
		m_propertiesPanel = new PropertiesPanel(Vector2::Zero, Vector2(700, 600), *this);
		m_logPanel = new LogPanel(Vector2::Zero, Vector2(700, 600), *this);
		m_headerPanel = new HeaderPanel(Vector2::Zero, Vector2::Zero, *this, m_appWindow->GetWindowProperties().m_title);

		// Add to the list.
		m_panels.push_back(m_ecsPanel);
		m_panels.push_back(m_materialPanel);
		m_panels.push_back(m_resourcesPanel);
		m_panels.push_back(m_scenePanel);
		m_panels.push_back(m_propertiesPanel);
		m_panels.push_back(m_logPanel);
		m_panels.push_back(m_headerPanel);

		// Setup panels.
		m_ecsPanel->Setup();
		m_materialPanel->Setup();
		m_resourcesPanel->Setup();
		m_scenePanel->Setup();
		m_propertiesPanel->Setup();
		m_logPanel->Setup();
		m_headerPanel->Setup();

		// Dockspace settings.
		setDockspaceLayout = true;

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

	}

	void GUILayer::OnDetach()
	{
		LINA_CLIENT_INFO("Editor GUI Layer Detached");

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		delete m_ecsPanel;
		delete m_materialPanel;
		delete m_resourcesPanel;
		delete m_scenePanel;
	}


	void GUILayer::OnTick(float dt)
	{
		// Set draw params first.
		m_renderEngine->SetDrawParameters(m_drawParameters);

		//Setup
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Top header.
		m_headerPanel->Draw(dt);

		// Level data dialogs.
		DrawLevelDataDialogs();

		// Draw main docking space.
		DrawCentralDockingSpace();

		// Draw overlay fps counter
		// DrawFPSCounter(1);

		//// Draw material panel.
		//m_MaterialPanel->Draw();


		// Draw resources panel
		m_resourcesPanel->Draw(dt);

		// Draw ECS Panel.
		m_ecsPanel->Draw(dt);

		// Draw Scene Panel
		m_scenePanel->Draw(dt);

		// Draw Log Panel
		m_logPanel->Draw(dt);

		// Draw properties panel
		m_propertiesPanel->Draw(dt);


		if (showIMGUIDemo)
			ImGui::ShowDemoWindow(&showIMGUIDemo);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


	void GUILayer::MenuBarItemClicked(const MenuBarItems& item)
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
		else if (item == MenuBarItems::SaveLevelData)
		{
			igfd::ImGuiFileDialog::Instance()->OpenDialog(saveLevelDialogID, "Choose File", ".linaleveldata", ".");
		}
		else if (item == MenuBarItems::LoadLevelData)
		{
			igfd::ImGuiFileDialog::Instance()->OpenDialog(loadLevelDialogID, "Choose File", ".linaleveldata", ".");
		}

		// Panels.
		else if (item == MenuBarItems::ECSPanel)
			m_ecsPanel->Open();
		else if (item == MenuBarItems::MaterialPanel)
			m_materialPanel->Open();
		else if (item == MenuBarItems::ScenePanel)
			m_scenePanel->Open();
		else if (item == MenuBarItems::ResourcesPanel)
			m_resourcesPanel->Open();
		else if (item == MenuBarItems::PropertiesPanel)
			m_propertiesPanel->Open();
		else if (item == MenuBarItems::LogPanel)
			m_logPanel->Open();
		else if (item == MenuBarItems::ImGuiPanel)
			showIMGUIDemo = true;

		// Debug
		else if (item == MenuBarItems::DebugViewPhysics)
			m_physicsEngine->SetDebugDraw(physicsDebugEnabled);

		else if (item == MenuBarItems::DebugViewShadows)
			m_scenePanel->SetDrawMode(LinaEditor::ScenePanel::DrawMode::ShadowMap);

		else if (item == MenuBarItems::DebugViewNormal)
			m_scenePanel->SetDrawMode(LinaEditor::ScenePanel::DrawMode::FinalImage);

	}

	void GUILayer::DrawLevelDataDialogs()
	{
		// Save level dialogue.
		if (igfd::ImGuiFileDialog::Instance()->FileDialog(saveLevelDialogID))
		{
			// action if OK
			if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
			{
				std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
				std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
				std::string fileName = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
				size_t lastIndex = fileName.find_last_of(".");
				std::string rawName = fileName.substr(0, lastIndex);

				m_currentLevel->SerializeLevelData(filePath, rawName, *m_currentLevel, *m_ecs);

				igfd::ImGuiFileDialog::Instance()->CloseDialog(saveLevelDialogID);
			}

			igfd::ImGuiFileDialog::Instance()->CloseDialog(saveLevelDialogID);
		}

		// Load level dialogue.
		if (igfd::ImGuiFileDialog::Instance()->FileDialog(loadLevelDialogID))
		{
			// action if OK
			if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
			{
				std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
				std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
				std::string fileName = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
				size_t lastIndex = fileName.find_last_of(".");
				std::string rawName = fileName.substr(0, lastIndex);

				// Load level data.
				World::Level::DeserializeLevelData(filePath, rawName, *m_currentLevel, *m_ecs);

				// Refresh ECS panel.
				m_ecsPanel->Refresh();

				igfd::ImGuiFileDialog::Instance()->CloseDialog(loadLevelDialogID);
			}

			igfd::ImGuiFileDialog::Instance()->CloseDialog(loadLevelDialogID);

		}
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
			std::string fpsText = std::to_string(m_application->GetCurrentFPS()) + " Frames per second";
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
			ImVec2 size = ImVec2(viewport->GetWorkSize().x, viewport->GetWorkSize().y - DOCKSPACE_BEGIN);
			ImVec2 pos = ImVec2(viewport->GetWorkPos().x, viewport->GetWorkPos().y + DOCKSPACE_BEGIN);
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

	

		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(HEADER_BG_COLOR.r, HEADER_BG_COLOR.g, HEADER_BG_COLOR.b, HEADER_BG_COLOR.a));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, WINDOW_FRAMEPADDING);

		ImGui::Begin("DockSpace", NULL, window_flags);

		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		
			ImGui::DockSpace(dockspace_id, ImVec2(0,0), dockspace_flags);

			if (setDockspaceLayout)
			{
				setDockspaceLayout = false;
				Vector2 screenSize = m_appWindow->GetSize();
				ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
				ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(screenSize.x, screenSize.y));

				ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
				ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, NULL, &dock_main_id);
				ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
				ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

				ImGui::DockBuilderDockWindow("Resources", dock_id_prop);
				ImGui::DockBuilderDockWindow("Entities", dock_id_prop);
				ImGui::DockBuilderDockWindow("Scene", dock_main_id);
				ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
				ImGui::DockBuilderDockWindow("Properties", dock_id_right);

				ImGui::DockBuilderFinish(dockspace_id);

			}

		}

		ImGui::End();
		ImGui::PopStyleVar();

	ImGui::Begin("Background", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
	
	ImGui::End();
	ImGui::PopStyleColor();
	}



	void GUILayer::DrawSkyboxSettingsWindow()
	{
		ImGui::Begin("Skybox Settings");

		ImGui::Separator();

		// Setup info.
		const char* items[] = { "Single Color", "Gradient", "Procedural", "Cubemap" };
		static int currentItemID = 0;
		static int previousCurrentItemID = 0;
		const char* label = items[currentItemID];
		static ImGuiComboFlags flags = 0;

		// Align label to left widget to right.
		int total_w = ImGui::GetContentRegionAvail().x; ImGui::Text("Skybox Type"); ImGui::SameLine(); ImGui::SetNextItemWidth(total_w - 100);

		// Draw combo box.
		if (ImGui::BeginCombo("", label, flags))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool isSelected = currentItemID == i;

				if (ImGui::Selectable(items[i], isSelected))
					currentItemID = i;

				// Set to focus.
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}


			ImGui::EndCombo();
		}


		ImGui::Separator();

		if (currentItemID == 0)
		{
			// Single color skybox
			/*Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Init colors.
			Color& skyboxColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::colorProperty);
			static ImVec4 color = ImVec4(skyboxColor.R(), skyboxColor.G(), skyboxColor.B(), skyboxColor.A());

			// Update skybox shader if changed.
			if (previousCurrentItemID != 0)
			{
				previousCurrentItemID = 0;;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxSingleColorShader);
				skyboxMaterial.SetColor(Graphics::MaterialConstants::colorProperty, Color(color.x, color.y, color.z, color.w));
			}

			// Show color picker.
			ImGui::Text("Color");
			ImGui::SameLine();


			bool isEditing = ImGui::ColorEdit4("Color##1", (float*)& color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// Update if changing the colors.
			if (isEditing)
				skyboxMaterial.SetColor(Graphics::MaterialConstants::colorProperty, Color(color.x, color.y, color.z, color.w));*/
		}
		else if (currentItemID == 1)
		{
			// Gradient color skybox
			/*Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Init colors.
			Color& skyboxStartColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::startColorProperty);
			Color& skyboxEndColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::endColorProperty);
			static ImVec4 colorStart = ImVec4(skyboxStartColor.R(), skyboxStartColor.G(), skyboxStartColor.B(), skyboxStartColor.A());
			static ImVec4 colorEnd = ImVec4(skyboxEndColor.R(), skyboxEndColor.G(), skyboxEndColor.B(), skyboxEndColor.A());

			// Update skybox shader if changed.
			if (previousCurrentItemID != 1)
			{
				previousCurrentItemID = 1;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxGradientShader);

				// Update colors.
				skyboxStartColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::startColorProperty);
				skyboxEndColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::endColorProperty);
				colorStart = ImVec4(skyboxStartColor.R(), skyboxStartColor.G(), skyboxStartColor.B(), skyboxStartColor.A());
				colorEnd = ImVec4(skyboxEndColor.R(), skyboxEndColor.G(), skyboxEndColor.B(), skyboxEndColor.A());

				// Set Colors.
				skyboxMaterial.SetColor(Graphics::MaterialConstants::startColorProperty, Color(colorStart.x, colorStart.y, colorStart.z, colorStart.w));
				skyboxMaterial.SetColor(Graphics::MaterialConstants::endColorProperty, Color(colorEnd.x, colorEnd.y, colorEnd.z, colorEnd.w));
			}

			ImGui::Text("End Color");
			ImGui::SameLine();
			bool isEditingEndColor = ImGui::ColorEdit4("Color##3", (float*)& colorEnd, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// Update if changing the color.
			if (isEditingEndColor)
				skyboxMaterial.SetColor(Graphics::MaterialConstants::endColorProperty, Color(colorEnd.x, colorEnd.y, colorEnd.z, colorEnd.w));


			// Show color pickers.
			ImGui::Text("Start Color");
			ImGui::SameLine();
			bool isEditingStartColor = ImGui::ColorEdit4("Color##2", (float*)& colorStart, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// Update if changing the color.
			if (isEditingStartColor)
				skyboxMaterial.SetColor(Graphics::MaterialConstants::startColorProperty, Color(colorStart.x, colorStart.y, colorStart.z, colorStart.w));*/

		}
		else if (currentItemID == 2)
		{
			// Procedural Skybox
		/*	Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Init colors.
			Color& skyboxStartColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::startColorProperty);
			Color& skyboxEndColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::endColorProperty);
			static ImVec4 colorStart = ImVec4(skyboxStartColor.R(), skyboxStartColor.G(), skyboxStartColor.B(), skyboxStartColor.A());
			static ImVec4 colorEnd = ImVec4(skyboxEndColor.R(), skyboxEndColor.G(), skyboxEndColor.B(), skyboxEndColor.A());


			// Update skybox shader if changed.
			if (previousCurrentItemID != 2)
			{
				previousCurrentItemID = 2;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxProceduralShader);

				// Update colors.
				skyboxStartColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::startColorProperty);
				skyboxEndColor = skyboxMaterial.GetColor(Graphics::MaterialConstants::endColorProperty);
				colorStart = ImVec4(skyboxStartColor.R(), skyboxStartColor.G(), skyboxStartColor.B(), skyboxStartColor.A());
				colorEnd = ImVec4(skyboxEndColor.R(), skyboxEndColor.G(), skyboxEndColor.B(), skyboxEndColor.A());

				// Set colors.
				skyboxMaterial.SetColor(Graphics::MaterialConstants::startColorProperty, Color(colorStart.x, colorStart.y, colorStart.z, colorStart.w));
				skyboxMaterial.SetColor(Graphics::MaterialConstants::endColorProperty, Color(colorEnd.x, colorEnd.y, colorEnd.z, colorEnd.w));
			}


			ImGui::Text("End Color");
			ImGui::SameLine();
			bool isEditingEndColor = ImGui::ColorEdit4("Color##5", (float*)& colorEnd, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// Update if changing the color.
			if (isEditingEndColor)
				skyboxMaterial.SetColor(Graphics::MaterialConstants::endColorProperty, Color(colorEnd.x, colorEnd.y, colorEnd.z, colorEnd.w));

			// Show color pickers.
			ImGui::Text("Start Color");
			ImGui::SameLine();
			bool isEditingStartColor = ImGui::ColorEdit4("Color##4", (float*)& colorStart, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			// Update if changing the color.
			if (isEditingStartColor)
				skyboxMaterial.SetColor(Graphics::MaterialConstants::startColorProperty, Color(colorStart.x, colorStart.y, colorStart.z, colorStart.w));*/

		}
		else if (currentItemID == 3)
		{
			// Cubemap Skybox
		/*	Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Update skybox shader if changed.
			if (previousCurrentItemID != 3)
			{
				previousCurrentItemID = 3;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxCubemapShader);
			}*/
		}

		ImGui::End();
	}

	void GUILayer::DrawContentBrowserWindow()
	{
		//ImGui::Begin("Content Browser");
		//
		//// Handle Right click popup.
		//if (ImGui::BeginPopupContextWindow())
		//{
		//	if (ImGui::BeginMenu("Create"))
		//	{
		//		if (ImGui::MenuItem("Folder"))
		//		{
		//			EditorUtility::CreateFolderInPath(EditorPathConstants::contentsPath + "NewFolder");
		//			ReadProjectContentsFolder();
		//		}
		//
		//		ImGui::Separator();
		//
		//		if (ImGui::MenuItem("Material"))
		//		{
		//
		//		}
		//		ImGui::EndMenu();
		//	}
		//
		//	ImGui::EndPopup();
		//}
		//
		//
		//ImGuiTreeNodeFlags node_flags = base_flags;
		//static int selection_mask = (1 << 2);
		//int node_clicked = -1;
		//
		//for (int i = 0; i < m_ContentFolders.size(); i++)
		//{
		//	const bool is_selected = (selection_mask & (1 << i)) != 0;
		//	if (is_selected)
		//		node_flags |= ImGuiTreeNodeFlags_Selected;
		//	bool node_open = ImGui::TreeNodeEx(m_ContentFolders[i].name.c_str());
		//
		//	if (ImGui::IsItemClicked())
		//		node_clicked = i;
		//	if (ImGui::BeginDragDropSource())
		//	{
		//		ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
		//		ImGui::Text("This is a drag and drop source");
		//		ImGui::EndDragDropSource();
		//	}
		//	if (node_open)
		//	{
		//		ImGui::BulletText("Blah blah\nBlah Blah");
		//		ImGui::TreePop();
		//	}
		//}
		//
		//
		//ImGui::End();

	}

	void GUILayer::ReadProjectContentsFolder()
	{
		// Get folders in directory.
		std::vector<std::string> rootFolders;
		EditorUtility::GetDirectories(rootFolders, EditorPathConstants::contentsPath);

		// Clear current folders.
		//m_ContentFolders.clear();

		// Iterate folders & fill data.
		for (auto& f : rootFolders)
		{
			//ContentFolder folder;
			//folder.path = EditorPathConstants::contentsPath;
			//folder.parent = NULL;
			//folder.name = f;
			//
			//// Add to contents list.
			//m_ContentFolders.push_back(folder);
		}

	}


}