/*
Class: SpriteRendererSystem

Responsible for adding all the sprite renderers into a pool which is then
flushed to draw those renderers' data by the RenderEngine.

Timestamp: 10/1/2020 9:27:40 AM
*/
#pragma once

#ifndef SpriteRendererSystem_HPP
#define SpriteRendererSystem_HPP

#include "Core/RenderEngineFwd.hpp"
#include "ECS/System.hpp"
#include "Math/Matrix.hpp"

#include "Data/Map.hpp"
#include "Data/Vector.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Material;
        class Mesh;
        struct DrawParams;
    } // namespace Graphics
} // namespace Lina

namespace Lina::ECS
{
    class SpriteRendererSystem : public System
    {

        struct BatchModelData
        {
            Vector<Matrix> m_models;
        };

    public:
        SpriteRendererSystem() = default;
        ~SpriteRendererSystem();

        virtual void Initialize(const String& name) override;
        virtual void UpdateComponents(float delta) override;

        void Render(Graphics::Material& material, const Matrix& transformIn);
        void Flush(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial = nullptr, bool completeFlush = true);

    private:
        Graphics::RenderDevice*                       m_renderDevice = nullptr;
        Graphics::RenderEngine*                       m_renderEngine = nullptr;
        Graphics::Mesh*                               m_quadMesh     = nullptr;
        Map<Graphics::Material*, BatchModelData> m_renderBatch;
    };
} // namespace Lina::ECS

#endif
