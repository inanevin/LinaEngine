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

#include "ECS/Systems/FreeLookSystem.hpp"
#include "Core/InputEngine.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Registry.hpp"
#include "Math/Math.hpp"

namespace Lina::ECS
{
    void FreeLookSystem::Initialize(const String& name)
    {
        System::Initialize(name);
        m_inputEngine = Input::InputEngine::Get();
    }

    void FreeLookSystem::UpdateComponents(float delta)
    {
        if (!m_isActive)
            return;
        auto* ecs = ECS::Registry::Get();

        const auto& view = ecs->view<FreeLookComponent>();
        m_poolSize = (int)view.size();

        for (auto entity : view)
        {
            FreeLookComponent& freeLook = ecs->get<FreeLookComponent>(entity);
            if (!freeLook.GetIsEnabled())
                return;

            EntityDataComponent& data = ecs->get<EntityDataComponent>(entity);

            Vector2 mouseAxis = m_inputEngine->GetMouseAxis();

            // Holding right click enables rotating.
            if (m_inputEngine->GetMouseButton(Input::InputCode::Mouse::Mouse2))
            {
                m_targetYAngle += mouseAxis.y * freeLook.m_rotationSpeeds.x;
                m_targetXAngle += mouseAxis.x * freeLook.m_rotationSpeeds.y;

                freeLook.m_angles.y = Math::Lerp(freeLook.m_angles.y, m_targetYAngle, 15 * delta);
                freeLook.m_angles.x = Math::Lerp(freeLook.m_angles.x, m_targetXAngle, 15 * delta);

                Quaternion qX = Quaternion::AxisAngle(Vector3::Up, freeLook.m_angles.x);
                Quaternion qY = Quaternion::AxisAngle(Vector3::Right, freeLook.m_angles.y);
                data.SetLocalRotation(qX * qY);
            }

            // Handle movement.
            float      horizontalKey    = m_inputEngine->GetHorizontalAxisValue();
            float      verticalKey      = m_inputEngine->GetVerticalAxisValue();
            float      sprintMultiplier = Input::InputEngine::Get()->GetKey(Input::InputCode::LSHIFT) ? 3.0f : 1.0f;
            Quaternion rotation         = data.GetRotation();
            Vector3    fw               = rotation.GetForward();
            Vector3    up               = rotation.GetUp();
            Vector3    rg               = rotation.GetRight();

            data.AddLocation(verticalKey * delta * freeLook.m_movementSpeeds.y * sprintMultiplier * fw.Normalized());
            data.AddLocation(horizontalKey * delta * freeLook.m_movementSpeeds.y * sprintMultiplier * rg.Normalized());
        }
    }
} // namespace Lina::ECS
