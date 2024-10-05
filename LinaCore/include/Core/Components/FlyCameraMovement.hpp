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

#include "Core/World/Component.hpp"

namespace Lina
{
	class FlyCameraMovement : public Component
	{
	public:
		FlyCameraMovement() : Component(CF_RECEIVE_TICK){};

		virtual void OnEvent(const ComponentEvent& ev) override;

		virtual TypeID GetComponentType() override
		{
			return GetTypeID<FlyCameraMovement>();
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_movementSpeed << m_rotationSpeed;
		};

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_movementSpeed >> m_rotationSpeed;
		}

		inline void SetMovementPower(float pw)
		{
			m_movementPower = pw;
		}

		inline void SetMovementSpeed(float sp)
		{
			m_movementSpeed = sp;
		}

		inline void SetRotationSpeed(float sp)
		{
			m_rotationSpeed = sp;
		}

		inline void SetRotationPower(float pw)
		{
			m_rotationPower = pw;
		}

		inline void SetRequireMousePressToRotate(bool req)
		{
			m_requireMousePressToRotate = req;
		}

	private:
		void Tick(float delta);

	private:
		LINA_REFLECTION_ACCESS(SimpleFlightMovement);
		bool m_requireMousePressToRotate = true;

		float	m_movementSpeed		 = 20.0f;
		float	m_movementPower		 = 5.0f;
		float	m_rotationSpeed		 = 24.0f;
		float	m_rotationPower		 = 32.0f;
		Vector3 m_targetAngles		 = Vector3::Zero;
		Vector3 m_usedAngles		 = Vector3::Zero;
		Vector2 m_usedMoveAmt		 = Vector2::Zero;
		Vector2 m_previousMouseDelta = Vector2::Zero;
		bool	m_firstTick			 = true;
	};

} // namespace Lina
