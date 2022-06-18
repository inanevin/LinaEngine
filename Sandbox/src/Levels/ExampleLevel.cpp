
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

#include "Levels/ExampleLevel.hpp"

#include "Audio/Audio.hpp"
#include "Core/Application.hpp"
#include "Core/RenderEngine.hpp"
#include "ECS/Components/AnimationComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Rendering/Model.hpp"
#include "Utility/UtilityFunctions.hpp"

using namespace Graphics;
using namespace ECS;

void ExampleLevel::Install()
{
    World::Level::Install();
}

