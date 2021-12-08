
/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Example1Level
Timestamp: 5/6/2019 9:22:56 PM

*/

#include "Levels/Example1Level.hpp"
#include "Core/Application.hpp"
#include "ECS/Components/AnimationComponent.hpp"

using namespace LinaEngine::Graphics;
using namespace LinaEngine::ECS;
using namespace LinaEngine;

Example1Level::~Example1Level()
{


}

bool Example1Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
{
	Level::Install(loadFromFile, path, levelName);
	return true;
}

void Example1Level::Initialize()
{
	LINA_CLIENT_TRACE("Example1Level initialized. Implement your logic for instantiating entities, players, assigning cameras etc. from now on.");

	auto ybot = Application::GetECSRegistry().GetEntity("YBot");

	//auto& anim = Application::GetECSRegistry().emplace<ECS::AnimationComponent>(ybot);
	//anim.m_animationName = "Armature_Move";
}

void Example1Level::Tick(bool isInPlayMode, float delta)
{
	return;
}

