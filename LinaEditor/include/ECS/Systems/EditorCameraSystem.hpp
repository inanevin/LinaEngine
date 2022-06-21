/*
Class: EditorCameraSystem

Responsible for updating/controlling the editor camera.

Timestamp: 10/27/2020 4:58:35 PM
*/

#pragma once

#ifndef EditorCameraSystem_HPP
#define EditorCameraSystem_HPP

// Headers here.
#include "Math/Vector.hpp"
#include "Core/CommonECS.hpp"
#include "Core/InputEngineFwd.hpp"
#include "ECS/System.hpp"

namespace Lina
{
    class Quaternion;
}

namespace Lina::Editor
{
    class LevelPanel;
}

namespace Lina::ECS
{
    class EditorCameraSystem : public System
    {
    public:
        void         Initialize(const String& name, Editor::LevelPanel& scenePanel);
        virtual void UpdateComponents(float delta) override;
        void         SetEditorCamera(Entity entity)
        {
            m_editorCamera = entity;
        }
        Entity GetEditorCamera()
        {
            return m_editorCamera;
        }
        void SetCameraSpeedMultiplier(float multiplier)
        {
            m_cameraSpeedMultiplier = multiplier;
        }

        inline Vector2 GetMovementSpeeds()
        {
            return m_movementSpeeds;
        }

        inline Vector2 GetRotationSpeeds()
        {
            return m_rotationSpeeds;
        }

        void MoveBehaviour(float delta, Vector2& mouseDragStart, Vector3& location, Quaternion& rotation);
        void RotateBehaviour(float delta, Quaternion& q, Vector2& angles);

    private:
        Vector2             m_movementSpeeds = Vector2::Zero;
        Vector2             m_rotationSpeeds = Vector2::Zero;
        Entity              m_editorCamera   = entt::null;
        Editor::LevelPanel* m_levelPanel;
        Input::InputEngine* m_inputEngine;
        float               m_horizontalKeyAmt      = 0.0f;
        float               m_verticalKeyAmt        = 0.0f;
        float               m_targetXAngle          = 0.0f;
        float               m_targetYAngle          = 0.0f;
        float               m_cameraSpeedMultiplier = 1.0f;
    };
} // namespace Lina::ECS

#endif
