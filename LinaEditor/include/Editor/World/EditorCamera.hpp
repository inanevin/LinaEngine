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
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class EntityWorld;
}

namespace Lina::Editor
{

	class EditorCamera
	{
	public:
		EditorCamera()			= default;
		virtual ~EditorCamera() = default;

		struct CameraProperties
		{
			float fov	= 90.0f;
			float zNear = 0.1f;
			float zFar	= 500.0f;
		};

		void Tick(float delta);

		virtual void OnHandleCamera(float delta) = 0;

		inline void SetWorld(EntityWorld* world)
		{
			m_world = world;
		}

		inline CameraProperties& GetProps()
		{
			return m_props;
		}

	protected:
		EntityWorld*	 m_world	   = nullptr;
		CameraProperties m_props	   = {};
		Vector3			 m_absPosition = Vector3::Zero;
		Quaternion		 m_absRotation = Quaternion::Identity();
	};

	class OrbitCamera : public EditorCamera
	{
	public:
		struct OrbitProperties
		{
			Vector3 targetPoint	  = Vector3::Zero;
			float	startDistance = 5.0f;
			float	minDistance	  = 1.0f;
			float	maxDistance	  = 24.0f;
		};

		OrbitCamera();
		virtual ~OrbitCamera() = default;

		virtual void OnHandleCamera(float delta) override;

		inline OrbitProperties& GetProps()
		{
			return m_orbitProps;
		}

	private:
		Tween			m_initialTransition;
		OrbitProperties m_orbitProps	 = {};
		float			m_xAngle		 = 0.0f;
		float			m_yAngle		 = 0.0f;
		float			m_targetDistance = 0.0f;
	};
} // namespace Lina::Editor
