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
#include "Core/CommonEngine.hpp"
#include "Input/Core/InputEngine.hpp"
#include "World/Core/Entity.hpp"
#include "World/Core/World.hpp"

namespace Lina::World
{
    void EditorFreeLookComponent::OnComponentCreated()
    {
        Component::OnComponentCreated();
        m_targetEuler = m_entity->GetRotationAngles();
    }

    void EditorFreeLookComponent::OnTick(const Event::ETick& ev)
    {
        if (ev.isInPlayMode)
            return;

        auto*         inputEngine = Input::InputEngine::Get();
        const Vector2 mouseAxis   = inputEngine->GetMouseDeltaRaw() * ev.deltaTime;

        if (inputEngine->GetMouseButtonDown(LINA_MOUSE_RIGHT))
        {
            inputEngine->SetCursorMode(Input::CursorMode::Disabled);
        }
        if (inputEngine->GetMouseButtonUp(LINA_MOUSE_RIGHT))
        {
            inputEngine->SetCursorMode(Input::CursorMode::Visible);
        }

        if (inputEngine->GetMouseButton(LINA_MOUSE_RIGHT))
        {
            m_targetEuler.x += mouseAxis.y * rotationPower;
            m_targetEuler.y += mouseAxis.x * rotationPower;
        }

        Quaternion currentRot = m_entity->GetRotation();
        currentRot            = Quaternion::Slerp(currentRot, Quaternion::Euler(m_targetEuler), ev.deltaTime * 25);
        m_entity->SetRotation(currentRot);

        // Handle movement.
        float         horizontalKey    = inputEngine->GetHorizontalAxisValue();
        float         verticalKey      = inputEngine->GetVerticalAxisValue();
        float         sprintMultiplier = Input::InputEngine::Get()->GetKey(Input::InputCode::KeyLSHIFT) ? 3.0f : 1.0f;
        Quaternion    rotation         = m_entity->GetRotation();
        Vector3       fw               = rotation.GetForward().Normalized();
        Vector3       rg               = rotation.GetRight().Normalized();
        Vector3       up               = rotation.GetUp().Normalized();
        const Vector2 scroll           = inputEngine->GetMouseScroll();

        const float dt = ev.deltaTime;

        Vector3 targetPosition = m_entity->GetPosition();
        targetPosition += fw * verticalKey * sprintMultiplier * movementSpeed * dt;
        targetPosition += rg * horizontalKey * sprintMultiplier * movementSpeed * dt;
        targetPosition += fw * scroll.y * 2.0f * movementSpeed * dt;

        if (inputEngine->GetMouseButton(LINA_MOUSE_MIDDLE))
        {
            const Vector2 delta = inputEngine->GetMousePosition() - m_lastMousePos;
            targetPosition += -rg * delta.x * 0.5f * movementSpeed * dt;
            targetPosition += up * delta.y * 0.5f * movementSpeed * dt;
        }

        m_entity->SetPosition(targetPosition);
        m_lastMousePos = inputEngine->GetMousePosition();
    }
} // namespace Lina::World
