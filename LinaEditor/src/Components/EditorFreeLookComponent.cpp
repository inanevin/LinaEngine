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

#include "Components/EditorFreeLookComponent.hpp"
#include "Core/InputEngine.hpp"
#include "Core/Entity.hpp"
#include "Core/World.hpp"
#include <GLFW/glfw3.h>

namespace Lina::World
{
    void EditorFreeLookComponent::OnTick(const Event::ETick& ev)
    {
        if (ev.isInPlayMode)
            return;

       // Entity* e = World::EntityWorld::Get()->GetEntity("Test");
       // static float sa = 0.0f;
       // sa += 0.01f;
       // e->SetPosition(Vector3(Math::Sin(sa) * 5, 0, 0));
       // return;
        auto*         inputEngine            = Input::InputEngine::Get();
        const Vector2 mouseAxis              = inputEngine->GetMouseAxis();
        bool          middleMouseControlsPos = false;

        if (inputEngine->GetMouseButton(LINA_MOUSE_RIGHT))
        {
            m_angles.y += mouseAxis.y * rotationPower;
            m_angles.x += mouseAxis.x * rotationPower;

            Quaternion qX       = Quaternion::AxisAngle(Vector3::Up, m_angles.x);
            Quaternion qY       = Quaternion::AxisAngle(Vector3::Right, m_angles.y);
            Quaternion localRot = m_entity->GetLocalRotation();
            Quaternion target   = Quaternion::Slerp(localRot, qX * qY, ev.deltaTime * 25);
            m_entity->SetLocalRotation(target);
        }
        else if (inputEngine->GetMouseButton(LINA_MOUSE_MIDDLE))
        {
            middleMouseControlsPos = true;
            const Vector2 delta    = inputEngine->GetMousePosition() - m_lastMousePos;

            m_entity->AddPosition(-m_entity->GetRotation().GetRight() * delta.x * ev.deltaTime * 5);
            m_entity->AddPosition(m_entity->GetRotation().GetUp() * delta.y * ev.deltaTime * 5);
        }

        m_lastMousePos = inputEngine->GetMousePosition();

        const Vector2 scroll = inputEngine->GetMouseScroll();
        m_entity->AddPosition(m_entity->GetRotation().GetForward() * scroll.y * ev.deltaTime * 250);

        // Handle movement.
        float      horizontalKey    = inputEngine->GetHorizontalAxisValue();
        float      verticalKey      = inputEngine->GetVerticalAxisValue();
        float      sprintMultiplier = Input::InputEngine::Get()->GetKey(Input::InputCode::LSHIFT) ? 3.0f : 1.0f;
        Quaternion rotation         = m_entity->GetRotation();
        Vector3    fw               = rotation.GetForward();
        Vector3    up               = rotation.GetUp();
        Vector3    rg               = rotation.GetRight();

        m_entity->AddPosition(verticalKey * ev.deltaTime * movementSpeed * sprintMultiplier * fw.Normalized());
        m_entity->AddPosition(horizontalKey * ev.deltaTime * movementSpeed * sprintMultiplier * rg.Normalized());

        // LINA_TRACE("{0}", m_entity->GetPosition().ToString());
    }
} // namespace Lina::World
