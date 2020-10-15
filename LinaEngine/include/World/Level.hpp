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

	namespace ECS
	{
		class ECSRegistry;
	}
}


namespace LinaEngine::World
{
	struct LevelData
	{
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(dummy); // serialize things by passing them to the archive
		}

		int dummy;

	};

	class Level
	{
	public:

		Level() { };
		virtual ~Level() {  };

		virtual void OnLevelLoaded() {};
		virtual void OnLevelRemoved() {};
		virtual bool Install() = 0;
		virtual void Uninstall() {};
		virtual void Initialize() {};
		virtual void Tick(float delta) {};



		static void SerializeLevelData(const std::string& path, const std::string& levelName, Level& level, LinaEngine::ECS::ECSRegistry& registry);
		static void DeserializeLevelData(const std::string& path, const std::string& levelName, Level& level, LinaEngine::ECS::ECSRegistry& registry);

		LevelData m_levelData;
		LinaEngine::ECS::ECSRegistry* m_ecs;


	protected:

		LinaEngine::Graphics::RenderEngine* m_renderEngine = nullptr;
		LinaEngine::Input::InputEngine* m_inputEngine = nullptr;

	private:

		friend class LinaEngine::Application;

		void SetEngineReferences(LinaEngine::ECS::ECSRegistry* reg, LinaEngine::Graphics::RenderEngine& renderEngineIn, LinaEngine::Input::InputEngine& inputEngineIn)
		{
			m_ecs = reg;
			m_renderEngine = &renderEngineIn;
			m_inputEngine = &inputEngineIn;
		}
		
	};
}

#endif