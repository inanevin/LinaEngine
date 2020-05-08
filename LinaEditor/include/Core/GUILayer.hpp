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

namespace LinaEngine
{
	class Application;
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
		void OnEvent(LinaEngine::Event& e) override;

		// CLASS METHODS
		FORCEINLINE void Setup(LinaEngine::Graphics::RenderEngine& engine, LinaEngine::Application* application) { m_RenderEngine = &engine; m_Application = application; }

	private:

		void DrawEntitiesWindow();
		void DrawSkyboxSettingsWindow();

	private:

		LinaEngine::Graphics::RenderEngine* m_RenderEngine;
		LinaEngine::Application* m_Application;

	};
}