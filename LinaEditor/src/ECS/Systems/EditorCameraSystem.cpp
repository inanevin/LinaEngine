/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "ECS/Systems/EditorCameraSystem.hpp"

#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/InputBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Registry.hpp"
#include "Math/Math.hpp"
#include "Panels/LevelPanel.hpp"

namespace Lina::ECS
{

    void EditorCameraSystem::Initialize(const std::string& name, Editor::LevelPanel& scenePanel)
    {
        System::Initialize(name);
        m_levelPanel  = &scenePanel;
        m_inputEngine = Input::InputEngineBackend::Get();
        m_poolSize = 1;
    }

    void ECS::EditorCameraSystem::UpdateComponents(float delta)
    {
        if (!m_isActive || !m_levelPanel->IsFocused())
            return;

        if (m_editorCamera != entt::null)
        {
            FreeLookComponent& freeLook = m_ecs->get<FreeLookComponent>(m_editorCamera);
            if (!freeLook.GetIsEnabled())
                return;

            EntityDataComponent& data = m_ecs->get<EntityDataComponent>(m_editorCamera);

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
            float      sprintMultiplier = Input::InputEngineBackend::Get()->GetKey(Input::InputCode::LSHIFT) ? 3.0f : 1.0f;
            Quaternion rotation         = data.GetRotation();
            Vector3    fw               = rotation.GetForward();
            Vector3    up               = rotation.GetUp();
            Vector3    rg               = rotation.GetRight();

            data.AddLocation(verticalKey * delta * freeLook.m_movementSpeeds.y * m_cameraSpeedMultiplier * sprintMultiplier * fw.Normalized());
            data.AddLocation(horizontalKey * delta * freeLook.m_movementSpeeds.y * m_cameraSpeedMultiplier * sprintMultiplier * rg.Normalized());

            const float scrollY = Input::InputEngineBackend::Get()->GetMouseScroll().y;

            data.AddLocation(scrollY * delta * -fw.Normalized() * freeLook.m_movementSpeeds.x * m_cameraSpeedMultiplier * sprintMultiplier);
        }
    }
} // namespace Lina::ECS
