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

#include "Core/CameraSystem.hpp"
#include "Core/Entity.hpp"
#include "Components/CameraComponent.hpp"
#include "Core/Window.hpp"

namespace Lina::Graphics
{
    void CameraSystem::Tick()
    {
        if (m_activeCamera != nullptr)
        {
            const float aspect = Window::Get()->GetAspect();

            if (aspect == 0.0f)
                return;

            World::Entity*   e      = m_activeCamera->GetEntity();
            const Vector3    camPos = e->GetPosition();
            const Quaternion camRot = e->GetRotation();
            m_pos                   = camPos;
            m_view                  = Matrix::InitLookAt(camPos, camPos + camRot.GetForward(), camRot.GetUp());
            m_proj                  = Matrix::Perspective(m_activeCamera->fieldOfView / 2.0f, aspect, m_activeCamera->zNear, m_activeCamera->zFar);

            const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);
            m_proj = clip * m_proj;
            // m_proj[1][1] *= -1;
        }
    }
} // namespace Lina::Graphics
