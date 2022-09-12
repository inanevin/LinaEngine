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

#include "Core/Resource.hpp"
#include "Core/GraphicsCommon.hpp"
#include "PipelineObjects/Image.hpp"
#include "PipelineObjects/Sampler.hpp"

namespace Lina::Graphics
{
    class Texture : public Resources::Resource
    {
        struct AssetData
        {
            Format             format    = Format::R8G8B8A8_SRGB;
            Filter             minFilter = Filter::Nearest;
            Filter             magFilter = Filter::Nearest;
            SamplerAddressMode mode      = SamplerAddressMode::Repeat;

            // Runtime
            uint32 width    = 0;
            uint32 height   = 0;
            uint32 channels = 0;
        };

    public:
        Texture() = default;
        ~Texture();

        virtual Resource* LoadFromMemory(Serialization::Archive<IStream>& archive) override;
        virtual Resource* LoadFromFile(const String& path) override;
        virtual void      WriteToPackage(Serialization::Archive<OStream>& archive) override;

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
        void GenerateBuffers(unsigned char* pixels);

    private:
        AssetData      m_assetData;
        Image          m_gpuImage;
        Sampler        m_sampler;
        Extent3D       m_extent;
        Buffer         m_cpuBuffer;
        unsigned char* m_pixels = nullptr;
    };
} // namespace Lina::Graphics

#endif
