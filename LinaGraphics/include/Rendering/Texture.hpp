/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: Texture

Represents a texture creatd in the GPU backend, is responsible for delegating the creation of
all different types of textures, 2D, 3D, cubemap, HDRI etc.

Timestamp: 1/7/2019 1:55:47 PM
*/

#pragma once

#ifndef Texture_HPP
#define Texture_HPP

#include "Core/RenderBackendFwd.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ImageAssetData.hpp"

namespace Lina::Graphics
{
    class ArrayBitmap;
    class DDSTexture;

    class Texture : public Resources::IResource
    {

    public:
        Texture() = default;
        ~Texture();

        virtual void* LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const std::string& path) override;
        void*         LoadFromFileHDRI(const std::string& path);
        void*         LoadFromMemoryHDRI(const std::string& path, unsigned char* data, size_t dataSize);
        void          WriteToFile(const std::string& path);

        void Construct(SamplerParameters samplerParams, bool shouldCompress, const std::string& path = "");
        void ConstructCubemap(SamplerParameters samplerParams, const std::vector<class ArrayBitmap*>& data, bool compress, const std::string& path = "");
        void ConstructHDRI(SamplerParameters samplerParams, const Vector2i& size, float* data, const std::string& path = "");
        void ConstructRTCubemapTexture(Vector2i size, SamplerParameters samplerParams, const std::string& path = "");
        void ConstructRTTexture(Vector2i size, SamplerParameters samplerParams, bool useBorder = false, const std::string& path = "");
        void ConstructRTTextureMSAA(Vector2i size, SamplerParameters samplerParams, int sampleCount, const std::string& path = "");
        void ConstructEmpty(SamplerParameters samplerParams = SamplerParameters(), const std::string& path = "");

        inline ImageAssetData* GetAssetData()
        {
            return m_assetData;
        }
        inline uint32 GetID() const
        {
            return m_id;
        };
        inline uint32 GetSamplerID() const
        {
            return m_sampler.GetID();
        }
        inline Sampler& GetSampler()
        {
            return m_sampler;
        }
        inline bool IsCompressed() const
        {
            return m_isCompressed;
        }
        inline bool HasMipmaps() const
        {
            return m_hasMipMaps;
        }
        inline Vector2i GetSize()
        {
            return m_size;
        }
        inline bool GetIsEmpty()
        {
            return m_isEmpty;
        }

    private:
        friend RenderEngine;

        TextureBindMode m_bindMode;
        Sampler         m_sampler;
        ImageAssetData* m_assetData     = nullptr;
        RenderDevice*   m_renderDevice  = nullptr;
        uint32          m_id            = 0;
        Vector2i        m_size          = Vector2::One;
        bool            m_isCompressed  = false;
        bool            m_hasMipMaps    = true;
        bool            m_isEmpty       = true;
        int             m_numComponents = 0;
        ArrayBitmap*    m_bitmap        = nullptr;
    };
} // namespace Lina::Graphics

#endif
