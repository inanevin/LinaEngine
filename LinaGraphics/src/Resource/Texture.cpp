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

#include "Resource/Texture.hpp"
#include "Core/RenderEngine.hpp"
#include "PipelineObjects/Buffer.hpp"
#include "PipelineObjects/UploadContext.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Utility/Command.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Utility/stb/stb_image.h"

#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Texture::~Texture()
    {
        m_gpuImage.Destroy();
        m_sampler.Destroy();
    }

    Resources::Resource* Texture::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        LoadFromArchive(archive);
        Sampler sampler = Sampler{
            .minFilter = m_assetData.minFilter,
            .magFilter = m_assetData.magFilter,
            .u         = m_assetData.mode,
            .v         = m_assetData.mode,
            .w         = m_assetData.mode,
        };
        GenerateCustomBuffers(m_assetData.width, m_assetData.height, 4, m_assetData.format, sampler, ImageTiling::Optimal);
        WriteToGPUImage(0, m_pixels, m_assetData.width * m_assetData.height * 4, Offset3D{.x = 0, .y = 0, .z = 0}, m_extent, true);
        return this;
    }

    Resources::Resource* Texture::LoadFromFile(const String& path)
    {
        LoadAssetData();

        if (m_pixels == nullptr)
        {
            int texWidth, texHeight, texChannels;
            m_pixels             = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            m_assetData.width    = static_cast<uint32>(texWidth);
            m_assetData.height   = static_cast<uint32>(texHeight);
            m_assetData.channels = static_cast<uint32>(texChannels);

            if (!m_pixels)
            {
                LINA_ERR("[Texture Loader]-> Could not load the texture from file! {0}", path);
                return nullptr;
            }

            SaveAssetData();
        }

        Sampler sampler = Sampler{
            .minFilter = m_assetData.minFilter,
            .magFilter = m_assetData.magFilter,
            .u         = m_assetData.mode,
            .v         = m_assetData.mode,
            .w         = m_assetData.mode,
        };
        GenerateCustomBuffers(m_assetData.width, m_assetData.height, 4, m_assetData.format, sampler, ImageTiling::Optimal);
        WriteToGPUImage(0, m_pixels, m_assetData.width * m_assetData.height * 4, Offset3D{.x = 0, .y = 0, .z = 0}, m_extent, true);
        return this;
    }

    void Texture::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        LoadAssetData();

        if (m_pixels == nullptr)
        {
            int texWidth, texHeight, texChannels;
            m_pixels             = stbi_load(m_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            m_assetData.width    = static_cast<uint32>(texWidth);
            m_assetData.height   = static_cast<uint32>(texHeight);
            m_assetData.channels = static_cast<uint32>(texChannels);
        }

        SaveToArchive(archive);
        stbi_image_free(m_pixels);
    }

    void Texture::SaveToArchive(Serialization::Archive<OStream>& archive)
    {
        const size_t size   = static_cast<size_t>(m_assetData.width * m_assetData.height * 4);
        const uint8  format = static_cast<uint8>(m_assetData.format);
        const uint8  min    = static_cast<uint8>(m_assetData.minFilter);
        const uint8  mag    = static_cast<uint8>(m_assetData.magFilter);
        const uint8  mode   = static_cast<uint8>(m_assetData.mode);
        archive(format, min, mag, mode);
        archive(m_assetData.width, m_assetData.height, m_assetData.channels);
        archive(size);

        if (size != 0)
            archive.GetStream().WriteEndianSafe(m_pixels, size);
    }

    void Texture::LoadFromArchive(Serialization::Archive<IStream>& archive)
    {
        uint8  format = 0, min = 0, mag = 0, mode = 0;
        uint32 w = 0, h = 0, chn = 0, pixelsSize = 0;
        archive(format, min, mag, mode);
        archive(w, h, chn, pixelsSize);

        m_assetData.format    = static_cast<Format>(format);
        m_assetData.minFilter = static_cast<Filter>(min);
        m_assetData.magFilter = static_cast<Filter>(mag);
        m_assetData.mode      = static_cast<SamplerAddressMode>(mode);
        m_assetData.width     = w;
        m_assetData.height    = h;
        m_assetData.channels  = chn;

        if (pixelsSize != 0)
        {
            m_pixels = new unsigned char[pixelsSize];
            archive.GetStream().ReadEndianSafe(m_pixels, pixelsSize);
        }
    }

    void Texture::CreateFromRuntime(Image img, Sampler sampler)
    {
        m_gpuImage = img;
        m_gpuImage.Create(true, false);
        m_sampler = sampler;
        m_sampler.Create(false);
    }

    void Texture::WriteToGPUImage(uint32 cpuBufferOffset, unsigned char* data, size_t dataSize, const Offset3D& gpuImgOffset, const Extent3D& copyExtent, bool destroyCPUBufferAfter)
    {
        if (data != nullptr)
            m_cpuBuffer.CopyIntoPadded(data, dataSize, cpuBufferOffset);

        Command cmd;
        cmd.Record = [this, copyExtent, gpuImgOffset, cpuBufferOffset](CommandBuffer& cmd) {
            ImageSubresourceRange range = ImageSubresourceRange{
                .aspectFlags    = GetImageAspectFlags(ImageAspectFlags::AspectColor),
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            };

            ImageMemoryBarrier imageBarrierToTransfer = ImageMemoryBarrier{
                .srcAccessMask    = 0,
                .dstAccessMask    = GetAccessFlags(AccessFlags::TransferWrite),
                .oldLayout        = ImageLayout::Undefined,
                .newLayout        = ImageLayout::TransferDstOptimal,
                .img              = m_gpuImage._allocatedImg.image,
                .subresourceRange = range,
            };

            Vector<ImageMemoryBarrier> imageBarriers;
            imageBarriers.push_back(imageBarrierToTransfer);

            cmd.CMD_PipelineBarrier(PipelineStageFlags::TopOfPipe, PipelineStageFlags::Transfer, 0, {}, {}, imageBarriers);

            ImageSubresourceRange copySubres = ImageSubresourceRange{
                .aspectFlags    = GetImageAspectFlags(ImageAspectFlags::AspectColor),
                .baseMipLevel   = 0,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            };

            BufferImageCopy copyRegion = BufferImageCopy{
                .bufferOffset      = cpuBufferOffset,
                .bufferRowLength   = 0,
                .bufferImageHeight = 0,
                .imageSubresource  = copySubres,
                .imageOffset       = gpuImgOffset,
                .imageExtent       = copyExtent,
            };

            // copy the buffer into the image
            cmd.CMD_CopyBufferToImage(m_cpuBuffer._ptr, m_gpuImage._allocatedImg.image, ImageLayout::TransferDstOptimal, {copyRegion});

            ImageMemoryBarrier barrierToReadable = imageBarrierToTransfer;
            barrierToReadable.oldLayout          = ImageLayout::TransferDstOptimal;
            barrierToReadable.newLayout          = ImageLayout::ShaderReadOnlyOptimal;
            barrierToReadable.srcAccessMask      = GetAccessFlags(AccessFlags::TransferWrite);
            barrierToReadable.dstAccessMask      = GetAccessFlags(AccessFlags::ShaderRead);

            cmd.CMD_PipelineBarrier(PipelineStageFlags::Transfer, PipelineStageFlags::FragmentShader, 0, {}, {}, {barrierToReadable});
        };

        cmd.OnSubmitted = [this, destroyCPUBufferAfter]() {
            m_gpuImage._ready = true;

            if (destroyCPUBufferAfter)
                m_cpuBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(cmd);
    }

    void Texture::GenerateCustomBuffers(int width, int height, int channels, Format format, Sampler sampler, ImageTiling imageTiling)
    {
        Extent3D ext = Extent3D{
            .width  = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .depth  = 1,
        };

        const uint32 bufferSize = ext.width * ext.height * channels;
        m_extent                = ext;

        // Cpu buf
        m_cpuBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        m_cpuBuffer.Create();

        ImageSubresourceRange range;
        range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);

        // Gpu buf
        m_gpuImage = Image{
            .format          = format,
            .tiling          = imageTiling,
            .extent          = ext,
            .imageUsageFlags = GetImageUsage(ImageUsageFlags::Sampled) | GetImageUsage(ImageUsageFlags::TransferDest),
            .subresRange     = range,
        };

        m_gpuImage.Create(true, false);

        // Sampler
        m_sampler = sampler;
        m_sampler.Create(false);
    }

} // namespace Lina::Graphics
