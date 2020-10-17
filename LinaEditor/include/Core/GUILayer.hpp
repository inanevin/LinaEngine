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

/*
Class: UILayer

This class is pushed as an overlay layer to the render engine and is responsible for drawing the editor.
It inits panels, drawers etc. and is the main bridge of communication between editor components.

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

		void DrawSplash();

		// Overrides
		void OnAttach() override;
		void OnDetach() override;
		void OnTick(float dt) override;

		// Menu bar item callback from header panel.
		void MenuBarItemClicked(const MenuBarItems& item);

		void Setup(LinaEngine::Graphics::Window& window, LinaEngine::Graphics::RenderEngine& renderEngine, LinaEngine::Physics::PhysicsEngine& physicsEngine, LinaEngine::Application* application, LinaEngine::ECS::ECSRegistry& ecs)
		{ 
			m_appWindow = &window;
			m_renderEngine = &renderEngine; 
			m_physicsEngine = &physicsEngine;
			m_application = application; 
			m_ecs = &ecs;	
		}

		LinaEngine::ECS::ECSRegistry* GetECS() const { return m_ecs; }
		class ECSPanel* GetECSPanel() const { return m_ecsPanel; }
		class PropertiesPanel* GetPropertiesPanel() const { return m_propertiesPanel; }
		class MaterialPanel* GetMaterialPanel() const { return m_materialPanel; }
		class ResourcesPanel* GetResourcesPanel() const { return m_resourcesPanel; }
		class ScenePanel* GetScenePanel() const { return m_scenePanel; }
		LinaEngine::Graphics::RenderEngine* GetRenderEngine() const { return m_renderEngine; }
		class LinaEngine::Graphics::Window* GetAppWindow() const { return m_appWindow; }
		class LinaEngine::Application* GetApp() const { return m_application; }
		void SetCurrentLevel(LinaEngine::World::Level& currentLevel) { m_currentLevel = &currentLevel; }

	private:

		void DrawLevelDataDialogs();
		void DrawFPSCounter(int corner = 0);
		void DrawCentralDockingSpace();

	private:

		LinaEngine::Graphics::DrawParams m_drawParameters;
		LinaEngine::Graphics::Window* m_appWindow = nullptr;
		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Physics::PhysicsEngine* m_physicsEngine = nullptr;
		LinaEngine::ECS::ECSRegistry* m_ecs = nullptr;
		LinaEngine::Application* m_application = nullptr;
		LinaEngine::World::Level* m_currentLevel = nullptr;
		class ECSPanel* m_ecsPanel = nullptr;
		class MaterialPanel* m_materialPanel = nullptr;
		class ResourcesPanel* m_resourcesPanel = nullptr;
		class ScenePanel* m_scenePanel = nullptr;
		class PropertiesPanel* m_propertiesPanel = nullptr;
		class LogPanel* m_logPanel = nullptr;
		class HeaderPanel* m_headerPanel = nullptr;
		std::vector<EditorPanel*> m_panels;

	};
}