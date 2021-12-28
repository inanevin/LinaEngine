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

/*
Class: CameraSystem

Responsible for finding cameras in the level and updating the view, projection and other
matrices depending on the most active camera.

Timestamp: 5/2/2019 12:40:46 AM
*/

#pragma once

#ifndef CameraSystem_HPP
#define CameraSystem_HPP

#include "Core/CommonECS.hpp"
#include "ECS/System.hpp"
#include "Math/Matrix.hpp"

namespace Lina
{
    class Color;
}

namespace Lina::ECS
{
    struct EntityDataComponent;
    struct CameraComponent;

    class CameraSystem : public System
    {
    public:
        CameraSystem() = default;
        void         Initialize(float aspect);
        virtual void UpdateComponents(float delta) override;

        /// <summary>
        /// Changes the camera's view matrix for a single frame with the given matrix.
        /// </summary>
        /// <param name="view"></param>
        void InjectViewMatrix(const Matrix& view)
        {
            m_viewMatrixInjected = true;
            m_view               = view;
        }

        /// <summary>
        /// Changes the camera's projection matrix for a single frame with the given matrix.
        /// </summary>
        /// <param name="proj"></param>
        void InjectProjMatrix(const Matrix& proj)
        {
            m_projMatrixInjected = true;
            m_projection         = proj;
        }

        /// <summary>
        /// Converts given screen-space coordinates to world-space coordinates.
        /// Z element is the distance from camera.
        /// (0,0) top-left, (screenSizeX, screenSizeY) bottom-right
        /// </summary>
        /// <param name="screenPos"></param>
        /// <returns></returns>
        static Vector3 ScreenToWorldCoordinates(const Vector3& screenPos);

        /// <summary>
        /// Converts given viewport-space coordinates to world-space coordinates.
        /// (0,0) top-left, (1,1) bottom-right
        /// </summary>
        /// <param name="viewport"></param>
        /// <returns></returns>
        static Vector3 ViewportToWorldCoordinates(const Vector3& viewport);

        /// <summary>
        /// Converts the given world-space coordinates to screen-space coordinates.
        /// (0,0) top-left, (screenSizeX, screenSizeY) bottom-right
        /// </summary>
        /// <param name="world"></param>
        /// <returns></returns>
        static Vector3 WorldToScreenCoordinates(const Vector3& world);

        /// <summary>
        /// Converts the given world-space coordinates to viewport coordinates.
        /// (0,0) top-left, (1,1) bottom-right
        /// </summary>
        /// <param name="world"></param>
        /// <returns></returns>
        static Vector3 WorldToViewportCoordinates(const Vector3& world);

        void SetAspectRatio(float aspect)
        {
            m_aspectRatio = aspect;
        }
        void   SetActiveCamera(Entity cameraOwner);
        Entity GetActiveCamera()
        {
            return m_activeCameraEntity;
        }
        Matrix& GetViewMatrix()
        {
            return m_view;
        }
        Matrix& GetProjectionMatrix()
        {
            return m_projection;
        }
        Vector3          GetCameraLocation();
        Color&           GetCurrentClearColor();
        CameraComponent* GetActiveCameraComponent();

        void OnCameraDestroyed(entt::registry& registry, entt::entity entity)
        {
            if (entity == m_activeCameraEntity)
                m_activeCameraEntity = entt::null;
        }

    private:
        Matrix m_view               = Matrix::Identity();
        Matrix m_projection         = Matrix::Perspective(35, 1.33f, 0.01f, 1000.0f);
        float  m_aspectRatio        = 1.33f;
        bool   m_useDirLightView    = false;
        bool   m_viewMatrixInjected = false;
        bool   m_projMatrixInjected = false;
        Entity m_activeCameraEntity = entt::null;
    };
} // namespace Lina::ECS

#endif
