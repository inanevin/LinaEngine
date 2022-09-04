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
#include "Utility/stb/stb_image.h"
#include "Utility/Command.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Texture::~Texture()
    {
        m_gpuImage.Destroy();
        m_sampler.Destroy();
    }

    Resources::Resource* Texture::LoadFromMemory(const IStream& stream)
    {
        // IResource::SetSID(path);
        // LoadAssetData();
        // int      texWidth, texHeight, texChannels;
        // stbi_uc* pixels = stbi_load_from_memory(data, static_cast<int>(dataSize), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        // m_width         = static_cast<uint32>(texWidth);
        // m_height        = static_cast<uint32>(texHeight);
        // m_channels      = static_cast<uint32>(texChannels);
        // GenerateBuffers(pixels);
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

        GenerateBuffers(m_pixels);

        return this;
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

    void Texture::GenerateBuffers(unsigned char* pixels)
    {
        const size_t bufferSize = m_assetData.width * m_assetData.height * 4;

        m_cpuBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        m_cpuBuffer.Create();
        m_cpuBuffer.CopyInto(pixels, bufferSize);
        stbi_image_free(m_pixels);

        m_extent = Extent3D{
            .width  = m_assetData.width,
            .height = m_assetData.height,
            .depth  = 1};

        m_gpuImage = Image{
            .format          = m_assetData.format,
            .tiling          = ImageTiling::Optimal,
            .extent          = m_extent,
            .aspectFlags     = GetImageAspectFlags(ImageAspectFlags::AspectColor),
            .imageUsageFlags = GetImageUsage(ImageUsageFlags::Sampled) | GetImageUsage(ImageUsageFlags::TransferDest),
        };

        m_gpuImage.Create(true, false);

        Command cmd;
        cmd.Record = [this](CommandBuffer& cmd) {
            ImageSubresourceRange range = ImageSubresourceRange{
                .aspectMask     = ImageAspectFlags::AspectColor,
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

            ImageSubresourceLayers copySubres = ImageSubresourceLayers{
                .aspectMask     = ImageAspectFlags::AspectColor,
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            };

            BufferImageCopy copyRegion = BufferImageCopy{
                .bufferOffset      = 0,
                .bufferRowLength   = 0,
                .bufferImageHeight = 0,
                .imageSubresource  = copySubres,
                .imageExtent       = m_extent,
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

        cmd.OnSubmitted = [this]() {
            m_gpuImage._ready = true;
            m_cpuBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(cmd);

        m_sampler = Sampler{
            .minFilter = m_assetData.minFilter,
            .magFilter = m_assetData.magFilter,
            .u         = m_assetData.mode,
            .v         = m_assetData.mode,
            .w         = m_assetData.mode,
        };

        m_sampler.Create(false);
    }

} // namespace Lina::Graphics
