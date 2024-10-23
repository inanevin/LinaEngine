/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#include "Common/StringID.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Math/Quaternion.hpp"

namespace Lina
{
	class EntityWorld;
}

namespace Lina::Editor
{

	enum class EditorCameraMode
	{
		FreeMovement,
		Orbit,
	};

	class EditorCamera
	{
	public:
		EditorCamera()	= default;
		~EditorCamera() = default;

		void Tick(float delta);

		inline void SetWorld(EntityWorld* world)
		{
			m_world = world;
		}

	private:
		void HandleFreeMovement(float delta);
		void HandleOrbit(float delta);

	private:
		EditorCameraMode m_mode				  = EditorCameraMode::FreeMovement;
		EntityWorld*	 m_world			  = nullptr;
		float			 m_fov				  = 90.0f;
		float			 m_zNear			  = 0.1f;
		float			 m_zFar				  = 500.0f;
		Vector3			 m_absPosition		  = Vector3::Zero;
		Quaternion		 m_absRotation		  = Quaternion::Identity();
		float			 m_movementPower	  = 32.0f;
		float			 m_movementSpeed	  = 16.0f;
		float			 m_rotationPower	  = 20.0f;
		float			 m_rotationSpeed	  = 12.0f;
		Vector2			 m_usedMoveAmt		  = Vector2::Zero;
		Vector3			 m_usedAngles		  = Vector3::Zero;
		Vector3			 m_targetAngles		  = Vector3::Zero;
		Vector2			 m_previousMouseDelta = Vector2::Zero;
	};
} // namespace Lina::Editor
