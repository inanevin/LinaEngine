/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SB_Application
Timestamp: 12/29/2018 11:15:41 PM

*/

#include "Core/EditorApplication.hpp"
#include "PackageManager/PAMWindow.hpp"
#include "PackageManager/PAMInputDevice.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Input/InputEngine.hpp"
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

		// Setup layers
		m_guiLayer->Setup(*appWindow, *renderEngine, *physicsEngine, app, *ecs);

		// Remove splash.
		delete splash;

		// Push layer into the engine.
		renderEngine->PushLayer(m_guiLayer);

	}

	void EditorApplication::Refresh()
	{
		m_guiLayer->GetECSPanel()->Refresh();
	}


}
