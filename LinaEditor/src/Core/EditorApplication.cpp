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

#include "Core/EditorApplication.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Panels/ECSPanel.hpp"
#include "Core/SplashScreen.hpp"
#include "Core/GUILayer.hpp"

namespace LinaEditor
{

	void EditorApplication::Initialize(LinaEngine::Graphics::Window* appWindow, LinaEngine::Graphics::RenderEngine* renderEngine, LinaEngine::Application* app, LinaEngine::Physics::PhysicsEngine* physicsEngine, LinaEngine::ECS::ECSRegistry* ecs)
	{
		LINA_CLIENT_TRACE("[Initialization] -> Editor Application ({0})", typeid(*this).name());

		LinaEngine::Graphics::WindowProperties splashProps;
		splashProps.m_width = 720;
		splashProps.m_height = 450;
		splashProps.m_decorated = false;
		splashProps.m_resizable = false;

		SplashScreen* splash = new SplashScreen();
		splash->Setup(appWindow, renderEngine, splashProps);
		splash->Draw(); // We should carry this over to a separate thread later on when things are more complex and requires data shown to the user while loading.

		// Create layers
		m_guiLayer = new GUILayer();
		m_guiLayer->Setup(*appWindow, *renderEngine, *physicsEngine, app, *ecs);

		// Remove splash.
		delete splash;

		renderEngine->PushLayer(m_guiLayer);

	}

	void EditorApplication::Refresh()
	{
		m_guiLayer->GetECSPanel()->Refresh();
	}


}
