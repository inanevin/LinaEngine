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
#include "World/Core/Entity.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Graphics/Core/WindowManager.hpp"

namespace Lina::Graphics
{

    void CameraSystem::CalculateCamera(CameraComponent* cam)
    {
        const float aspect = m_windowManager->GetMainWindow().GetAspect();
        if (aspect == 0.0f)
            return;

        CalculateCamera(cam, aspect);
    }

    void CameraSystem::CalculateCamera(CameraComponent* cam, float aspect)
    {
        if (aspect == 0.0f)
            aspect = m_windowManager->GetMainWindow().GetAspect();

        World::Entity*   e      = cam->GetEntity();
        const Vector3    pos    = e->GetPosition();
        const Quaternion camRot = e->GetRotation();
        cam->m_view             = Matrix::InitLookAt(pos, pos + camRot.GetForward(), camRot.GetUp());
        cam->m_projection       = Matrix::Perspective(cam->fieldOfView / 2.0f, aspect, cam->zNear, cam->zFar);

        const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);
        cam->m_projection = clip * cam->m_projection;
        // m_proj[1][1] *= -1;
    }
} // namespace Lina::Graphics
