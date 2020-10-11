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
#include "Rendering/RenderEngine.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "Rendering/Texture.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

LinaEngine::Graphics::Texture* splashScreenTexture;

namespace LinaEditor
{


	void SplashScreen::Setup(LinaEngine::Graphics::Window* splashWindow, LinaEngine::Graphics::RenderEngine* renderEngine, const LinaEngine::Graphics::WindowProperties& props)
	{
		// Set ref.
		m_window = splashWindow;

		// Set GUI draw params.
		LinaEngine::Graphics::DrawParams splashDrawParams;
		splashDrawParams.useScissorTest = false;
		splashDrawParams.useDepthTest = true;
		splashDrawParams.useStencilTest = true;
		splashDrawParams.primitiveType = Graphics::PrimitiveType::PRIMITIVE_TRIANGLES;
		splashDrawParams.faceCulling = Graphics::FaceCulling::FACE_CULL_BACK;
		splashDrawParams.sourceBlend = Graphics::BlendFunc::BLEND_FUNC_SRC_ALPHA;
		splashDrawParams.destBlend = Graphics::BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		splashDrawParams.shouldWriteDepth = true;
		splashDrawParams.depthFunc = Graphics::DrawFunc::DRAW_FUNC_LEQUAL;
		splashDrawParams.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
		splashDrawParams.stencilComparisonVal = 0;
		splashDrawParams.stencilTestMask = 0xFF;
		splashDrawParams.stencilWriteMask = 0xFF;
		splashDrawParams.stencilFail = Graphics::StencilOp::STENCIL_KEEP;
		splashDrawParams.stencilPass = Graphics::StencilOp::STENCIL_REPLACE;
		splashDrawParams.stencilPassButDepthFail = Graphics::StencilOp::STENCIL_KEEP;
		splashDrawParams.scissorStartX = 0;
		splashDrawParams.scissorStartY = 0;
		splashDrawParams.scissorWidth = 0;
		splashDrawParams.scissorHeight = 0;

		// Set draw params.
		renderEngine->SetDrawParameters(splashDrawParams);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		GLFWwindow* window = static_cast<GLFWwindow*>(splashWindow->GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		splashWindow->SetSize(Vector2(props.m_width, props.m_height));
		splashWindow->SetPos(LinaEngine::Vector2(mode->width / 2.0f + props.m_xPos - props.m_width / 2.0f, mode->height / 2.0f + props.m_yPos - props.m_height / 2.0f));

		// Create pixel data.
		splashScreenTexture = &renderEngine->CreateTexture2D("resources/textures/splashScreen.png");
	}



	void SplashScreen::Draw()
	{
		// Nf
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Setup wndow.
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(viewport->Size);

		// Draw window.
		ImGui::Begin("SplashScreen", NULL, ImGuiWindowFlags_NoDecoration);

		ImGui::GetWindowDrawList()->AddImage((void*)splashScreenTexture->GetID(), ImVec2(0,0), viewport->Size, ImVec2(0,1), ImVec2(1,0));
		ImGui::Text("Loading %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);

		ImGui::End();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// swap buffers.
		m_window->Tick();

	}

	SplashScreen::~SplashScreen()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		delete splashScreenTexture;
	}
}