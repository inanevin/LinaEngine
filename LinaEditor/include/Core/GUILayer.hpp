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

#include "Core/Layer.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/ECSSystem.hpp"
#include <vector>

namespace LinaEngine
{
	class Application;

	namespace World
	{
		class Level;
	}

	namespace Physics
	{
		class PhysicsEngine;
	}

	namespace Graphics
	{
		class Window;
	}
}


namespace LinaEditor
{


	class GUILayer : public LinaEngine::Layer
	{

	public:

		GUILayer() : LinaEngine::Layer("UILayer")
		{

		}

		// OVERRIDES
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent() override;

		// CLASS METHODS
		FORCEINLINE void Setup(LinaEngine::Graphics::Window& window, LinaEngine::Graphics::RenderEngine& renderEngine, LinaEngine::Physics::PhysicsEngine& physicsEngine, LinaEngine::Application* application, LinaEngine::ECS::ECSRegistry& ecs, LinaEngine::World::Level& level, const Vector2& scenePanelSize)
		{ 
			m_appWindow = &window;
			m_RenderEngine = &renderEngine; 
			m_physicsEngine = &physicsEngine;
			m_Application = application; 
			m_ECS = &ecs;	
			m_CurrentLevel = &level; 
			m_scenePanelSize = scenePanelSize;
		}

		// Getters for references.
		FORCEINLINE LinaEngine::ECS::ECSRegistry* GetECS() const { return m_ECS; }
		FORCEINLINE class ECSPanel* GetECSPanel() const { return m_ECSPanel; }
		FORCEINLINE class PropertiesPanel* GetPropertiesPanel() const { return m_PropertiesPanel; }
		FORCEINLINE class MaterialPanel* GetMaterialPanel() const { return m_MaterialPanel; }
		FORCEINLINE class ResourcesPanel* GetResourcesPanel() const { return m_ResourcesPanel; }
		FORCEINLINE class ScenePanel* GetScenePanel() const { return m_ScenePanel; }
		FORCEINLINE LinaEngine::Graphics::RenderEngine* GetRenderEngine() const { return m_RenderEngine; }	

	private:


		// Draws a tools overlay.
		void DrawTools(bool* p_open, int corner = 0);

		// Draws an fps counter overlay.
		void DrawFPSCounter(bool* p_open, int corner = 0);

		// Draws a central docking space.
		void DrawCentralDockingSpace();

		// Draw content browser.
		void DrawContentBrowserWindow();

		// Draw skybox settings.
		void DrawSkyboxSettingsWindow();

		// Show the file menu for content browser.
		void ShowContentBrowserFileMenu();

		// Checks the project content folder for new files.
		void ReadProjectContentsFolder();

	private:


	private:

		LinaEngine::Graphics::Window* m_appWindow = nullptr;
		LinaEngine::Graphics::RenderEngine* m_RenderEngine = nullptr;
		LinaEngine::Physics::PhysicsEngine* m_physicsEngine = nullptr;
		LinaEngine::ECS::ECSRegistry* m_ECS = nullptr;
		LinaEngine::Application* m_Application = nullptr;
		LinaEngine::World::Level* m_CurrentLevel = nullptr;
		bool m_FPSCounterOpen = true;
		class ECSPanel* m_ECSPanel;
		class MaterialPanel* m_MaterialPanel;
		class ResourcesPanel* m_ResourcesPanel;
		class ScenePanel* m_ScenePanel;
		class PropertiesPanel* m_PropertiesPanel;
		class LogPanel* m_LogPanel;
		Vector2 m_scenePanelSize = Vector2::Zero;
	};
}