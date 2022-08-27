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
#include "Data/Deque.hpp"
#include "Utility/StringId.hpp"
#include "Data/FixedVector.hpp"
#include "Core/CommonApplication.hpp"
#include "Utility/DeletionQueue.hpp"
#include "PipelineObjects/DescriptorSetLayout.hpp"
#include "PipelineObjects/UploadContext.hpp"
#include "Backend.hpp"
#include "Window.hpp"
#include <functional>

namespace Lina
{
    namespace Event
    {
        struct ESwapchainRecreated;
        struct EEngineResourcesLoaded;
    } // namespace Event
} // namespace Lina

namespace Lina::Graphics
{
    class Model;

    class RenderEngine
    {

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

        inline Renderer& GetLevelRenderer()
        {
            return m_levelRenderer;
        }

        inline DeletionQueue& GetMainDeletionQueue()
        {
            return m_mainDeletionQueue;
        }

        inline bool IsInitialized()
        {
            return m_initedSuccessfully;
        }

        inline Recti& GetScissor()
        {
            return m_scissor;
        }

        inline Viewport& GetViewport()
        {
            return m_viewport;
        }

        inline DescriptorSetLayout& GetLayout(uint32 setIndex)
        {
            return *m_descriptorLayouts[setIndex];
        }

        inline DescriptorPool& GetDescriptorPool()
        {
            return m_descriptorPool;
        }

        inline UploadContext& GetGPUUploader()
        {
            return m_gpuUploader;
        }

        void Join();

    private:
        void Initialize(const InitInfo& initInfo);
        void Clear();
        void Tick();
        void Render();
        void Shutdown();
        void GameSimCompleted();
        void OnSwapchainRecreated(const Event::ESwapchainRecreated& ev);
        void OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev);

    private:
        friend class Engine;

        static RenderEngine* s_instance;

        Model*        m_placeholderModel     = nullptr;
        ModelNode*    m_placeholderModelNode = nullptr;
        Material*     m_placeholderMaterial  = nullptr;
        DeletionQueue m_mainDeletionQueue;
        InitInfo      m_appInfo;
        Window        m_window;
        Backend       m_backend;
        bool          m_initedSuccessfully = false;

        UploadContext                         m_gpuUploader;
        DescriptorPool                        m_descriptorPool;
        DescriptorSetLayout                   m_globalSetLayout;
        DescriptorSetLayout                   m_objectDataLayout;
        DescriptorSetLayout                   m_textureDataLayout;
        HashMap<uint32, DescriptorSetLayout*> m_descriptorLayouts;
        Viewport                              m_viewport;
        Recti                                 m_scissor;
        Renderer                              m_levelRenderer;
    };
} // namespace Lina::Graphics

#endif
