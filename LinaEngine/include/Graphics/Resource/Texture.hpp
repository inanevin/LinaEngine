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

#ifndef Texture_HPP
#define Texture_HPP

#include "Resource/Core/Resource.hpp"
#include "Graphics/Core/GraphicsCommon.hpp"
#include "Graphics/PipelineObjects/Image.hpp"
#include "Graphics/PipelineObjects/Sampler.hpp"
#include "Graphics/PipelineObjects/Buffer.hpp"

namespace Lina::Graphics
{
    class CommandBuffer;

    class Texture : public Resources::Resource
    {

    public:
        struct Mipmap
        {
            uint32         width  = 0;
            uint32         height = 0;
            unsigned char* pixels = nullptr;
        };

    private:
#define VERSION 2

        struct AssetData
        {
            Format             format            = Format::R8G8B8A8_SRGB;
            Filter             minFilter         = Filter::Nearest;
            Filter             magFilter         = Filter::Nearest;
            SamplerAddressMode mode              = SamplerAddressMode::Repeat;
            MipmapFilter       mipmapFilter      = MipmapFilter::Mitchell;
            MipmapMode         mipmapMode        = MipmapMode::Linear;
            bool               anisotropyEnabled = true;
            float              anisotropy        = 2.0f;
            bool               generateMipmaps   = true;
            bool               isInLinearSpace   = false;

            // Runtime
            uint32         width          = 0;
            uint32         height         = 0;
            uint32         mipLevels      = 0;
            uint32         channels       = 0;
            unsigned char* pixels         = nullptr;
            Vector<Mipmap> mipmaps;
        };

    public:
        Texture() = default;
        ~Texture();

        virtual uint32 GetVersion() override
        {
            return VERSION;
        }

        virtual Resource* LoadFromMemory(Serialization::Archive<IStream>& archive) override;
        virtual Resource* LoadFromFile(const char* path) override;
        virtual void      WriteToPackage(Serialization::Archive<OStream>& archive) override;
        void              CreateFromRuntime(const Image& img, const Sampler& sampler, const Extent3D& ext);
        void              WriteToGPUImage(const Offset3D& gpuImgOffset, const Extent3D& copyExtent, bool destroyCPUBufferAfter);
        void              WriteToGPUImage(const Vector<Mipmap>& mipmaps, const Offset3D& gpuImgOffset, const Extent3D& copyExtent, bool destroyCPUBufferAfter);
        void              GenerateCustomBuffers(int width, int height, int channels, uint32 mipLevels, Format format = Format::R8G8B8A8_SRGB, Sampler sampler = Sampler(), ImageTiling = ImageTiling::Linear);

        inline const Extent3D& GetExtent() const
        {
            return m_extent;
        }

        inline Buffer& GetCPUBuffer()
        {
            return m_cpuBuffer;
        }

        inline Image& GetImage()
        {
            return m_gpuImage;
        }

        inline Sampler& GetSampler()
        {
            return m_sampler;
        }

    protected:
        virtual void SaveToArchive(Serialization::Archive<OStream>& archive) override;
        virtual void LoadFromArchive(Serialization::Archive<IStream>& archive) override;

    private:
        void    AddPixelsFromAssetData();
        void    CheckFormat(int channels);
        void    GenerateMipmaps();
        void    CopyImage(uint32 cpuOffset, const CommandBuffer& cmd, const Offset3D& gpuImgOffset, const Extent3D& copyExtent, uint32 totalMipLevels, uint32 baseMipLevel);
        Sampler CreateDefaultSampler();

    private:
        friend class GUIBackend;

        AssetData m_assetData;
        Image     m_gpuImage;
        Sampler   m_sampler;
        Extent3D  m_extent;
        Buffer    m_cpuBuffer;
    };
} // namespace Lina::Graphics

#endif
