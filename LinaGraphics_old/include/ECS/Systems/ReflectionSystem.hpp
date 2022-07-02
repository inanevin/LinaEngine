/*
Class: ReflectionSystem



Timestamp: 1/23/2022 2:11:17 AM
*/

#pragma once

#ifndef ReflectionSystem_HPP
#define ReflectionSystem_HPP

// Headers here.
#include "ECS/System.hpp"
#include "Math/Vector.hpp"
#include "Core/RenderEngineFwd.hpp"
#include "Core/CommonApplication.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Material;
        class Texture;
    } // namespace Graphics

    namespace Event
    {
        struct EPlayModeChanged;
    }
} // namespace Lina

namespace Lina::ECS
{
    class ReflectionSystem : public System
    {

    public:
        ReflectionSystem()  = default;
        ~ReflectionSystem() = default;
        virtual void Initialize(const String& name, ApplicationMode& appMode);
        virtual void UpdateComponents(float delta){UpdateReflectionData();};

        /// <summary>
        /// Goes through the reflection areas & captures their cubemaps if they are dynamic.
        /// </summary>
        void UpdateReflectionData();

        /// <summary>
        /// Sets the reflection area map on a material.
        /// </summary>
        void SetReflectionsOnMaterial(Graphics::Material* mat, const Vector3& transformLocation);

    private:
        void ConstructRefAreaTexture(Graphics::Texture* texture, const Vector2i& res);
        void OnPlayModeChanged(const Event::EPlayModeChanged& ev);

    private:
        Graphics::RenderEngine* m_renderEngine = nullptr;
        ApplicationMode         m_appMode;
    };
} // namespace Lina::ECS

#endif
