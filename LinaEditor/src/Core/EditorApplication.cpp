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

#include <Lina.hpp>
#include "Core/GUILayer.hpp"
#include "Levels/Example1Level.hpp"
#include "PackageManager/PAMWindow.hpp"
#include "PackageManager/PAMInputDevice.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Input/InputEngine.hpp"
#include "Core/SplashScreen.hpp"
#include <thread>

namespace LinaEditor
{
	void CreateSplashScreen(bool* run)
	{

		LinaEngine::Graphics::WindowProperties splashProps;
		splashProps.m_width = 720;
		splashProps.m_height = 450;
		splashProps.m_decorated = false;
		splashProps.m_resizable = false;

		SplashScreen* splashScreen = new SplashScreen();

		// Setup splash screen.
		splashScreen->Setup(new ContextWindow(), splashProps);

		while (*run)
		{
			splashScreen->Draw();
		}

		delete splashScreen;
	}

	class EditorApplication : public LinaEngine::Application
	{
	public:


		EditorApplication() {

			LINA_CLIENT_TRACE("[Constructor] -> Editor Application ({0})", typeid(*this).name());

		//bool showSplashScreen = true;
		//
		//// Load startup level.
		//!InstallLevel(&m_startupLevel);
		//
		//std::thread t1(LinaEditor::CreateSplashScreen, &showSplashScreen);
		//
		//t1.join();
		//
		//return;

			LinaEngine::Graphics::WindowProperties props;
			props.m_width = 1440;
			props.m_height = 900;
			props.m_decorated = false;
			props.m_resizable = false;
			props.m_title = "Lina Engine - Configuration [] - Build Type [] - Project [] - Build []";

			Initialize(props);

		
			// Create layers
			m_guiLayer = new LinaEditor::GUILayer();

			// Setup layers
			m_guiLayer->Setup(GetAppWindow(), GetRenderEngine(), GetPhysicsEngine(), this, GetECSREgistry(), m_startupLevel, m_scenePanelSize);

			InstallLevel(&m_startupLevel);
			InitializeLevel(&m_startupLevel);

			// Push layer into the engine.
			GetRenderEngine().PushLayer(m_guiLayer);


			Run();


		}


		~EditorApplication()
		{
			LINA_CLIENT_TRACE("[Destructor] -> Editor Application ({0})", typeid(*this).name());
		}


	private:

		Vector2 m_scenePanelSize = Vector2(800, 600);
		GUILayer* m_guiLayer;
		Example1Level m_startupLevel;
	};

}


LinaEngine::Application* LinaEngine::CreateApplication()
{
	return new LinaEditor::EditorApplication();
}

// Default platform context window.
LinaEngine::Graphics::Window* LinaEngine::CreateContextWindow()
{
	return new ContextWindow();
}

// Default platform input device.
LinaEngine::Input::InputDevice* LinaEngine::CreateInputDevice()
{
	return new InputDevice();
}

// Default engine
LinaEngine::Graphics::RenderEngine* LinaEngine::CreateRenderEngine()
{
	return new LinaEngine::Graphics::RenderEngine();
}

// Default engine
LinaEngine::Physics::PhysicsEngine* LinaEngine::CreatePhysicsEngine()
{
	return new LinaEngine::Physics::PhysicsEngine();
}

// Default engine
LinaEngine::Input::InputEngine* LinaEngine::CreateInputEngine()
{
	return new LinaEngine::Input::InputEngine();
}