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
#include "Core/Application.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Panels/ECSPanel.hpp"
#include "Core/SplashScreen.hpp"
#include "Core/GUILayer.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "Core/EditorCommon.hpp"
using namespace Lina::ECS;

namespace Lina::Editor
{

	Lina::Action::ActionDispatcher EditorApplication::s_editorDispatcher;

	EditorApplication::EditorApplication()
	{
		s_editorDispatcher.Initialize(Lina::Action::ActionType::EditorActionsStartIndex, Lina::Action::ActionType::EditorActionsEndIndex);

	}

	EditorApplication::~EditorApplication()
	{
		LINA_CLIENT_TRACE("[Destructor] -> Editor Application ({0})", typeid(*this).name());
	}

	void EditorApplication::Setup()
	{
		LINA_CLIENT_TRACE("[Constructor] -> Editor Application ({0})", typeid(*this).name());

		Lina::Graphics::WindowProperties splashProps;
		splashProps.m_width = 720;
		splashProps.m_height = 450;
		splashProps.m_decorated = false;
		splashProps.m_resizable = false;

		SplashScreen* splash = new SplashScreen();
		splash->Setup(splashProps);
		splash->Draw(); // We should carry this over to a separate thread later on when things are more complex and requires data shown to the user while loading.

		// Remove splash.
		delete splash;

		Lina::Application::GetRenderEngine().PushLayer(m_guiLayer);

		Lina::Application::GetEngineDispatcher().SubscribeAction<Lina::World::Level*>("##linaeditor_level_init", Lina::Action::ActionType::LevelInitialized,
			std::bind(&EditorApplication::LevelInstalled, this, std::placeholders::_1));

		Lina::Application::GetEngineDispatcher().SubscribeAction <bool>("##linaeditor_playmode", Lina::Action::ActionType::PlayModeActivation, std::bind(&EditorApplication::PlayModeChanged, this, std::placeholders::_1));

		editorCameraSystem.Construct(Lina::Application::GetECSRegistry(), Lina::Application::GetInputEngine(), m_guiLayer.GetScenePanel());
		editorCameraSystem.SystemActivation(true);

		Lina::Application::GetApp().AddToMainPipeline(editorCameraSystem);


	}

	void EditorApplication::Refresh()
	{
		m_guiLayer.Refresh();
	}

	void EditorApplication::LevelInstalled(Lina::World::Level* level)
	{
		ECSRegistry& ecs = Lina::Application::GetECSRegistry();

		auto singleView = ecs.view<Lina::ECS::EntityDataComponent>();

		if (ecs.GetEntity(EDITOR_CAMERA_NAME) == entt::null)
		{
			ECSEntity editorCamera = ecs.CreateEntity(EDITOR_CAMERA_NAME);
			EntityDataComponent cameraTransform;
			CameraComponent cameraComponent;
			FreeLookComponent freeLookComponent;
			ecs.emplace<CameraComponent>(editorCamera, cameraComponent);
			ecs.emplace<FreeLookComponent>(editorCamera, freeLookComponent);
			Lina::Application::GetRenderEngine().GetCameraSystem()->SetActiveCamera(editorCamera);
			editorCameraSystem.SetEditorCamera(editorCamera);
			Refresh();
		}
		else
		{
			ECSEntity editorCamera = ecs.GetEntity(EDITOR_CAMERA_NAME);
			Lina::Application::GetRenderEngine().GetCameraSystem()->SetActiveCamera(editorCamera);
			ecs.get<FreeLookComponent>(editorCamera).m_isEnabled = true;
			editorCameraSystem.SetEditorCamera(editorCamera);
		}

	}

	void EditorApplication::PlayModeChanged(bool enabled)
	{
		ECSRegistry& ecs = Lina::Application::GetECSRegistry();
		ECSEntity editorCamera = ecs.GetEntity(EDITOR_CAMERA_NAME);

		if (editorCamera != entt::null)
		{
			if (enabled)
			{
				if (Lina::Application::GetRenderEngine().GetCameraSystem()->GetActiveCamera() == editorCamera)
					Lina::Application::GetRenderEngine().GetCameraSystem()->SetActiveCamera(entt::null);

				ecs.get<FreeLookComponent>(editorCamera).m_isEnabled = false;
				editorCameraSystem.SystemActivation(false);
			}
			else
			{
				Lina::Application::GetRenderEngine().GetCameraSystem()->SetActiveCamera(editorCamera);
				ecs.get<FreeLookComponent>(editorCamera).m_isEnabled = true;
				editorCameraSystem.SystemActivation(true);
			}
			
		}
	}


}
