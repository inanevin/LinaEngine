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

#include "Core/SplashScreen.hpp"
#include "Core/WindowBackend.hpp"
#include "Log/Log.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Rendering/Texture.hpp"
#include "Core/RenderDeviceBackend.hpp"
#include "Core/Application.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Lina::Graphics::Texture* splashScreenTexture;

namespace Lina::Editor
{
	void SplashScreen::Initialize(const Lina::WindowProperties& props)
	{
		// Set GUI draw params.
		Lina::Graphics::DrawParams splashDrawParams;
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
		Lina::Graphics::RenderEngineBackend::Get()->SetDrawParameters(splashDrawParams);

		// Setup Dear ImGui context
		//IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Add default font.
		io.Fonts->AddFontFromFileTTF("resources/editor/fonts/Mukta-Medium.ttf", 22.0f, NULL);

		Lina::Graphics::WindowBackend* splashWindow = Lina::Graphics::WindowBackend::Get();

		GLFWwindow* window = static_cast<GLFWwindow*>(splashWindow->GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		splashWindow->SetSize(Vector2(props.m_width, props.m_height));
		splashWindow->SetPos(Lina::Vector2(mode->width / 2.0f + props.m_xPos - props.m_width / 2.0f, mode->height / 2.0f + props.m_yPos - props.m_height / 2.0f));

		// Build pixel data.
		splashScreenTexture = &Lina::Graphics::Texture::CreateTexture2D("resources/editor/textures/splashScreen.png", Graphics::SamplerParameters(), false, false, "");
		Event::EventSystem::Get()->Connect<Event::EResourceLoadUpdated, &SplashScreen::OnResourceLoadUpdated>(this);
		Draw();
	}

	void SplashScreen::OnResourceLoadUpdated(Event::EResourceLoadUpdated ev)
	{
		m_currentlyLoadingResource = ev.m_currentResource;
		m_percentage = ev.m_percentage;
		Draw();
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("SplashScreen", NULL, ImGuiWindowFlags_NoDecoration);
		ImGui::GetWindowDrawList()->AddImage((void*)splashScreenTexture->GetID(), ImVec2(0, 0), viewport->Size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SetNextWindowPos(ImVec2(40, 310));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::BeginChild("text", ImVec2(640, 90), ImGuiWindowFlags_NoDecoration);
		ImGui::Text("Loading %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
		ImGui::Text(m_currentlyLoadingResource.c_str());
		std::string loadData = std::to_string(m_percentage) + "%";
		ImGui::Text(loadData.c_str());
		WidgetsUtility::IncrementCursorPosY(10);
		WidgetsUtility::DrawShadowedLine(1, ImVec4(1, 1, 1, 1), 2);
		ImVec2 max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() * m_percentage / 100.0f, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());
		WidgetsUtility::DrawShadowedLine(1, ImVec4(1, 0, 1, 1), 2, ImVec2(0,0), max);
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::End();
		ImGui::PopStyleVar();
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// swap buffers.
		Lina::Graphics::WindowBackend::Get()->Tick();
	}

	SplashScreen::~SplashScreen()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		if (splashScreenTexture)
			Lina::Graphics::Texture::UnloadTextureResource(splashScreenTexture->GetSID());
	}
}