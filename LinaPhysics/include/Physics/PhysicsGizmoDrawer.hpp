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
Timestamp: 9/30/2020 4:39:19 PM

*/
#pragma once

#ifndef PhysicsGizmoDrawer_HPP
#define PhysicsGizmoDrawer_HPP

#include "btBulletDynamicsCommon.h"
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Color.hpp"
#include <functional>

// Headers here.

namespace LinaEngine
{
	class Application;
}

class PhysicsGizmoDrawer : public btIDebugDraw
{
public:


	void Setup(std::function<void(LinaEngine::Vector3, LinaEngine::Vector3, LinaEngine::Color, float)>& cb) { m_drawLineCallback = cb; }

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		
	}

	virtual void reportErrorWarning(const char* warningString)
	{

	}

	virtual void draw3dText(const btVector3& location, const char* textString)
	{

	}

	virtual void setDebugMode(int debugMode)
	{
		m_debugMode = (DebugDrawModes)debugMode;
	}

	virtual int getDebugMode() const {
		return m_debugMode;
	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) ;

private:
	
	DebugDrawModes m_debugMode;
	std::function<void(LinaEngine::Vector3, LinaEngine::Vector3, LinaEngine::Color, float)> m_drawLineCallback;
};

#endif
