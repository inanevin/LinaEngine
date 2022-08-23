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
#include "Components/ModelNodeComponent.hpp"
#include "Core/Renderer.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Material.hpp"
#include "Resource/ModelNode.hpp"
#include "Core/World.hpp"

namespace Lina::Graphics
{
    void StaticMeshRenderer::Initialize(FeatureRendererManager& manager)
    {
        manager.onFetchVisibility.push_back(std::bind(&StaticMeshRenderer::OnFetchVisibility, this, std::placeholders::_1));
        manager.onAssignVisibility.push_back(std::bind(&StaticMeshRenderer::OnAssignVisibility, this, std::placeholders::_1));
        manager.onExtractPerView.push_back(std::bind(&StaticMeshRenderer::OnExtractPerView, this, std::placeholders::_1));
        manager.onPrepare.push_back(std::bind(&StaticMeshRenderer::OnPrepare, this));
        manager.onSubmitPerView.push_back(std::bind(&StaticMeshRenderer::OnSubmitPerView, this, std::placeholders::_1, std::placeholders::_2));
        m_renderData[0].m_visibleObjects.reserve(1000);
    }

    void StaticMeshRenderer::Shutdown()
    {
    }

    void StaticMeshRenderer::OnFetchVisibility(World::EntityWorld* world)
    {
        PROFILER_FUNC("Static Mesh Renderer");
        uint32      compSize = 0;
        const auto& comps    = world->View<ModelNodeComponent>(&compSize);

        Model*     placeholderModel = RenderEngine::Get()->GetPlaceholderModel();
        ModelNode* placeholderNode  = RenderEngine::Get()->GetPlaceholderModelNode();

         for (uint32 i = 0; i < compSize; i++)
         {
             ModelNodeComponent* comp = comps[i];
             if (!comp->IsVisible())
                 continue;
        
             // if (comp)
             // {
             //     Model*     model = comp->m_modelHandle.IsValid() ? comp->m_modelHandle.value : placeholderModel;
             //     ModelNode* node  = comp->m_modelHandle.IsValid() ? model->GetNodes()[comp->m_nodeIndex] : placeholderNode;
             // 
             //     VisibilityData visdata;
             //     visdata.position   = comp->GetEntity()->GetPosition();
             //     visdata.renderable = comp;
             //     visdata.aabb       = node->GetAABB();
             // 
             //     // GetDataToWrite().m_visibleObjects.insert(visdata);
             // }
         }
    }

    void StaticMeshRenderer::OnAssignVisibility(FramePacket& fp)
    {
        return;
        PROFILER_FUNC(PROFILER_THREAD_SIMULATION);
        linatl::for_each(GetDataToWrite().m_visibleObjects.begin(), GetDataToWrite().m_visibleObjects.end(), [&](const VisibilityData& vd) { fp.visibles.insert(vd); });
    }

    void StaticMeshRenderer::OnExtractPerView(View* v)
    {
        return;
        if (v->GetType() != ViewType::Player)
            return;

        const auto& visibles     = v->GetVisibleObjects();
        auto&       myVisibles   = GetDataToWrite().m_visibleObjects;
        auto&       visibleNodes = GetDataToWrite().m_visibleNodeComponents;

        for (auto& vis : myVisibles)
        {
            // Skip if this object is not visible by this view.
            // if (!v->IsVisibleByView(vis.renderable))
            //     continue;

            visibleNodes.insert(static_cast<ModelNodeComponent*>(vis.renderable));
        }

        myVisibles.clear();
        m_dataToWrite = m_dataToWrite == 0 ? 1 : 0;
    }

    void StaticMeshRenderer::OnPrepare()
    {

        auto data = GetDataToRead().m_visibleNodeComponents;

        return;

        for (auto comp : data)
        {
            Model*     model = comp->m_modelHandle.IsValid() ? comp->m_modelHandle.value : RenderEngine::Get()->GetPlaceholderModel();
            ModelNode* node  = comp->m_modelHandle.IsValid() ? model->GetNodes()[comp->m_nodeIndex] : RenderEngine::Get()->GetPlaceholderModelNode();

            for (auto m : node->GetMeshes())
            {
                const uint32 meshSlot = static_cast<uint32>(m->GetMaterialSlot());
                auto&        handle   = comp->m_materials[meshSlot];
                Material*    mat      = handle.IsValid() ? handle.value : RenderEngine::Get()->GetPlaceholderMaterial();

                RenderPair pair = RenderPair{
                    .transform = comp->GetEntity()->GetTransform().ToMatrix(),
                    .mesh      = m,
                };

                GetDataToRead().m_gpuData[mat].push_back(pair);
            }
        }

        GetDataToRead().m_visibleNodeComponents.clear();
    }

    void StaticMeshRenderer::OnSubmitPerView(CommandBuffer& buffer, View* v)
    {
        return;
        if (v->GetType() != ViewType::Player)
            return;

        static float _frameNumber = 0.0f;
        _frameNumber += 0.1f;

        Material* mat  = Resources::ResourceStorage::Get()->GetResource<Material>("Resources/Engine/Materials/Default.linamat");
        Model*    mod  = Resources::ResourceStorage::Get()->GetResource<Model>("Resources/Engine/Meshes/BlenderMonkey.obj");
        Mesh*     mesh = mod->GetRootNode()->GetMeshes()[0];
        mat->GetShaderHandle().value->GetPipeline().Bind(buffer, PipelineBindPoint::Graphics);
        uint64 offset = 0;
        buffer.BindVertexBuffers(0, 1, mesh->GetGPUVtxBuffer().buffer, &offset);

        for (int i = -6000; i < 6000; i++)
        {
            Matrix    t          = Matrix::Translate(Vector3(i * 2, 0, 0));
            glm::mat4 meshMatrix = v->GetProj() * v->GetView() * t;

            Graphics::MeshPushConstants constants;
            constants.renderMatrix = meshMatrix;
            buffer.PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);

            buffer.Draw(static_cast<uint32>(mesh->GetVertices().size()), 1, 0, 0);
        }
        // for (auto [mat, pair] : GetDataToRead().m_gpuData)
        // {
        //     mat->GetShaderHandle().value->GetPipeline().Bind(buffer, PipelineBindPoint::Graphics);
        //
        //     for (auto& rp : pair)
        //     {
        //         Matrix t = rp.transform ;
        //         glm::mat4 meshMatrix = v->GetProj()  * v->GetView() * t;
        //
        //         Graphics::MeshPushConstants constants;
        //         constants.renderMatrix = meshMatrix;
        //         buffer.PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetShaderStage(ShaderStage::Vertex), 0, sizeof(Graphics::MeshPushConstants), &constants);
        //
        //         uint64 offset = 0;
        //         buffer.BindVertexBuffers(0, 1, rp.mesh->GetGPUVtxBuffer().buffer, &offset);
        //
        //         buffer.Draw(static_cast<uint32>(rp.mesh->GetVertices().size()), 1, 0, 0);
        //     }
        // }

        GetDataToRead().m_gpuData.clear();
        m_dataToRead = m_dataToRead == 1 ? 0 : 1;
    }

} // namespace Lina::Graphics
