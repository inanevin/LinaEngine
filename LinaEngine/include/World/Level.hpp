/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Level
Timestamp: 5/6/2019 5:10:23 PM

*/

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include "Core/Common.hpp"
#include "ECS/ECS.hpp"

namespace LinaEngine
{
	class Application;

	namespace Graphics
	{
		class RenderEngine;
	}

	namespace Input
	{
		class InputEngine;
	}
}


namespace LinaEngine::World
{
	

	class Level
	{
	public:

		Level() {};
		virtual ~Level() {};

		virtual void OnLevelLoaded() {};
		virtual void OnLevelRemoved() {};
		virtual void Install() {};
		virtual void Initialize() {};
		virtual void Tick(float delta) {};


	protected:

		LinaEngine::ECS::ECSRegistry* m_ECS = nullptr;
		LinaEngine::Graphics::RenderEngine* m_RenderEngine = nullptr;
		LinaEngine::Input::InputEngine* m_InputEngine = nullptr;

		std::vector<ECSEntity> m_LevelEntities;

	private:

		friend class LinaEngine::Application;

		void SetEngineReferences(LinaEngine::ECS::ECSRegistry& ecsIn, LinaEngine::Graphics::RenderEngine& renderEngineIn, LinaEngine::Input::InputEngine& inputEngineIn)
		{
			m_ECS = &ecsIn;
			m_RenderEngine = &renderEngineIn;
			m_InputEngine = &inputEngineIn;
		}
		
	};
}

#endif