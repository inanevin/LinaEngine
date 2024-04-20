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

#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Components/ModelComponent.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/System/System.hpp"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Lina
{
#define MAX_GFX_COMMANDS  250
#define MAX_COPY_COMMANDS 50
#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100

	WorldRenderer::WorldRenderer(GfxManager* man, EntityWorld* world, const Vector2ui& viewSize)
	{
        m_world = world;
		m_gfxManager = man;
		m_guiBackend = m_gfxManager->GetGUIBackend();
		m_lgx		 = m_gfxManager->GetLGX();
        m_world->AddListener(this);
        
        auto* rm = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
        m_skyCube = rm->GetResource<Model>("Resources/Core/Models/SkyCube.glb"_hs)->GetFirstNodeWMesh();
        m_skyShader = rm->GetResource<Shader>("Resources/Core/Shaders/Sky/SimpleSky.linashader"_hs);
        
		// for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		// {
		// 	auto& data					 = m_pfd[i];
		//
		// 	data.copyStream				 = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, "WorldRenderer: Copy Stream"});
		// 	data.copySemaphore.semaphore = m_lgx->CreateUserSemaphore();
		// 	data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "World Renderer GUI Vertex Buffer");
		// 	data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "World Renderer GUI Index Buffer");
		// 	data.guiMaterialBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, MAX_GUI_MATERIALS * sizeof(GPUMaterialGUI));
		// }
        
        m_size = viewSize;
        
        LinaGX::TextureDesc rtDesc = {
            .format                   = DEFAULT_RT_FORMAT_HDR,
            .flags                    = LinaGX::TF_ColorAttachment,
            .width                    = viewSize.x,
            .height                   = viewSize.y,
            .debugName                = "WorldRendererTexture",
        };
        
        LinaGX::TextureDesc depthDesc = {
            .format = LinaGX::Format::D32_SFLOAT,
            .depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
            .flags                    = LinaGX::TF_DepthTexture | LinaGX::TF_Sampled,
            .width                    = viewSize.x,
            .height                   = viewSize.y,
            .debugName                = "WorldRendererDepthTexture",
        };
        
        
        for(uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& data = m_pfd[i];
            
            data.gfxStream                 = m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "WorldRenderer: Gfx Stream"});
            data.colorTarget = m_lgx->CreateTexture(rtDesc);
            data.depthTarget = m_lgx->CreateTexture(depthDesc);
            
            m_mainPass.SetColorAttachment(i, 0,{.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.colorTarget, .isSwapchain = false} );
            m_mainPass.DepthStencilAttachment(i, {.useDepth = true, .texture = data.depthTarget, .clearDepth = 1.0f});
        }
        
        m_mainPass.Create(m_gfxManager, RenderPassDescriptorType::Main);
        
        FetchRenderables();
	}

    WorldRenderer::~WorldRenderer()
    {
        for(uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& data = m_pfd[i];
            m_lgx->DestroyCommandStream(data.gfxStream);
            m_lgx->DestroyTexture(data.colorTarget);
            m_lgx->DestroyTexture(data.depthTarget);
        }
        
        m_mainPass.Destroy();
        m_world->RemoveListener(this);
    }

    void WorldRenderer::FetchRenderables()
    {
        m_modelComponents.clear();
        m_world->GetAllComponents<ModelComponent>(m_modelComponents);
    }

    void WorldRenderer::OnComponentAdded(Component *c)
    {
        if(c->GetComponentType() == GetTypeID<ModelComponent>())
            m_modelComponents.push_back(static_cast<ModelComponent*>(c));
    }

    void WorldRenderer::OnComponentRemoved(Component *c)
    {
        if(c->GetComponentType() == GetTypeID<ModelComponent>())
            m_modelComponents.erase(linatl::find_if(m_modelComponents.begin(), m_modelComponents.end(), [c](ModelComponent* model) -> bool { return c == model; }));
    }

	void WorldRenderer::Tick(float delta)
	{       
        CameraComponent* camera = m_world->GetActiveCamera();

        m_gpuDataView = {
            .screenSize = Vector2(static_cast<float>(m_size.x), static_cast<float>(m_size.y)),
                                  };
        if(camera == nullptr)
        {
            m_gpuDataView.view = Matrix4::InitLookAt(Vector3(5,5,5), Vector3::Zero, Vector3::Up);
            m_gpuDataView.proj = Matrix4::Perspective(90, static_cast<float>(m_size.x) / static_cast<float>(m_size.y), 0.1f, 100.0f);
            m_gpuDataView.viewProj = m_gpuDataView.proj * m_gpuDataView.view;
            m_gpuDataView.cameraNearFar = Vector2(0.1f, 100.0f);
        }
        else
        {
            
        }
        
        m_gpuDataScene = {.skyTop = Vector4(1,0,0,1), .skyHor = Vector4(1,1,1,1), .skyBot = Vector4(1,0,0,1)};
        
        

	}

	LinaGX::CommandStream* WorldRenderer::Render(uint32 frameIndex, int32 threadIndex)
	{
        auto& currentFrame = m_pfd[frameIndex];

        const LinaGX::Viewport viewport = {
            .x          = 0,
            .y          = 0,
            .width      = m_size.x,
            .height      = m_size.y,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        const LinaGX::ScissorsRect scissors = {
            .x        = 0,
            .y        = 0,
            .width    = m_size.x,
            .height = m_size.y,
        };
        
        // Barrier to Attachment
        LinaGX::CMDBarrier* barrierToAttachment    = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
        barrierToAttachment->srcStageFlags        = LinaGX::PSF_TopOfPipe;
        barrierToAttachment->dstStageFlags        = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
        barrierToAttachment->textureBarrierCount = 2;
        barrierToAttachment->textureBarriers        = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
        barrierToAttachment->textureBarriers[0]    = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.colorTarget);
        barrierToAttachment->textureBarriers[1]    = GfxHelpers::GetTextureBarrierDepthRead2Att(currentFrame.depthTarget);
        
        
        
        m_gfxManager->GetMeshManager().BindBuffers(currentFrame.gfxStream, 0);

        m_mainPass.GetBuffer(frameIndex, 0).BufferData(0, (uint8*)&m_gpuDataView, sizeof(GPUDataView));
        m_mainPass.GetBuffer(frameIndex, 1).BufferData(0, (uint8*)&m_gpuDataScene, sizeof(GPUDataScene));
        m_mainPass.BindDescriptors(currentFrame.gfxStream, frameIndex);
        
        m_mainPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
        
        DrawSky(currentFrame.gfxStream);
        
        m_mainPass.End(currentFrame.gfxStream);
        
        // Barrier to shader read.
        LinaGX::CMDBarrier* barrierFromAttachment    = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
        barrierFromAttachment->srcStageFlags        = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
        barrierFromAttachment->dstStageFlags        = LinaGX::PSF_FragmentShader;
        barrierFromAttachment->textureBarrierCount = 2;
        barrierFromAttachment->textureBarriers        = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
        barrierFromAttachment->textureBarriers[0]    = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.colorTarget);
        barrierFromAttachment->textureBarriers[1]    = GfxHelpers::GetTextureBarrierDepthRead2Att(currentFrame.depthTarget);
        
        m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);
        return currentFrame.gfxStream;
	}

    void WorldRenderer::DrawSky(LinaGX::CommandStream* stream)
    {
        m_skyShader->Bind(stream, m_skyShader->GetGPUHandle());
        
        m_skyCube->GetMesh()->Draw(stream, 1);
    }
} // namespace Lina
