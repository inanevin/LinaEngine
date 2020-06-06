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
#include "Utility/Log.hpp"
#include "Utility/EditorUtility.hpp"
#include "Core/EditorCommon.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderConstants.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <stdio.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static bool rightClickedContentBrowser = false;
static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static bool isECSPanelOpen;
static bool showIMGUIDemo;
static bool setDockspaceLayout = true;

namespace LinaEditor
{

	void GUILayer::OnUpdate()
	{
		
		//Setup
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
	
		// Draw main docking space.
		DrawCentralDockingSpace();

		// Draw tools
		// DrawTools();

		// Draw overlay fps counter
		DrawFPSCounter(&m_FPSCounterOpen, 1);
		//

		//
		//// Draw ECS Panel.
		m_ECSPanel->Draw();
		//
		//// Draw material panel.
		//m_MaterialPanel->Draw();
		//
		//// Draw resources panel
		m_ResourcesPanel->Draw();
		//
		//// Draw Scene Panel
		m_ScenePanel->Draw();

		if (showIMGUIDemo)
			ImGui::ShowDemoWindow(&showIMGUIDemo);
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
	}

	void GUILayer::OnEvent()
	{

	}

	void GUILayer::OnAttach()
	{
	
		LINA_CLIENT_INFO("TestLayer Attached");

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.Fonts->AddFontFromFileTTF("resources/fonts/defaultFont.ttf", 20.0f, NULL)->Scale = 0.92f;
		
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();


		GLFWwindow* window = static_cast<GLFWwindow*>(m_RenderEngine->GetNativeWindow());
		
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		ImGuiStyle& style = ImGui::GetStyle();
	
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Border] = ImVec4(0.39f, 0.38f, 0.39f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.40f, 0.23f, 0.26f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.28f, 0.28f, 0.28f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.54f, 0.12f, 0.23f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.54f, 0.12f, 0.23f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.54f, 0.12f, 0.23f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.54f, 0.12f, 0.23f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.54f, 0.12f, 0.23f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.12f, 0.23f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.26f, 0.06f, 0.11f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.42f, 0.11f, 0.19f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.39f, 0.10f, 0.18f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);


		// setup panels, windows etc.
		m_ECSPanel = new ECSPanel(Vector2::Zero, Vector2(700,600));
		m_MaterialPanel = new MaterialPanel(Vector2::Zero, Vector2(700, 600));
		m_ResourcesPanel = new ResourcesPanel(Vector2::Zero, Vector2(700, 400));
		m_ScenePanel = new ScenePanel(Vector2::Zero, Vector2(800, 600));

		m_ECSPanel->Setup(*m_ECS, *m_ScenePanel, m_RenderEngine->GetMainWindow(), *m_MaterialPanel);
		m_ECSPanel->Open();

		m_MaterialPanel->Setup(*m_RenderEngine);
		m_MaterialPanel->Open();

		m_ResourcesPanel->Setup();
		m_ResourcesPanel->Open();

		m_ScenePanel->Setup(*m_RenderEngine);
		m_ScenePanel->Open();

		setDockspaceLayout = true;
	}

	void GUILayer::OnDetach()
	{
		LINA_CLIENT_INFO("TestLayer Detached");

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		delete m_ECSPanel;
		delete m_MaterialPanel;
		delete m_ResourcesPanel;
		delete m_ScenePanel;
	}



	void GUILayer::DrawTools(bool* p_open, int corner)
	{
		
	}

	void GUILayer::DrawFPSCounter(bool* p_open, int corner)
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

		if (ImGui::Begin("FPSOverlay", p_open, window_flags))
		{
			ImGui::Text("FPS Overlay");
			ImGui::Separator();
			std::string fpsText = std::to_string(m_Application->GetCurrentFPS()) + " Frames per second";
			ImGui::Text(fpsText.c_str());
		}
		ImGui::End();
	}

	

	void GUILayer::DrawCentralDockingSpace()
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", NULL, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			if (setDockspaceLayout)
			{
				setDockspaceLayout = false;
				Vector2 screenSize = m_RenderEngine->GetWindowSize();
				ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
				ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(screenSize.x, screenSize.y));

				ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
				ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
				ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

				ImGui::DockBuilderDockWindow("Resources", dock_id_prop);
				ImGui::DockBuilderDockWindow("ECS", dock_id_prop);
				ImGui::DockBuilderDockWindow("Scene", dock_main_id);

				ImGui::DockBuilderFinish(dockspace_id);

			}
		
		}
		else
		{
			//ShowDockingDisabledMessage();
		}

		if (ImGui::BeginMenuBar())
		{
			/* if (ImGui::BeginMenu("Docking"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::Separator();
				//if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
					//*p_open = false;
				ImGui::EndMenu();
			} */

			if (ImGui::BeginMenu("Panels"))
			{
				if (ImGui::MenuItem("ECS Panel"))
					m_ECSPanel->Open();
				if (ImGui::MenuItem("Material Panel"))
					m_MaterialPanel->Open();
				if (ImGui::MenuItem("Scene Panel"))
					m_ScenePanel->Open();
				if (ImGui::MenuItem("Resources Panel"))
					m_ResourcesPanel->Open();
				if (ImGui::MenuItem("IMGUI Demo"))
					showIMGUIDemo = true;

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();
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
		ImGui::Begin("Content Browser");

		// Handle Right click popup.
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					Utility::EditorUtility::CreateFolderInPath(EditorPathConstants::contentsPath + "NewFolder");
					ReadProjectContentsFolder();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Material"))
				{

				}
				ImGui::EndMenu();
			}
			
			ImGui::EndPopup();
		}


		ImGuiTreeNodeFlags node_flags = base_flags;
		static int selection_mask = (1 << 2);
		int node_clicked = -1;

		for (int i = 0; i < m_ContentFolders.size(); i++)
		{
			const bool is_selected = (selection_mask & (1 << i)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			bool node_open = ImGui::TreeNodeEx(m_ContentFolders[i].name.c_str());

			if (ImGui::IsItemClicked())
				node_clicked = i;
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
				ImGui::Text("This is a drag and drop source");
				ImGui::EndDragDropSource();
			}
			if (node_open)
			{
				ImGui::BulletText("Blah blah\nBlah Blah");
				ImGui::TreePop();
			}
		}


		ImGui::End();

	}

	void GUILayer::ShowContentBrowserFileMenu()
	{
		ImGui::MenuItem("(dummy menu)", NULL, false, false);
		if (ImGui::MenuItem("New")) {}
		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
		if (ImGui::BeginMenu("Open Recent"))
		{
			ImGui::MenuItem("fish_hat.c");
			ImGui::MenuItem("fish_hat.inl");
			ImGui::MenuItem("fish_hat.h");
			if (ImGui::BeginMenu("More.."))
			{
				ImGui::MenuItem("Hello");
				ImGui::MenuItem("Sailor");
				if (ImGui::BeginMenu("Recurse.."))
				{
					//ShowContentBrowserFileMenu();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
		if (ImGui::MenuItem("Save As..")) {}

		ImGui::Separator();
		if (ImGui::BeginMenu("Options"))
		{
			static bool enabled = true;
			ImGui::MenuItem("Enabled", "", &enabled);
			ImGui::BeginChild("child", ImVec2(0, 60), true);
			for (int i = 0; i < 10; i++)
				ImGui::Text("Scrolling Text %d", i);
			ImGui::EndChild();
			static float f = 0.5f;
			static int n = 0;
			ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
			ImGui::InputFloat("Input", &f, 0.1f);
			ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Colors"))
		{
			float sz = ImGui::GetTextLineHeight();
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
				ImGui::Dummy(ImVec2(sz, sz));
				ImGui::SameLine();
				ImGui::MenuItem(name);
			}
			ImGui::EndMenu();
		}

		// Here we demonstrate appending again to the "Options" menu (which we already created above)
		// Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
		// In a real code-base using it would make senses to use this feature from very different code locations.
		if (ImGui::BeginMenu("Options")) // <-- Append!
		{
			static bool b = true;
			ImGui::Checkbox("SomeOption", &b);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Disabled", false)) // Disabled
		{
			IM_ASSERT(0);
		}
		if (ImGui::MenuItem("Checked", NULL, true)) {}
		if (ImGui::MenuItem("Quit", "Alt+F4")) {}
	}

	void GUILayer::ReadProjectContentsFolder()
	{
		// Get folders in directory.
		std::vector<std::string> rootFolders;
		Utility::EditorUtility::GetDirectories(rootFolders, EditorPathConstants::contentsPath);

		// Clear current folders.
		m_ContentFolders.clear();

		// Iterate folders & fill data.
		for (auto& f : rootFolders)
		{
			ContentFolder folder;
			folder.path = EditorPathConstants::contentsPath;
			folder.parent = NULL;
			folder.name = f;

			// Add to contents list.
			m_ContentFolders.push_back(folder);
		}

	}


}