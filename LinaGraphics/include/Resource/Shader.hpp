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

#ifndef Shader_HPP
#define Shader_HPP

#include "Core/IResource.hpp"
#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Core/GraphicsCommon.hpp"

#include "PipelineObjects/Pipeline.hpp"
#include "PipelineObjects/PipelineLayout.hpp"
#include "PipelineObjects/DescriptorSetLayout.hpp"

struct VkShaderModule_T;

namespace Lina::Graphics
{
    struct ShaderModule
    {
        String               moduleName = "";
        VkShaderModule_T*    ptr        = nullptr;
        String               moduleText = "";
        Vector<unsigned int> byteCode;
    };

    class Shader : public Resources::IResource
    {
    public:
        Shader() = default;
        virtual ~Shader();

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;
        virtual void  LoadAssetData() override;
        virtual void  SaveAssetData() override;

        void UploadedToPipeline();

        inline const HashMap<ShaderStage, ShaderModule>& GetModules()
        {
            return m_modules;
        }

        inline SurfaceType GetSurfaceType()
        {
            return m_surface;
        }

        inline Pipeline& GetPipeline()
        {
            return m_pipeline;
        }

    private:
        void CheckIfModuleExists(const String& name, ShaderStage stage, const String& define);
        void GenerateByteCode();
        bool CreateShaderModules();
        void GeneratePipeline();

    private:
        struct AssetData
        {
        };

    private:
        Pipeline                    m_pipeline;
        PipelineLayout              m_pipelineLayout;
        SurfaceType                 m_surface = SurfaceType::Opaque;
        AssetData                   m_assetData;
        String                      m_text       = "";
        String                      m_vertexText = "";
        String                      m_fragText   = "";
        String                      m_geoText    = "";
        Vector<DescriptorSetLayout> m_setLayouts;

    private:
        HashMap<ShaderStage, ShaderModule> m_modules;
    };

} // namespace Lina::Graphics

#endif
