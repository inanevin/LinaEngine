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

namespace LinaEditor
{
	class EditorApplication : public LinaEngine::Application
	{
	public:

		EditorApplication() {
			LINA_CLIENT_TRACE("[Constructor] -> Editor Application ({0})", typeid(*this).name());

			// Create layers
			m_guiLayer = new LinaEditor::GUILayer();

			// Setup layers
			m_guiLayer->Setup(GetAppWindow(), GetRenderEngine(), GetPhysicsEngine(), this, GetECSREgistry(), m_StartupLevel, m_scenePanelSize);

			// Load startup level.
			LoadLevel(&m_StartupLevel);

			// Push layer into the engine. ** WHILE LOOP INSIDE ** 
			//GetRenderEngine().PushLayer(m_guiLayer);

			// Set display size to match scene panel.
			//PushOverlay(new LinaEngine::Layer_IMGUI());

		}
		~EditorApplication() {
			LINA_CLIENT_TRACE("[Destructor] -> Editor Application ({0})", typeid(*this).name());
		
		}


	private:

	
		Vector2 m_scenePanelSize = Vector2(800, 600);
		GUILayer* m_guiLayer;
		Example1Level m_StartupLevel;

	};

}


LinaEngine::Application* LinaEngine::CreateApplication()
{
	return new LinaEditor::EditorApplication();
}

LinaEngine::Graphics::Window* LinaEngine::CreateContextWindow()
{
	return new ContextWindow();
}