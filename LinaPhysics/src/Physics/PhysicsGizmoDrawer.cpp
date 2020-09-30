/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: PhysicsGizmoDrawer
Timestamp: 9/30/2020 5:01:29 PM

*/

#include "Physics/PhysicsGizmoDrawer.hpp"

#define LINE_WIDTH 2.0f

void PhysicsGizmoDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	m_drawLineCallback(LinaEngine::Vector3(from.getX(), from.getY(), from.getZ()), LinaEngine::Vector3(to.getX(), to.getY(), to.getZ()), LinaEngine::Color(color.getX(), color.getY(), color.getZ()), LINE_WIDTH);
}

