/*
Class: LightingSystem

Responsible for processing lights and light information to be used by rendering engine
to update the shaders that receive lighting.

Timestamp: 5/13/2019 12:49:19 AM
*/

#pragma once

#ifndef LightingSystem_HPP
#define LightingSystem_HPP

#include "Core/CommonApplication.hpp"
#include "Core/RenderEngineFwd.hpp"
#include "Core/SizeDefinitions.hpp"
#include "ECS/System.hpp"
#include "Math/Color.hpp"
#include "Math/Matrix.hpp"

#include <tuple>
#include "Data/Vector.hpp"

namespace Lina::ECS
{
    struct EntityDataComponent;
    struct DirectionalLightComponent;
    struct PointLightComponent;
    struct SpotLightComponent;

    class LightingSystem : public System
    {
    public:
        LightingSystem() = default;

        void         Initialize(const String& name, ApplicationMode& appMode);
        virtual void UpdateComponents(float delta) override;
        void         SetLightingShaderData(uint32 shaderID);
        void         ResetLightData();
        void         SetAmbientColor(Color col)
        {
            m_ambientColor = col;
        }

        Matrix              GetDirectionalLightMatrix();
        Matrix              GetDirLightBiasMatrix();
        Vector<Matrix> GetPointLightMatrices(Vector3 lightPos, Vector2i m_resolution, float near, float farPlane);
        const Vector3&      GetDirectionalLightPos();

        DirectionalLightComponent* GetDirLight()
        {
            return std::get<1>(m_directionalLight);
        }
        Color GetAmbientColor()
        {
            return m_ambientColor;
        }
        Vector<std::tuple<EntityDataComponent*, PointLightComponent*>>& GetPointLights()
        {
            return m_pointLights;
        }

    private:
        ApplicationMode                                                     m_appMode;
        Graphics::RenderDevice*                                             m_renderDevice = nullptr;
        Graphics::RenderEngine*                                             m_renderEngine = nullptr;
        std::tuple<EntityDataComponent*, DirectionalLightComponent*>        m_directionalLight;
        Vector<std::tuple<EntityDataComponent*, PointLightComponent*>> m_pointLights;
        Vector<std::tuple<EntityDataComponent*, SpotLightComponent*>>  m_spotLights;
        Color                                                               m_ambientColor = Color(0.0f, 0.0f, 0.0f);
    };
} // namespace Lina::ECS

#endif
