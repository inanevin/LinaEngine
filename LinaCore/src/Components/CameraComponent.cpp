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

#include "Core/Components/CameraComponent.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Math/Matrix.hpp"

namespace Lina
{
	void CameraComponent::PostTick(float delta)
	{
		Matrix4 rotMat			  = Matrix4(m_entity->GetRotation().Inverse());
		Matrix4 translationMatrix = Matrix4::Translate(-m_entity->GetPosition());
		m_view					  = rotMat * translationMatrix;
		const Vector2ui& sz		  = m_world->GetRenderSize();
		m_projection			  = Matrix4::Perspective(m_fieldOfView / 2, static_cast<float>(sz.x) / static_cast<float>(sz.y), m_zNear, m_zFar);
	}
} // namespace Lina
