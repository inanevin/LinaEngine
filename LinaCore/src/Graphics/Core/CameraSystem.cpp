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

#include "Graphics/Core/CameraSystem.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "World/Core/Entity.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"

namespace Lina
{
	void CameraSystem::CalculateCamera(CameraComponent& cam, float aspect)
	{
		Entity*			 e		= cam.GetEntity();
		const Vector3	 pos	= e->GetPosition();
		const Quaternion camRot = e->GetRotation();
		cam.m_view				= Matrix4::InitLookAt(pos, pos + camRot.GetForward(), camRot.GetUp());
		cam.m_projection		= Matrix4::Perspective(cam.fieldOfView / 2.0f, aspect, cam.zNear, cam.zFar);

#ifdef LINA_GRAPHICS_DX12
		cam.m_view[2][0] *= -1.0f;
		cam.m_view[2][1] *= -1.0f;
		cam.m_view[2][2] *= -1.0f;
		cam.m_view[2][3] *= -1.0f;
		// cam.m_view[1][1] *= -1;
		
#elif LINA_GRAPHICS_VULKAN
		const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);
		cam.m_projection = clip * cam.m_projection;
#endif
	}
} // namespace Lina
