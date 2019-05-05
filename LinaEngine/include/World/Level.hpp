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
Timestamp: 5/4/2019 3:33:24 PM

*/

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include "Core/APIExport.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "Rendering/RenderEngine.hpp"
#include "PackageManager/PAMInputEngine.hpp"

namespace LinaEngine
{
	class Application;
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

		EntityComponentSystem* m_ECS;
		RenderEngine* m_RenderEngine;
		InputEngine<PAMInputEngine>* m_InputEngine;


	private:

		friend class LinaEngine::Application;

		void SetEngineReferences(EntityComponentSystem& ecsIn, RenderEngine& renderEngineIn, InputEngine<PAMInputEngine>& inputEngineIn)
		{
			m_ECS = &ecsIn;
			m_RenderEngine = &renderEngineIn;
			m_InputEngine = &inputEngineIn;
		}

	};
}


#endif