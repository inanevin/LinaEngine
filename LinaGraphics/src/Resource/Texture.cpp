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
#include "Core/ResourceDataManager.hpp"
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

    void* Texture::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        IResource::SetSID(path);
        LoadAssetData();
        int      texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load_from_memory(data, static_cast<int>(dataSize), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        m_width         = static_cast<uint32>(texWidth);
        m_height        = static_cast<uint32>(texHeight);
        m_channels      = static_cast<uint32>(texChannels);
        GenerateBuffers(pixels);
        return static_cast<void*>(this);
    }

    void* Texture::LoadFromFile(const String& path)
    {
        IResource::SetSID(path);
        LoadAssetData();

        int      texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        m_width         = static_cast<uint32>(texWidth);
        m_height        = static_cast<uint32>(texHeight);
        m_channels      = static_cast<uint32>(texChannels);

        if (!pixels)
        {
            LINA_ERR("[Texture Loader]-> Could not load the texture from file! {0}", path);
            return nullptr;
        }

        GenerateBuffers(pixels);
        return static_cast<void*>(this);
    }

    void Texture::LoadAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        if (!dm->Exists(m_sid))
            SaveAssetData();

        m_assetData.m_format    = static_cast<Format>(dm->GetValue<uint8>(m_sid, "format"));
        m_assetData.m_minFilter = static_cast<Filter>(dm->GetValue<uint8>(m_sid, "min"));
        m_assetData.m_magFilter = static_cast<Filter>(dm->GetValue<uint8>(m_sid, "mag"));
        m_assetData.m_mode      = static_cast<SamplerAddressMode>(dm->GetValue<uint8>(m_sid, "mode"));
    }

    void Texture::SaveAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        dm->CleanSlate(m_sid);
        dm->SetValue<uint8>(m_sid, "format", static_cast<uint8>(m_assetData.m_format));
        dm->SetValue<uint8>(m_sid, "min", static_cast<uint8>(m_assetData.m_minFilter));
        dm->SetValue<uint8>(m_sid, "mag", static_cast<uint8>(m_assetData.m_magFilter));
        dm->SetValue<uint8>(m_sid, "mode", static_cast<uint8>(m_assetData.m_mode));
        dm->Save();
    }

    void Texture::GenerateBuffers(unsigned char* pixels)
    {
        const size_t bufferSize = m_width * m_height * 4;

        m_cpuBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        m_cpuBuffer.Create();
        m_cpuBuffer.CopyInto(pixels, bufferSize);
        stbi_image_free(pixels);

        m_extent = Extent3D{
            .width  = m_width,
            .height = m_height,
            .depth  = 1};

        m_gpuImage = Image{
            .format          = m_assetData.m_format,
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
            LINA_TRACE("[Texture] -> Buffer transferred to VRAM. {0}", m_path);
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(cmd);

        m_sampler = Sampler{
            .minFilter = m_assetData.m_minFilter,
            .magFilter = m_assetData.m_magFilter,
            .u         = m_assetData.m_mode,
            .v         = m_assetData.m_mode,
            .w         = m_assetData.m_mode,
        };

        m_sampler.Create(false);
    }

} // namespace Lina::Graphics
