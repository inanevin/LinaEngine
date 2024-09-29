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

#include "Core/Components/SimpleFlightMovement.hpp"
#include "Core/World/Entity.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void SimpleFlightMovement::Begin()
	{
	}

	void SimpleFlightMovement::End()
	{
	}

	void SimpleFlightMovement::Tick(float delta)
	{
		if (!m_input->GetMouseButton(LINAGX_MOUSE_1))
			return;

		auto md = m_input->GetMouseDelta();

#ifdef LINAGX_PLATFORM_WINDOWS
		const int clampAmt = 10;
#else
		const int clampAmt = 3;
#endif
		md.x = Math::Clamp((int)md.x, -clampAmt, clampAmt);
		md.y = Math::Clamp((int)md.y, -clampAmt, clampAmt);

		m_targetAngles += Vector2(md.y, md.x) * m_rotationPower;
		m_finalAngles = Math::Lerp(m_finalAngles, m_targetAngles, delta * m_rotationSpeed);
		m_entity->SetRotationAngles(Vector3(m_finalAngles.x, m_finalAngles.y, 0.0f));

		// Calc pos.
		Vector2 input = Vector2::Zero;
		if (m_input->GetKey(LINAGX_KEY_W))
			input.y = 1.0f;
		else if (m_input->GetKey(LINAGX_KEY_S))
			input.y = -1.0f;
		if (m_input->GetKey(LINAGX_KEY_D))
			input.x = 1.0f;
		else if (m_input->GetKey(LINAGX_KEY_A))
			input.x = -1.0f;

		input *= m_movementPower;

		const Vector3 targetPosition = (m_entity->GetPosition() + m_entity->GetRotation().GetForward() * input.y + m_entity->GetRotation().GetRight() * input.x);
		m_entity->SetPosition(Math::Lerp(m_entity->GetPosition(), targetPosition, delta * m_movementSpeed));
	}
} // namespace Lina
