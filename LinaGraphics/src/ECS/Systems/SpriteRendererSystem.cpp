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

#include "ECS/Systems/SpriteRendererSystem.hpp"

#include "Core/RenderDeviceBackend.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Registry.hpp"
#include "Rendering/Mesh.hpp"
#include "Utility/ModelLoader.hpp"

namespace Lina::ECS
{
    SpriteRendererSystem::~SpriteRendererSystem()
    {
        delete m_quadMesh;
    }

    void SpriteRendererSystem::Initialize(const std::string& name)
    {
        System::Initialize(name);
        m_renderEngine = Graphics::RenderEngineBackend::Get();
        m_renderDevice = m_renderEngine->GetRenderDevice();

        m_quadMesh = new Graphics::Mesh();
        Graphics::ModelLoader::LoadSpriteQuad(*m_quadMesh);
        m_quadMesh->CreateVertexArray(Graphics::BufferUsage::USAGE_STATIC_COPY);
    }


    void SpriteRendererSystem::UpdateComponents(float delta)
    {
        auto view = m_ecs->view<EntityDataComponent, SpriteRendererComponent>();
        m_poolSize = (int)view.size_hint();

        // Find the sprites and add them to the render queue.
        for (auto entity : view)
        {
            SpriteRendererComponent& renderer = view.get<SpriteRendererComponent>(entity);
            EntityDataComponent&     data     = view.get<EntityDataComponent>(entity);

            if (!renderer.GetIsEnabled() || !data.GetIsEnabled())
                return;


            // Dont draw if mesh or material does not exist.
            if (renderer.m_materialID < 0)
                continue;

            Graphics::Material& mat = Graphics::Material::GetMaterial(renderer.m_materialID);
            Render(mat, data.ToMatrix());
        }
    }

    void SpriteRendererSystem::Render(Graphics::Material& material, const Matrix& transformIn)
    {
        m_renderBatch[&material].m_models.push_back(transformIn);
    }

    void SpriteRendererSystem::Flush(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
    {
        // When flushed, all the data is delegated to the render device to do the actual
        // drawing. Then the data is cleared if complete flush is requested.

        for (std::map<Graphics::Material*, BatchModelData>::iterator it = m_renderBatch.begin(); it != m_renderBatch.end(); ++it)
        {
            // Get references.
            BatchModelData& modelData     = it->second;
            size_t          numTransforms = modelData.m_models.size();
            if (numTransforms == 0)
                continue;

            Matrix* models = &modelData.m_models[0];

            // Get the material for drawing, object's own material or overriden material.
            Graphics::Material* mat = overrideMaterial == nullptr ? it->first : overrideMaterial;

            // Update the buffer w/ each transform.
            m_quadMesh->GetVertexArray().UpdateBuffer(2, models, numTransforms * sizeof(Matrix));

            m_renderEngine->UpdateShaderData(mat);
            m_renderDevice->Draw(m_quadMesh->GetVertexArray().GetID(), drawParams, (uint32)numTransforms, m_quadMesh->GetVertexArray().GetIndexCount(), false);

            // Clear the buffer.
            if (completeFlush)
            {
                modelData.m_models.clear();
            }
        }
    }
} // namespace Lina::ECS
