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

#include "Data/String.hpp"
#include "Graphics/Utility/DeletionQueue.hpp"
#include "Graphics/PipelineObjects/DescriptorSetLayout.hpp"
#include "Graphics/PipelineObjects/PipelineLayout.hpp"
#include "Graphics/PipelineObjects/UploadContext.hpp"
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

    class RenderEngine
    {

    public:
        RenderEngine()  = default;
        ~RenderEngine() = default;

        static inline RenderEngine* Get()
        {
            return s_instance;
        }

        // void CreateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size);
        // void UpdateAdditionalWindow(const String& nameID, const Vector2& pos, const Vector2& size);
        // void DestroyAdditionalWindow(const String& nameID);
        Vector<String> GetEngineShaderPaths();
        Vector<String> GetEngineMaterialPaths();
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

        inline Renderer* GetRenderer()
        {
            return m_renderer;
        }

        // inline HashMap<StringID, AdditionalWindow>& GetAdditionalWindows()
        // {
        //     return m_additionalWindows;
        // }

    private:
        friend class Engine;
        friend class World::LevelManager;

        void Initialize(const InitInfo& initInfo);
        void Tick();
        void SyncData();
        void Render();
        void Stop();
        void Shutdown();
        void OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev);
        void Join();
        void SetRenderer(Renderer* renderer);

    private:
        static RenderEngine* s_instance;

        Model*        m_placeholderModel     = nullptr;
        ModelNode*    m_placeholderModelNode = nullptr;
        Material*     m_placeholderMaterial  = nullptr;
        DeletionQueue m_mainDeletionQueue;
        InitInfo      m_appInfo;
        Backend       m_backend;
        Window*       m_window             = nullptr;
        bool          m_initedSuccessfully = false;

        UploadContext                                   m_gpuUploader;
        HashMap<DescriptorSetType, DescriptorSetLayout> m_descriptorLayouts;
        PipelineLayout                                  m_globalAndPassLayout;
        GUIBackend*                                     m_guiBackend;

        Renderer* m_renderer = nullptr;
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
