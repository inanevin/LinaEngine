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
#include "Graphics/Utility/DeletionQueue.hpp"
#include "Graphics/PipelineObjects/DescriptorSetLayout.hpp"
#include "Graphics/PipelineObjects/PipelineLayout.hpp"
#include "Graphics/PipelineObjects/UploadContext.hpp"
#include "Graphics/PipelineObjects/Buffer.hpp"
#include "Screen.hpp"
#include "Backend.hpp"
#include "WindowManager.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
    namespace Event
    {
        struct EEngineResourcesLoaded;
        struct EWindowPositioned;
        struct EWindowResized;
        struct EPreMainLoop;
        struct ELevelUninstalled;
        struct ELevelInstalled;
        struct EResourceLoaded;
    } // namespace Event

    namespace World
    {
        class LevelManager;
    }
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
    class Mesh;
    class WorldRenderer;
    class SurfaceRenderer;

    class RenderEngine
    {

    private:
    public:
        RenderEngine();
        ~RenderEngine() = default;

        static inline RenderEngine* Get()
        {
            return s_instance;
        }

        Vector<String> GetEngineShaderPaths();
        Vector<String> GetEnginePrimitivePaths();
        Vector<String> GetEngineTexturePaths();
        Mesh*          GetPlaceholderMesh();

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

        inline const DescriptorSetLayout& GetLayout(DescriptorSetType set)
        {
            return m_descriptorLayouts[set];
        }

        inline const PipelineLayout& GetGlobalAndPassLayouts()
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

        inline const Screen& GetScreen()
        {
            return m_screen;
        }

        inline const HashMap<Mesh*, MergedBufferMeshEntry>& GetMeshEntries()
        {
            return m_meshEntries;
        }

        inline const Buffer& GetGPUVertexBuffer()
        {
            return m_gpuVtxBuffer;
        }

        inline const Buffer& GetGPUIndexBuffer()
        {
            return m_gpuIndexBuffer;
        }

        inline uint32 GetFrameIndex()
        {
            return m_frameNumber % FRAMES_IN_FLIGHT;
        }

        inline const HashMap<StringID, SurfaceRenderer*>& GetChildWindowRenderers()
        {
            return m_childWindowRenderers;
        }

        void CreateChildWindow(const String& name, const Vector2i& pos, const Vector2i& size, SurfaceRenderer* associatedRenderer);
        void DestroyChildWindow(const String& name);
        void AddRenderer(Renderer* renderer);
        void DeleteRenderer(Renderer* renderer);

    private:
        friend class Engine;
        friend class World::LevelManager;

        void Initialize(const InitInfo& initInfo);
        void Tick();
        void SyncData();
        void Render();
        void Stop();
        void Shutdown();
        void Join();
        void MergeMeshes();

        void OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev);
        void OnPreMainLoop(const Event::EPreMainLoop& ev);
        void OnWindowPositioned(const Event::EWindowPositioned& ev);
        void OnWindowResized(const Event::EWindowResized& ev);
        void OnLevelUninstalled(const Event::ELevelUninstalled& ev);
        void OnLevelInstalled(const Event::ELevelInstalled& ev);
        void OnResourceLoaded(const Event::EResourceLoaded& res);

    private:
        static RenderEngine* s_instance;

        DeletionQueue                         m_mainDeletionQueue;
        WindowManager                         m_windowManager;
        InitInfo                              m_appInfo;
        Backend                               m_backend;
        Screen                                m_screen;
        Vector<SimpleAction>                  m_syncedActions;
        HashMap<Mesh*, MergedBufferMeshEntry> m_meshEntries;
        Vector<StringID>                      m_mergedModelIDs;
        Buffer                                m_cpuVtxBuffer;
        Buffer                                m_cpuIndexBuffer;
        Buffer                                m_gpuVtxBuffer;
        Buffer                                m_gpuIndexBuffer;
        bool                                  m_initedSuccessfully = false;
        bool                                  m_hasLevelLoaded     = false;
        Frame                                 m_frames[FRAMES_IN_FLIGHT];
        uint32                                m_frameNumber = 0;

        UploadContext                                   m_gpuUploader;
        HashMap<DescriptorSetType, DescriptorSetLayout> m_descriptorLayouts;
        PipelineLayout                                  m_globalAndPassLayout;
        GUIBackend*                                     m_guiBackend;
        Vector<Renderer*>                               m_renderers;
        Model*                                          m_placeholderModel       = nullptr;
        ModelNode*                                      m_placeholderModelNode   = nullptr;
        Material*                                       m_placeholderMaterial    = nullptr;
        WorldRenderer*                                  m_defaultWorldRenderer   = nullptr;
        SurfaceRenderer*                                m_defaultSurfaceRenderer = nullptr;

        // Resources
        HashMap<EngineShaderType, String>    m_engineShaderNames;
        HashMap<EnginePrimitiveType, String> m_enginePrimitiveNames;
        HashMap<EngineTextureType, String>   m_engineTextureNames;
        HashMap<EngineShaderType, Shader*>   m_engineShaders;
        HashMap<EngineShaderType, Material*> m_engineMaterials;
        HashMap<EnginePrimitiveType, Model*> m_engineModels;
        HashMap<EngineTextureType, Texture*> m_engineTextures;
        HashMap<StringID, SurfaceRenderer*>  m_childWindowRenderers;
    };
} // namespace Lina::Graphics

#endif
