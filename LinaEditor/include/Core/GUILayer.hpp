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
#include "Panels/EditorPanel.hpp"
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

	enum class MenuBarItems
	{
		NewProject = 0,
		LoadProject = 1,
		SaveProject = 2,

		Edit = 10,
		View = 20,

		SaveLevelData = 30,
		LoadLevelData = 31,
		NewLevelData = 32,

		ECSPanel = 40,
		HeaderPanel = 11,
		LogPanel = 42,
		MaterialPanel = 43,
		PropertiesPanel = 44,
		ResourcesPanel = 45,
		ScenePanel = 46,
		ImGuiPanel = 47,

		DebugViewPhysics = 61,
		DebugViewShadows = 62,
		DebugViewNormal = 63,

	};

	class GUILayer : public LinaEngine::Layer
	{

	public:

		GUILayer() : LinaEngine::Layer("UILayer")
		{

		}

		~GUILayer();

		// Draws the launcher splash screen.
		void DrawSplash();

		// OVERRIDES
		void OnAttach() override;
		void OnDetach() override;
		void OnTick(float dt) override;

		// Menu bar item callbacks.
		void MenuBarItemClicked(const MenuBarItems& item);

		// CLASS METHODS
		FORCEINLINE void Setup(LinaEngine::Graphics::Window& window, LinaEngine::Graphics::RenderEngine& renderEngine, LinaEngine::Physics::PhysicsEngine& physicsEngine, LinaEngine::Application* application, LinaEngine::ECS::ECSRegistry& ecs)
		{ 
			m_appWindow = &window;
			m_renderEngine = &renderEngine; 
			m_physicsEngine = &physicsEngine;
			m_application = application; 
			m_ecs = &ecs;	
		}

		// Getters for references.
		FORCEINLINE LinaEngine::ECS::ECSRegistry* GetECS() const { return m_ecs; }
		FORCEINLINE class ECSPanel* GetECSPanel() const { return m_ecsPanel; }
		FORCEINLINE class PropertiesPanel* GetPropertiesPanel() const { return m_propertiesPanel; }
		FORCEINLINE class MaterialPanel* GetMaterialPanel() const { return m_materialPanel; }
		FORCEINLINE class ResourcesPanel* GetResourcesPanel() const { return m_resourcesPanel; }
		FORCEINLINE class ScenePanel* GetScenePanel() const { return m_scenePanel; }
		FORCEINLINE LinaEngine::Graphics::RenderEngine* GetRenderEngine() const { return m_renderEngine; }
		FORCEINLINE class LinaEngine::Graphics::Window* GetAppWindow() const { return m_appWindow; }
		FORCEINLINE class LinaEngine::Application* GetApp() const { return m_application; }
		FORCEINLINE void SetCurrentLevel(LinaEngine::World::Level& currentLevel) { m_currentLevel = &currentLevel; }

	private:

		// Draws level data dialogs.
		void DrawLevelDataDialogs();

		// Draws an fps counter overlay.
		void DrawFPSCounter(int corner = 0);

		// Draws a central docking space.
		void DrawCentralDockingSpace();

		// Draw content browser.
		void DrawContentBrowserWindow();

		// Draw skybox settings.
		void DrawSkyboxSettingsWindow();

		// Checks the project content folder for new files.
		void ReadProjectContentsFolder();

	private:


	private:

		LinaEngine::Graphics::DrawParams m_drawParameters;
		LinaEngine::Graphics::Window* m_appWindow = nullptr;
		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Physics::PhysicsEngine* m_physicsEngine = nullptr;
		LinaEngine::ECS::ECSRegistry* m_ecs = nullptr;
		LinaEngine::Application* m_application = nullptr;
		LinaEngine::World::Level* m_currentLevel = nullptr;
		class ECSPanel* m_ecsPanel;
		class MaterialPanel* m_materialPanel;
		class ResourcesPanel* m_resourcesPanel;
		class ScenePanel* m_scenePanel;
		class PropertiesPanel* m_propertiesPanel;
		class LogPanel* m_logPanel;
		class HeaderPanel* m_headerPanel;
		std::vector<EditorPanel*> m_panels;

	};
}