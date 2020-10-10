/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SplashScreen
Timestamp: 10/10/2020 3:26:27 PM

*/


#include "Core/SplashScreen.hpp"
#include "Rendering/Window.hpp"
#include "Utility/Log.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace LinaEditor
{
	void SplashScreen::Draw()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("sa");
		ImGui::Text("heeey");
		ImGui::End();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		m_window->Tick();

	}

	void SplashScreen::Setup(LinaEngine::Graphics::Window* splashWindow, const LinaEngine::Graphics::WindowProperties& props)
	{
		// Set ref.
		m_window = splashWindow;

		// Create context.
		bool windowCreationSuccess = splashWindow->CreateContext(props);
		if (!windowCreationSuccess)
		{
			LINA_CORE_ERR("Window Creation Failed!");
			return;
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		GLFWwindow* window = static_cast<GLFWwindow*>(splashWindow->GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

	}

	SplashScreen::~SplashScreen()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		delete m_window;
	}
}