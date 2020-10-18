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
Class: Level

Level class defines functions required for installing, loading and processing a level.
Derived classes contains custom logic for creating entity systems and processing the gameplay
logic.

Timestamp: 5/6/2019 5:10:23 PM
*/

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include <string>

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
			archive(dummy); 
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

		void SerializeLevelData(const std::string& path, const std::string& levelName);
		void DeserializeLevelData(const std::string& path, const std::string& levelName);

		LevelData m_levelData;

	};
}

#endif