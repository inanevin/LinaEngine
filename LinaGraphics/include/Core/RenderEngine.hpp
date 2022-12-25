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

#pragma once

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "RenderData.hpp"
#include "Data/String.hpp"
#include "Utility/DeletionQueue.hpp"
#include "PipelineObjects/DescriptorSetLayout.hpp"
#include "PipelineObjects/DescriptorPool.hpp"
#include "PipelineObjects/PipelineLayout.hpp"
#include "PipelineObjects/UploadContext.hpp"
#include "PipelineObjects/CommandBuffer.hpp"
#include "PipelineObjects/CommandPool.hpp"
#include "PipelineObjects/Framebuffer.hpp"
#include "PipelineObjects/RenderPass.hpp"
#include "Backend.hpp"
#include "Window.hpp"
#include "CameraSystem.hpp"
#include "View.hpp"

namespace Lina
{
    namespace Event
    {
        struct EEngineResourcesLoaded;
        struct EEWindowResized;
        struct EEWindowPositioned;
        struct EPreMainLoop;
    } // namespace Event
} // namespace Lina

namespace Lina::Graphics
{
    class Model;
    class Texture;
    class ModelNode;
    class Material;
    class GUIBackend;
    class Renderer;
    class Shader;

    class RenderEngine
    {

    public:
        RenderEngine()  = default;
        ~RenderEngine() = default;

        // void CreateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size);
        // void UpdateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size);
        // void DestroyAdditionalWindow(const String& nameID);
        void           AddRenderer(Renderer* renderer);
        void           RemoveRenderer(Renderer* renderer);
        Vector<String> GetEngineShaderPaths();
        Vector<String> GetEngineMaterialPaths();
        Vector<String> GetEnginePrimitivePaths();
        Vector<String> GetEngineTexturePaths();
        Mesh*          GetPlaceholderMesh();

        static inline RenderEngine* Get()
        {
            return s_instance;
        }

        inline Model* GetPlaceholderModel()
        {
            return m_placeholderModel;
        }

        inline ModelNode* GetPlaceholderModelNode()
        {
            return m_placeholderModelNode;
        }

        inline Material* GetPlaceholderMaterial()
        {
            return m_placeholderMaterial;
        }

        inline DeletionQueue& GetMainDeletionQueue()
        {
            return m_mainDeletionQueue;
        }

        inline bool IsInitialized()
        {
            return m_initedSuccessfully;
        }

        inline DescriptorSetLayout& GetLayout(DescriptorSetType set)
        {
            return m_descriptorLayouts[set];
        }

        inline PipelineLayout& GetGlobalAndPassLayouts()
        {
            return m_globalAndPassLayout;
        }

        inline UploadContext& GetGPUUploader()
        {
            return m_gpuUploader;
        }

        inline Material* GetEngineMaterial(EngineShaderType shader)
        {
            return m_engineMaterials[shader];
        }

        inline Model* GetEngineModel(EnginePrimitiveType primitive)
        {
            return m_engineModels[primitive];
        }

        inline Texture* GetEngineTexture(EngineTextureType texture)
        {
            return m_engineTextures[texture];
        }

        inline Shader* GetEngineShader(EngineShaderType sh)
        {
            return m_engineShaders[sh];
        }

        inline const Viewport& GetViewport() const
        {
            return m_viewport;
        }

        inline const Recti& GetScissors() const
        {
            return m_scissors;
        }

        inline uint32 GetFrameIndex()
        {
            return m_frameNumber % FRAMES_IN_FLIGHT;
        }

        inline CameraSystem& GetCameraSystem()
        {
            return m_cameraSystem;
        }

        inline const View& GetPlayerView()
        {
            return m_playerView;
        }

        inline const RenderPass& GetRenderPass(RenderPassType rpType)
        {
            return m_renderPasses[rpType];
        }
        // inline HashMap<StringID, AdditionalWindow>& GetAdditionalWindows()
        // {
        //     return m_additionalWindows;
        // }

    private:
        friend class Engine;

        void Initialize(const InitInfo& initInfo);
        void Clear();
        void Tick();
        void SyncData();
        void Render();
        void Stop();
        void Shutdown();
        void SwapchainRecreated();
        void OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev);
        void OnPreMainLoop(const Event::EPreMainLoop& ev);
        void OnOutOfDateImageHandled(Swapchain* swp);
        void Join();

    private:
        static RenderEngine* s_instance;

        Model*        m_placeholderModel     = nullptr;
        ModelNode*    m_placeholderModelNode = nullptr;
        Material*     m_placeholderMaterial  = nullptr;
        DeletionQueue m_mainDeletionQueue;
        InitInfo      m_appInfo;
        Backend       m_backend;
        bool          m_initedSuccessfully = false;
        Window*       m_window             = nullptr;

        UploadContext                                   m_gpuUploader;
        DescriptorPool                                  m_descriptorPool;
        HashMap<DescriptorSetType, DescriptorSetLayout> m_descriptorLayouts;
        PipelineLayout                                  m_globalAndPassLayout;
        GUIBackend*                                     m_guiBackend;
        Viewport                                        m_viewport;
        Recti                                           m_scissors;
        HashMap<RenderPassType, RenderPass>             m_renderPasses;

        CommandPool       m_cmdPool;
        uint32            m_frameNumber = 0;
        Frame             m_frames[FRAMES_IN_FLIGHT];
        Vector<Renderer*> m_renderers;
        CameraSystem      m_cameraSystem;
        View              m_playerView;

        //  HashMap<StringID, AdditionalWindow>  m_additionalWindows;

        // Resources
        HashMap<EngineShaderType, String>    m_engineShaderNames;
        HashMap<EnginePrimitiveType, String> m_enginePrimitiveNames;
        HashMap<EngineTextureType, String>   m_engineTextureNames;
        HashMap<EngineShaderType, Shader*>   m_engineShaders;
        HashMap<EngineShaderType, Material*> m_engineMaterials;
        HashMap<EnginePrimitiveType, Model*> m_engineModels;
        HashMap<EngineTextureType, Texture*> m_engineTextures;
    };
} // namespace Lina::Graphics

#endif
