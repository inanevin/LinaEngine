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

struct VkShaderModule_T;

namespace Lina::Graphics
{
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

        bool              HasStage(ShaderStage stage);
        VkShaderModule_T* GetModule(ShaderStage stage);

        inline SurfaceType GetSurfaceType()
        {
            return m_surface;
        }

        /// <summary>
        /// Returns a shader block from a full linashader text.
        /// </summary>
        /// <param name="shader">Full text.</param>
        /// <param name="defineStart">Define string, e.g. #LINA_VS or #LINA_FRAG </param>
        /// <returns></returns>
        static String GetShaderStageText(const String& shader, const String& defineStart);

    private:
        void GenerateByteCode();
        bool CreateShaderModules();

    private:
        struct AssetData
        {
            bool                 geoShader = false;
            Vector<unsigned int> vtxData;
            Vector<unsigned int> fragData;
            Vector<unsigned int> geoData;
        };

    private:
        SurfaceType m_surface = SurfaceType::Opaque;
        AssetData   m_assetData;
        String      m_text       = "";
        String      m_vertexText = "";
        String      m_fragText   = "";
        String      m_geoText    = "";

    private:
        VkShaderModule_T* _ptrVtx  = nullptr;
        VkShaderModule_T* _ptrFrag = nullptr;
        VkShaderModule_T* _ptrGeo  = nullptr;
    };

} // namespace Lina::Graphics

#endif
