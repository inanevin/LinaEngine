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

namespace LinaEditor
{
	void CreateSplashScreen()
	{

		LinaEngine::Graphics::WindowProperties splashProps;
		splashProps.m_width = 720;
		splashProps.m_height = 450;
		splashProps.m_decorated = false;
		splashProps.m_resizable = false;

		// Setup splash screen.
		SplashScreen* splashScreen = new SplashScreen();
		//splashScreen->Setup(new ContextWindow(), splashProps);
		//splashScreen->Draw();		
		delete splashScreen;
	}

	class EditorApplication : public LinaEngine::Application
	{
	public:


		EditorApplication() {

			LINA_CLIENT_TRACE("[Constructor] -> Editor Application ({0})", typeid(*this).name());


			// Init engine.
			LinaEngine::Graphics::WindowProperties props;
			props.m_width = 1440;
			props.m_height = 900;
			props.m_decorated = false;
			props.m_resizable = false;
			props.m_title = "Lina Engine - Configuration [] - Build Type [] - Project [] - Build []";
			Initialize(props);
	
			LinaEngine::Graphics::WindowProperties splashProps;
			splashProps.m_width = 720;
			splashProps.m_height = 450;
			splashProps.m_decorated = false;
			splashProps.m_resizable = false;

			SplashScreen* splash = new SplashScreen();
			splash->Setup(&GetAppWindow(), &GetRenderEngine(), splashProps);
			splash->Draw(); // We should carry this over to a separate thread later on when things are more complex and requires data shown to the user while loading.

			// Create layers
			m_guiLayer = new LinaEditor::GUILayer();

			// Setup layers
			m_guiLayer->Setup(GetAppWindow(), GetRenderEngine(), GetPhysicsEngine(), this, GetECSREgistry(), m_startupLevel, m_scenePanelSize);

			// Install level.
			InstallLevel(&m_startupLevel);	
			InitializeLevel(&m_startupLevel);

			// Remove splash.
			delete splash;

			// Push layer into the engine.
			GetRenderEngine().PushLayer(m_guiLayer);

			// Set the app window size back to original.
			GetAppWindow().SetSize(Vector2(props.m_width, props.m_height));
			GetAppWindow().SetPosCentered(Vector2::Zero);


			// Run engine.
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