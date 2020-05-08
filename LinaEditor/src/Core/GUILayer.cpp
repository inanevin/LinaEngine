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
#include "Utility/Log.hpp"
#include "Rendering/Material.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace LinaEditor
{
	void GUILayer::OnUpdate()
	{

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Draw hierarchy window.
		DrawEntitiesWindow();

		// Draw skybox settings.
		DrawSkyboxSettingsWindow();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	}

	void GUILayer::OnEvent(LinaEngine::Event& e)
	{

	}

	void GUILayer::OnAttach()
	{
		LINA_CLIENT_INFO("TestLayer Attached");

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		GLFWwindow* window = static_cast<GLFWwindow*>(m_RenderEngine->GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

	}

	void GUILayer::OnDetach()
	{
		LINA_CLIENT_INFO("TestLayer Detached");

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	void GUILayer::DrawEntitiesWindow()
	{
		ImGui::Begin("Scene Entities");
		
		// Draw a list of entities.

		ImGui::End();
	}

	void GUILayer::DrawSkyboxSettingsWindow()
	{
		ImGui::ShowDemoWindow();

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
			Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

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
			

			bool isEditing = ImGui::ColorEdit4("Color##3", (float*)& color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		
			if (isEditing)
			{
				skyboxMaterial.SetColor(Graphics::MaterialConstants::colorProperty, Color(color.x, color.y, color.z, color.w));
			}
		}
		else if (currentItemID == 1)
		{
			// Gradient color skybox
			Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Update skybox shader if changed.
			if (previousCurrentItemID != 1)
			{
				previousCurrentItemID = 1;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxGradientShader);
			}

			// Show color pickers.
			ImGui::Text("Start Color");
			ImGui::SameLine();
			static ImVec4 colorStart = ImVec4(255, 255, 255, 255);
			bool isEditingStartColor = ImGui::ColorEdit4("Color##3", (float*)& colorStart, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			ImGui::Text("End Color");
			ImGui::SameLine();
			static ImVec4 colorEnd = ImVec4(255, 255, 255, 255);
			bool isEditingEndColor = ImGui::ColorEdit4("Color##3", (float*)& colorEnd, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		}
		else if (currentItemID == 2)
		{
			// Procedural Skybox
			Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Update skybox shader if changed.
			if (previousCurrentItemID != 2)
			{
				previousCurrentItemID = 2;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxProceduralShader);
			}

			// Show color pickers.
			ImGui::Text("Start Color");
			ImGui::SameLine();
			static ImVec4 colorStartP = ImVec4(255, 255, 255, 255);
			bool isEditingStartColor = ImGui::ColorEdit4("Color##3", (float*)& colorStartP, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			ImGui::Text("End Color");
			ImGui::SameLine();
			static ImVec4 colorEndP = ImVec4(255, 255, 255, 255);
			bool isEditingEndColor = ImGui::ColorEdit4("Color##3", (float*)& colorEndP, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		}
		else if (currentItemID == 3)
		{
			// Cubemap Skybox
			Graphics::Material& skyboxMaterial = m_RenderEngine->GetMaterial(Graphics::MaterialConstants::skyboxMaterialName);

			// Update skybox shader if changed.
			if (previousCurrentItemID != 3)
			{
				previousCurrentItemID = 3;
				m_RenderEngine->SetMaterialShader(skyboxMaterial, Graphics::ShaderConstants::skyboxCubemapShader);
			}
		}

		ImGui::End();
	}


}