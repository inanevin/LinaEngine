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

#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/PipelineObjects/Buffer.hpp"
#include "Graphics/PipelineObjects/UploadContext.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Core/Command.hpp"
#include "Math/Math.hpp"
#include "Log/Log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "Graphics/Utility/stb/stb_image.h"
#include "Graphics/Utility/stb/stb_image_resize.h"

#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Texture::~Texture()
    {
        if (m_cpuBuffer.size != 0)
            m_cpuBuffer.Destroy();

        m_gpuImage.Destroy();
        m_sampler.Destroy();
    }

    Resources::Resource* Texture::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        const Sampler sampler = CreateDefaultSampler();
        LoadFromArchive(archive);
        GenerateCustomBuffers(m_assetData.width, m_assetData.height, m_assetData.channels, m_assetData.mipLevels, m_assetData.format, sampler, ImageTiling::Optimal);
        AddPixelsFromAssetData();
        WriteToGPUImage(m_assetData.mipmaps, Offset3D{.x = 0, .y = 0, .z = 0}, m_extent, true);
        return this;
    }

    Resources::Resource* Texture::LoadFromFile(const char* path)
    {
        LoadAssetData();

        if (m_assetData.pixels == nullptr)
        {
            int texWidth, texHeight, texChannels;
            m_assetData.pixels    = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            m_assetData.width     = static_cast<uint32>(texWidth);
            m_assetData.height    = static_cast<uint32>(texHeight);
            m_assetData.mipLevels = m_assetData.generateMipmaps ? static_cast<uint32>(Math::FloorLog2(Math::Max(texWidth, texHeight))) + 1 : 1;
            m_assetData.channels  = 4;
            CheckFormat(texChannels);
            GenerateMipmaps();

            LINA_TRACE("Loading {0}, dim {1}x{2}, channels {3}", m_path.c_str(), m_assetData.width, m_assetData.height, texChannels);

            if (!m_assetData.pixels)
            {
                LINA_ERR("[Texture Loader]-> Could not load the texture from file! {0}", path);
                return nullptr;
            }

            SaveAssetData();
        }

        const Sampler sampler = CreateDefaultSampler();
        GenerateCustomBuffers(m_assetData.width, m_assetData.height, m_assetData.channels, m_assetData.mipLevels, m_assetData.format, sampler, ImageTiling::Optimal);
        AddPixelsFromAssetData();
        WriteToGPUImage(m_assetData.mipmaps, Offset3D{.x = 0, .y = 0, .z = 0}, m_extent, true);
        return this;
    }

    void Texture::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        LoadAssetData();

        if (m_assetData.pixels == nullptr)
        {
            int texWidth, texHeight, texChannels;
            m_assetData.pixels    = stbi_load(m_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            m_assetData.width     = static_cast<uint32>(texWidth);
            m_assetData.height    = static_cast<uint32>(texHeight);
            m_assetData.mipLevels = m_assetData.generateMipmaps ? static_cast<uint32>(Math::FloorLog2(Math::Max(texWidth, texHeight))) + 1 : 1;
            m_assetData.channels  = 4;
            CheckFormat(texChannels);
            GenerateMipmaps();
        }

        SaveToArchive(archive);
        stbi_image_free(m_assetData.pixels);

        for (auto& mm : m_assetData.mipmaps)
        {
            if (mm.pixels != nullptr)
                stbi_image_free(mm.pixels);
        }
    }

    void Texture::SaveToArchive(Serialization::Archive<OStream>& archive)
    {
        const uint32 size         = static_cast<uint32>(m_assetData.width * m_assetData.height * m_assetData.channels);
        const uint8  format       = static_cast<uint8>(m_assetData.format);
        const uint8  min          = static_cast<uint8>(m_assetData.minFilter);
        const uint8  mag          = static_cast<uint8>(m_assetData.magFilter);
        const uint8  mode         = static_cast<uint8>(m_assetData.mode);
        const uint8  mipmapMode   = static_cast<uint8>(m_assetData.mipmapMode);
        const uint8  mipmapFilter = static_cast<uint8>(m_assetData.mipmapFilter);

        archive(m_assetData.width, m_assetData.height, m_assetData.mipLevels, m_assetData.generateMipmaps, m_assetData.channels);
        archive(m_assetData.anisotropyEnabled, m_assetData.anisotropy, m_assetData.isInLinearSpace);
        archive(size, format, min, mag, mode, mipmapMode, mipmapFilter);

        if (size != 0)
            archive.GetStream().WriteEndianSafe(m_assetData.pixels, size);

        const uint32 mipCount = static_cast<uint32>(m_assetData.mipmaps.size());
        archive(mipCount);

        for (uint32 i = 0; i < mipCount; i++)
        {
            const auto& mm = m_assetData.mipmaps[i];
            archive(mm.width);
            archive(mm.height);

            const uint32 pixelsSize = static_cast<uint32>(mm.width * mm.height * m_assetData.channels);
            archive(pixelsSize);

            if (pixelsSize != 0)
                archive.GetStream().WriteEndianSafe(mm.pixels, pixelsSize);
        }
    }

    void Texture::LoadFromArchive(Serialization::Archive<IStream>& archive)
    {
        uint32 pixelsSize = 0;
        uint8  format = 0, min = 0, mag = 0, mode = 0, mipmapMode = 0, mipmapFilter = 0;
        archive(m_assetData.width, m_assetData.height, m_assetData.mipLevels, m_assetData.generateMipmaps, m_assetData.channels);
        archive(m_assetData.anisotropyEnabled, m_assetData.anisotropy, m_assetData.isInLinearSpace);
        archive(pixelsSize, format, min, mag, mode, mipmapMode, mipmapFilter);

        m_assetData.format       = static_cast<Format>(format);
        m_assetData.minFilter    = static_cast<Filter>(min);
        m_assetData.magFilter    = static_cast<Filter>(mag);
        m_assetData.mode         = static_cast<SamplerAddressMode>(mode);
        m_assetData.mipmapMode   = static_cast<MipmapMode>(mipmapMode);
        m_assetData.mipmapFilter = static_cast<MipmapFilter>(mipmapFilter);

        if (pixelsSize != 0)
        {
            m_assetData.pixels = new unsigned char[pixelsSize];
            archive.GetStream().ReadEndianSafe(m_assetData.pixels, pixelsSize);
        }

        uint32 mipCount = 0;
        archive(mipCount);

        for (uint32 i = 0; i < mipCount; i++)
        {
            Mipmap mm = {};

            archive(mm.width);
            archive(mm.height);

            uint32 mmPixelsSize = 0;
            archive(mmPixelsSize);

            if (mmPixelsSize != 0)
            {
                mm.pixels = new unsigned char[mmPixelsSize];
                archive.GetStream().ReadEndianSafe(mm.pixels, mmPixelsSize);
            }

            m_assetData.mipmaps.push_back(mm);
        }
    }

    void Texture::AddPixelsFromAssetData()
    {
        m_cpuBuffer.CopyIntoPadded(m_assetData.pixels, m_assetData.width * m_assetData.height * m_assetData.channels, 0);
        uint32 offset = m_assetData.width * m_assetData.height * m_assetData.channels;
        for (auto& mp : m_assetData.mipmaps)
        {
            const uint32 size = mp.width * mp.height * m_assetData.channels;
            m_cpuBuffer.CopyIntoPadded(mp.pixels, size, offset);
            offset += size;
        }
    }

    void Texture::CheckFormat(int channels)
    {
        if (channels == 2)
            m_assetData.format = Format::R8G8_UNORM;
        else if (channels == 1)
            m_assetData.format = Format::R8_UNORM;
        else
            m_assetData.format = m_assetData.isInLinearSpace ? Format::R8G8B8A8_UNORM : Format::R8G8B8A8_SRGB;
    }

    void Texture::GenerateMipmaps()
    {
        if (!m_assetData.generateMipmaps || m_assetData.mipLevels == 1)
            return;

        if (!m_assetData.mipmaps.empty())
            return;

        int width  = static_cast<int>(m_assetData.width);
        int height = static_cast<int>(m_assetData.height);

        Vector<Mipmap> mipmaps;
        mipmaps.resize(m_assetData.mipLevels - 1);

        unsigned char* lastPixels = m_assetData.pixels;
        uint32         lastWidth  = m_assetData.width;
        uint32         lastHeight = m_assetData.height;

        for (uint32 i = 0; i < m_assetData.mipLevels - 1; i++)
        {
            uint32 width  = lastWidth / 2;
            uint32 height = lastHeight / 2;

            if (width < 1)
                width = 1;

            if (height < 1)
                height = 1;

            Mipmap mipmap;
            mipmap.width  = width;
            mipmap.height = height;
            mipmap.pixels = new unsigned char[width * height * m_assetData.channels];
            // stbir_resize_uint8(m_assetData.pixels, m_assetData.width, m_assetData.height, 0, mipmap.pixels, width, height, 0, m_assetData.channels);
            const stbir_colorspace cs = m_assetData.isInLinearSpace ? stbir_colorspace::STBIR_COLORSPACE_LINEAR : stbir_colorspace::STBIR_COLORSPACE_SRGB;
            stbir_resize_uint8_generic(lastPixels, lastWidth, lastHeight, 0, mipmap.pixels, width, height, 0, m_assetData.channels, STBIR_ALPHA_CHANNEL_NONE, 0, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(m_assetData.mipmapFilter), cs, 0);
            lastWidth  = width;
            lastHeight = height;
            lastPixels = mipmap.pixels;
            mipmaps[i] = mipmap;
        }

        m_assetData.mipmaps = mipmaps;
    }

    void Texture::CopyImage(uint32 cpuOffset, const CommandBuffer& cmd, const Offset3D& gpuImgOffset, const Extent3D& copyExtent, uint32 totalMipLevels, uint32 baseMipLevel)
    {
        ImageSubresourceRange copySubres = ImageSubresourceRange{
            .aspectFlags    = GetImageAspectFlags(ImageAspectFlags::AspectColor),
            .baseMipLevel   = baseMipLevel,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };

        BufferImageCopy copyRegion = BufferImageCopy{
            .bufferOffset      = cpuOffset,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource  = copySubres,
            .imageOffset       = gpuImgOffset,
            .imageExtent       = copyExtent,
        };

        // Copy the buffer into the image
        cmd.CMD_CopyBufferToImage(m_cpuBuffer._ptr, m_gpuImage._allocatedImg.image, ImageLayout::TransferDstOptimal, {copyRegion});

        // Transfer to shader read.
        cmd.CMD_ImageTransition(m_gpuImage._allocatedImg.image, ImageLayout::TransferDstOptimal, ImageLayout::ShaderReadOnlyOptimal, ImageAspectFlags::AspectColor, AccessFlags::TransferWrite, AccessFlags::ShaderRead, PipelineStageFlags::Transfer,
                                PipelineStageFlags::FragmentShader, totalMipLevels, baseMipLevel);
    }

    Sampler Texture::CreateDefaultSampler()
    {
        Sampler sampler = Sampler{
            .minFilter         = m_assetData.minFilter,
            .magFilter         = m_assetData.magFilter,
            .u                 = m_assetData.mode,
            .v                 = m_assetData.mode,
            .w                 = m_assetData.mode,
            .minLod            = 0.0f,
            .maxLod            = m_assetData.generateMipmaps ? static_cast<float>(m_assetData.mipLevels) : 0.0f,
            .anisotropyEnabled = m_assetData.anisotropyEnabled,
            .maxAnisotropy     = m_assetData.anisotropy,
            .mipmapMode        = m_assetData.mipmapMode,
        };

        return sampler;
    }

    void Texture::CreateFromRuntime(const Image& img, const Sampler& sampler, const Extent3D& ext)
    {
        m_extent   = ext;
        m_gpuImage = img;
        m_gpuImage.Create(true, false);
        m_sampler = sampler;
        m_sampler.Create(false);
    }

    void Texture::WriteToGPUImage(const Offset3D& gpuImgOffset, const Extent3D& copyExtent, bool destroyCPUBufferAfter)
    {
        Command cmd;
        cmd.Record = [this, copyExtent, gpuImgOffset](CommandBuffer& cmd) {
            // Transfer to destination optimal.
            cmd.CMD_ImageTransition(m_gpuImage._allocatedImg.image, ImageLayout::Undefined, ImageLayout::TransferDstOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::TransferWrite, PipelineStageFlags::TopOfPipe,
                                    PipelineStageFlags::Transfer, 1, 0);

            // Copy and transfer to shader read.
            CopyImage(0, cmd, gpuImgOffset, copyExtent, 1, 0);
        };

        cmd.OnSubmitted = [this, destroyCPUBufferAfter]() {
            m_gpuImage._ready = true;

            if (destroyCPUBufferAfter)
                m_cpuBuffer.Destroy();

            if (m_assetData.pixels != nullptr)
                stbi_image_free(m_assetData.pixels);

            for (auto& mm : m_assetData.mipmaps)
            {
                if (mm.pixels != nullptr)
                    stbi_image_free(mm.pixels);
            }
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(cmd);
    }

    void Texture::WriteToGPUImage(const Vector<Mipmap>& mipmaps, const Offset3D& gpuImgOffset, const Extent3D& copyExtent, bool destroyCPUBufferAfter)
    {
        Command cmd;
        cmd.Record = [this, copyExtent, gpuImgOffset, mipmaps](CommandBuffer& cmd) {
            // Transfer to destination optimal.
            cmd.CMD_ImageTransition(m_gpuImage._allocatedImg.image, ImageLayout::Undefined, ImageLayout::TransferDstOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::TransferWrite, PipelineStageFlags::TopOfPipe,
                                    PipelineStageFlags::Transfer, m_assetData.mipLevels, 0);

            // Copy and transfer to shader read.
            CopyImage(0, cmd, gpuImgOffset, copyExtent, m_assetData.mipLevels, 0);

            uint32 mipLevel = 1;
            uint32 offset   = m_assetData.width * m_assetData.height * m_assetData.channels;
            for (auto& mm : mipmaps)
            {
                // Transfer to destination optimal.
                cmd.CMD_ImageTransition(m_gpuImage._allocatedImg.image, ImageLayout::Undefined, ImageLayout::TransferDstOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::TransferWrite, PipelineStageFlags::TopOfPipe,
                                        PipelineStageFlags::Transfer, m_assetData.mipLevels, mipLevel);

                const Extent3D mipExtent = Extent3D{mm.width, mm.height, 1};
                // Copy and transfer to shader read.
                CopyImage(offset, cmd, gpuImgOffset, mipExtent, m_assetData.mipLevels, mipLevel);

                offset += mm.width * mm.height * m_assetData.channels;
                mipLevel++;
            }
        };

        cmd.OnSubmitted = [this, destroyCPUBufferAfter]() {
            m_gpuImage._ready = true;

            if (destroyCPUBufferAfter)
                m_cpuBuffer.Destroy();

            if (m_assetData.pixels != nullptr)
                stbi_image_free(m_assetData.pixels);

            for (auto& mm : m_assetData.mipmaps)
            {
                if (mm.pixels != nullptr)
                    stbi_image_free(mm.pixels);
            }
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(cmd);
    }

    void Texture::GenerateCustomBuffers(int width, int height, int channels, uint32 mipLevels, Format format, Sampler sampler, ImageTiling imageTiling)
    {
        Extent3D ext = Extent3D{
            .width  = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .depth  = 1,
        };

        uint32 bufferSize = ext.width * ext.height * channels;
        m_extent          = ext;

        // Add all mipmap buffers
        for (auto& mp : m_assetData.mipmaps)
            bufferSize += mp.width * mp.height * channels;

        // Cpu buf
        m_cpuBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        m_cpuBuffer.Create();

        ImageSubresourceRange range;
        range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);
        range.levelCount  = mipLevels;

        // Gpu buf
        m_gpuImage = Image{
            .format          = format,
            .tiling          = imageTiling,
            .extent          = ext,
            .imageUsageFlags = GetImageUsage(ImageUsageFlags::Sampled) | GetImageUsage(ImageUsageFlags::TransferDest),
            .subresRange     = range,
            .mipLevels       = mipLevels,
        };

        m_gpuImage.Create(true, false);

        // Sampler
        m_sampler = sampler;
        m_sampler.Create(false);
    }

} // namespace Lina::Graphics
