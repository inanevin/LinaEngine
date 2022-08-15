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

#include "FeatureRenderers/StaticMeshRenderer.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "Core/Renderer.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Material.hpp"

namespace Lina::Graphics
{
    void StaticMeshRenderer::Initialize()
    {
        auto& fr = Renderer::Get()->GetFeatureRendererManager();
        fr.RegisterVisibility(FeatureRendererVisibilityStage::FetchVisibility, std::bind(&StaticMeshRenderer::OnFetchVisibility, this));
        fr.RegisterVisibility(FeatureRendererVisibilityStage::AssignVisibility, std::bind(&StaticMeshRenderer::OnAssignVisibility, this));
        fr.RegisterViewStage(FeatureRendererViewStage::OnExtractView, std::bind(&StaticMeshRenderer::OnExtractPerView, this, std::placeholders::_1));
        fr.RegisterDefaultStage(FeatureRendererDefaultStage::OnPrepare, std::bind(&StaticMeshRenderer::OnPrepare, this));
        fr.RegisterSubmit(FeatureRendererSubmitStage::OnSubmit, std::bind(&StaticMeshRenderer::OnSubmit, this, std::placeholders::_1));
    }

    void StaticMeshRenderer::Shutdown()
    {
    }

    void StaticMeshRenderer::OnFetchVisibility()
    {
        PROFILER_FUNC("Static Mesh Renderer");
        auto* ecs = ECS::Registry::Get();
        if (ecs == nullptr)
            return;

        const auto& view = ecs->view<ECS::ModelNodeComponent>();

        for (auto e : view)
        {
            auto modelComp = view.get<ECS::ModelNodeComponent>(e);
            if (!modelComp.visible)
                continue;

            auto entityData = ecs->get<ECS::EntityDataComponent>(e);

            // Generate visibility data
            VisibilityData visData;
            visData.position = entityData.GetPosition();
            visData.entity   = e;

            if (modelComp.m_node.IsValid())
                visData.aabb = modelComp.m_node.value->GetAABB();
            else
                visData.aabb = RenderEngine::Get()->GetPlaceholderModelNode()->GetAABB();

            // Generate renderable data
            RenderableData renderable;
            renderable.node = modelComp.m_node.IsValid() ? RenderEngine::Get()->GetPlaceholderModelNode() : modelComp.m_node.value;

            for (auto matHandle : modelComp.m_materials)
            {
                if (matHandle.IsValid())
                    renderable.materials.push_back(matHandle.value);
                else
                    renderable.materials.push_back(RenderEngine::Get()->GetPlaceholderMaterial());
            }

            FetchedData fetchedData;
            fetchedData.visibility = visData;
            fetchedData.renderable = renderable;
            m_fetchedObjects.push_back(fetchedData);
        }
    }

    void StaticMeshRenderer::OnAssignVisibility()
    {
        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);
        linatl::for_each(m_fetchedObjects.begin(), m_fetchedObjects.end(), [](const FetchedData& fd) { RenderEngine::Get()->GetFramePacket().AddVisibilityData(fd.visibility); });
    }

    void StaticMeshRenderer::OnExtractPerView(View* v)
    {
        if (v->GetType() != ViewType::Player)
            return;

        const auto& visibles = v->GetVisibleObjects();
        for (const auto& r : m_fetchedObjects)
        {
            // Skip if this object is not visible by this view.
            if (!v->IsVisibleByView(r.visibility.entity))
                continue;

            m_renderableObjects.push_back(r.renderable);
        }

        m_fetchedObjects.clear();
    }

    void StaticMeshRenderer::OnPrepare()
    {
        Vector<GPUData> opaques;
        Vector<GPUData> transparents;

        for (auto& renderable : m_renderableObjects)
        {
            const auto& meshes    = renderable.node->GetMeshes();
            const auto& materials = renderable.materials;

            for (int i = 0; i < meshes.size(); i++)
            {
                GPUData gpuData;
                gpuData.mesh = meshes[i];
                gpuData.mat  = materials[i];

                const auto& handle = gpuData.mat->GetShaderHandle();
                Shader*     shader = nullptr;
                if (!handle.IsValid())
                    gpuData.mat = RenderEngine::Get()->GetPlaceholderMaterial();

                shader = gpuData.mat->GetShaderHandle().value;

                if (shader->GetSurfaceType() == SurfaceType::Opaque)
                    opaques.push_back(gpuData);
                else
                    transparents.push_back(gpuData);
            }
        }

        linatl::copy(opaques.begin(), opaques.end(), linatl::back_inserter(m_gpuData));
        linatl::copy(transparents.begin(), transparents.end(), linatl::back_inserter(m_gpuData));
        opaques.clear();
        transparents.clear();
    }

    Material* lastMaterial;
    Mesh*     lastMesh;

    void StaticMeshRenderer::OnSubmit(CommandBuffer& buffer)
    {
        glm::vec3 camPos = {0.f, 0.f, -350.f};

        static float _frameNumber = 0.0f;
        _frameNumber += 0.1f;

        glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
        // camera projection
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1200.0f / 1200.0f, 0.1f, 1000.0f);
        projection[1][1] *= -1;

        for (auto& renderable : m_gpuData)
        {
            if (renderable.mat != lastMaterial)
            {
                renderable.mat->GetShaderHandle().value->GetPipeline().Bind(buffer, PipelineBindPoint::Graphics);
                renderable.mat = lastMaterial;
            }

            const glm::mat4 meshMatrix = projection * view * renderable.transform;

            Graphics::MeshPushConstants constants;
            constants.renderMatrix = meshMatrix;

            buffer.PushConstants(lastMaterial->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);

            if (renderable.mesh != lastMesh)
            {
                buffer.Draw(static_cast<uint32>(renderable.mesh->GetVertices().size()), 1, 0, 0);
                lastMesh = renderable.mesh;
            }
        }
    }

} // namespace Lina::Graphics
