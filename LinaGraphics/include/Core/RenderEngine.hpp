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

#include "Renderer.hpp"
#include "Data/String.hpp"
#include "Utility/DeletionQueue.hpp"
#include "PipelineObjects/DescriptorSetLayout.hpp"
#include "PipelineObjects/PipelineLayout.hpp"
#include "PipelineObjects/UploadContext.hpp"
#include "PipelineObjects/Semaphore.hpp"
#include "PipelineObjects/Fence.hpp"
#include "PipelineObjects/Buffer.hpp"
#include "PipelineObjects/DescriptorSet.hpp"
#include "Backend.hpp"
#include "Window.hpp"

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

    struct AdditionalWindow
    {
        StringID            sid = 0;
        Swapchain*          swapchain;
        Texture*            depthImg = nullptr;
        Vector<Framebuffer> framebuffers;
        Semaphore           waitSemaphore;
        Semaphore           presentSemaphore;
    };

    class RenderEngine
    {
    private:
        struct Frame
        {
            Fence         graphicsFence;
            Semaphore     presentSemaphore;
            Semaphore     graphicsSemaphore;
            Buffer        objDataBuffer;
            Buffer        indirectBuffer;
            Buffer        sceneDataBuffer;
            Buffer        viewDataBuffer;
            Buffer        lightDataBuffer;
            DescriptorSet passDescriptor;
            DescriptorSet globalDescriptor;
        };

    public:
        RenderEngine()  = default;
        ~RenderEngine() = default;

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

        inline Renderer& GetRenderer()
        {
            return m_renderer;
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

        inline DescriptorPool& GetDescriptorPool()
        {
            return m_descriptorPool;
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

        inline Viewport& GetViewport()
        {
            return m_viewport;
        }

        inline Recti& GetScissors()
        {
            return m_scissors;
        }

        inline HashMap<StringID, AdditionalWindow>& GetAdditionalWindows()
        {
            return m_additionalWindows;
        }

        void CreateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size);
        void UpdateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size);
        void DestroyAdditionalWindow(const String& nameID);

        void           Join();
        Vector<String> GetEngineShaderPaths();
        Vector<String> GetEngineMaterialPaths();
        Vector<String> GetEnginePrimitivePaths();
        Vector<String> GetEngineTexturePaths();
        Mesh*          GetPlaceholderMesh();

    private:
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

    private:
        friend class Renderer;
        friend class Engine;

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
        Renderer                                        m_renderer;
        GUIBackend*                                     m_guiBackend;
        Viewport                                        m_viewport;
        Recti                                           m_scissors;

        // Resources
        HashMap<EngineShaderType, String>    m_engineShaderNames;
        HashMap<EnginePrimitiveType, String> m_enginePrimitiveNames;
        HashMap<EngineTextureType, String>   m_engineTextureNames;
        HashMap<EngineShaderType, Shader*>   m_engineShaders;
        HashMap<EngineShaderType, Material*> m_engineMaterials;
        HashMap<EnginePrimitiveType, Model*> m_engineModels;
        HashMap<EngineTextureType, Texture*> m_engineTextures;
        HashMap<StringID, AdditionalWindow>  m_additionalWindows;
    };
} // namespace Lina::Graphics

#endif
