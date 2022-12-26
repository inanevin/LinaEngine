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

#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/PipelineObjects/RenderPass.hpp"
#include "Graphics/PipelineObjects/Framebuffer.hpp"
#include "Graphics/Resource/Texture.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

namespace Lina::Graphics
{
    VkAttachmentDescription VulkanUtility::CreateAttachmentDescription(const Attachment& att)
    {
        return VkAttachmentDescription{
            .flags          = att.flags,
            .format         = GetFormat(att.format),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = GetLoadOp(att.loadOp),
            .storeOp        = GetStoreOp(att.storeOp),
            .stencilLoadOp  = GetLoadOp(att.stencilLoadOp),
            .stencilStoreOp = GetStoreOp(att.stencilStoreOp),
            .initialLayout  = GetImageLayout(att.initialLayout),
            .finalLayout    = GetImageLayout(att.finalLayout),
        };
    }
    VkPipelineShaderStageCreateInfo VulkanUtility::CreatePipelineShaderStageCreateInfo(ShaderStage stage, VkShaderModule shaderModule)
    {
        VkPipelineShaderStageCreateInfo info = VkPipelineShaderStageCreateInfo{
            .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext  = nullptr,
            .stage  = static_cast<VkShaderStageFlagBits>(GetShaderStage(stage)),
            .module = shaderModule,
            .pName  = "main",
        };

        return info;
    }
    VkPipelineVertexInputStateCreateInfo VulkanUtility::CreatePipelineVertexInputStateCreateInfo(const Vector<VkVertexInputBindingDescription>& bindingDescs, const Vector<VkVertexInputAttributeDescription>& attDescs)
    {
        VkPipelineVertexInputStateCreateInfo info = VkPipelineVertexInputStateCreateInfo{
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext                           = nullptr,
            .vertexBindingDescriptionCount   = static_cast<uint32>(bindingDescs.size()),
            .pVertexBindingDescriptions      = bindingDescs.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32>(attDescs.size()),
            .pVertexAttributeDescriptions    = attDescs.data(),
        };

        return info;
    }
    VkPipelineInputAssemblyStateCreateInfo VulkanUtility::CreatePipelineInputAssemblyCreateInfo(Topology top, bool primitiveRestart)
    {
        VkPipelineInputAssemblyStateCreateInfo info = VkPipelineInputAssemblyStateCreateInfo{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext                  = nullptr,
            .topology               = GetTopology(top),
            .primitiveRestartEnable = primitiveRestart ? VK_TRUE : VK_FALSE,
        };

        return info;
    }

    VkPipelineRasterizationStateCreateInfo VulkanUtility::CreatePipelineRasterStateCreateInfo(PolygonMode pm, CullMode cm, FrontFace frontFace)
    {
        VkPipelineRasterizationStateCreateInfo info = VkPipelineRasterizationStateCreateInfo{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext                   = nullptr,
            .depthClampEnable        = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode             = GetPolygonMode(pm),
            .cullMode                = GetCullMode(cm),
            .frontFace               = GetFrontFace(frontFace),
            .depthBiasEnable         = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp          = 0.0f,
            .depthBiasSlopeFactor    = 0.0f,
            .lineWidth               = 1.0f,
        };
        return info;
    }
    VkPipelineMultisampleStateCreateInfo VulkanUtility::CreatePipelineMSAACreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo info = VkPipelineMultisampleStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            // multisampling defaulted to no multisampling (1 sample per pixel)
            .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable   = VK_FALSE,
            .minSampleShading      = 1.0f,
            .pSampleMask           = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE,

        };
        return info;
    }

    VkPipelineColorBlendAttachmentState VulkanUtility::CreatePipelineBlendAttachmentState(ColorBlendAttachmentState blendAtt)
    {
        VkPipelineColorBlendAttachmentState info = VkPipelineColorBlendAttachmentState{
            .blendEnable         = blendAtt.blendEnable ? VK_TRUE : VK_FALSE,
            .srcColorBlendFactor = GetBlendFactor(blendAtt.srcColorBlendFactor),
            .dstColorBlendFactor = GetBlendFactor(blendAtt.dstColorBlendFactor),
            .colorBlendOp        = GetBlendOp(blendAtt.colorBlendOp),
            .srcAlphaBlendFactor = GetBlendFactor(blendAtt.srcAlphaBlendFactor),
            .dstAlphaBlendFactor = GetBlendFactor(blendAtt.dstAlphaBlendFactor),
            .alphaBlendOp        = GetBlendOp(blendAtt.alphaBlendOp),
            .colorWriteMask      = GetColorComponentFlags(blendAtt.componentFlags),
        };
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo VulkanUtility::CreatePipelineDepthStencilStateCreateInfo(bool depthTest, bool depthWrite, CompareOp op)
    {
        VkPipelineDepthStencilStateCreateInfo info = {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext                 = nullptr,
            .depthTestEnable       = depthTest ? VK_TRUE : VK_FALSE,
            .depthWriteEnable      = depthWrite ? VK_TRUE : VK_FALSE,
            .depthCompareOp        = depthTest ? GetCompareOp(op) : VK_COMPARE_OP_ALWAYS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable     = VK_FALSE,
            .minDepthBounds        = 0.0f, // Optional
            .maxDepthBounds        = 1.0f, // Optional
        };

        return info;
    }

    void VulkanUtility::GetDefaultPassSampler(Sampler& sampler)
    {
        sampler = Sampler{
            .minFilter     = Filter::Linear,
            .magFilter     = Filter::Linear,
            .u             = SamplerAddressMode::ClampToEdge,
            .v             = SamplerAddressMode::ClampToEdge,
            .w             = SamplerAddressMode::ClampToEdge,
            .mipLodBias    = 0.0f,
            .maxAnisotropy = 1.0f,
            .minLod        = 0.0f,
            .maxLod        = 1.0f,
            .borderColor   = BorderColor::FloatOpaqueWhite,
        };
    }

    Texture* VulkanUtility::CreateDefaultPassTextureColor(bool useExplicitSize, int width, int height)
    {
        const Vector2i size = Backend::Get()->GetMainSwapchain().size;
        Extent3D       ext  = Extent3D{.depth = 1};
        ext.width           = useExplicitSize ? width : static_cast<unsigned int>(size.x);
        ext.height          = useExplicitSize ? height : static_cast<unsigned int>(size.y);

        ImageSubresourceRange range;
        range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);

        Image image = Image{
            .format              = Backend::Get()->GetMainSwapchain().format,
            .tiling              = ImageTiling::Optimal,
            .extent              = ext,
            .imageUsageFlags     = GetImageUsage(ImageUsageFlags::ColorAttachment) | GetImageUsage(ImageUsageFlags::Sampled),
            .memoryUsageFlags    = MemoryUsageFlags::GpuOnly,
            .memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
            .subresRange         = range,
        };

        // Color texture
        Texture* txt = new Texture();
        Sampler  sampler;
        GetDefaultPassSampler(sampler);
        txt->CreateFromRuntime(image, sampler);
        return txt;
    }

    Texture* VulkanUtility::CreateDefaultPassTextureDepth(bool useExplicitSize, int width, int height)
    {
        const Vector2i size = Backend::Get()->GetMainSwapchain().size;
        Extent3D       ext  = Extent3D{.depth = 1};
        ext.width           = useExplicitSize ? width : static_cast<unsigned int>(size.x);
        ext.height          = useExplicitSize ? height : static_cast<unsigned int>(size.y);

        ImageSubresourceRange range;
        range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectDepth); // | GetImageAspectFlags(ImageAspectFlags::AspectStencil);

        Image image = Image{
            .format              = Format::D32_SFLOAT,
            .tiling              = ImageTiling::Optimal,
            .extent              = ext,
            .imageUsageFlags     = GetImageUsage(ImageUsageFlags::DepthStencilAttachment),
            .memoryUsageFlags    = MemoryUsageFlags::GpuOnly,
            .memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
            .subresRange         = range,
        };

        // Texture
        Texture* txt = new Texture();
        Sampler  sampler;
        GetDefaultPassSampler(sampler);
        txt->CreateFromRuntime(image, sampler);
        return txt;
    }

    void VulkanUtility::SetupAndCreateMainRenderPass(RenderPass& pass)
    {
        const Vector2i size = Backend::Get()->GetMainSwapchain().size;
        Extent3D       ext  = Extent3D{.width = static_cast<unsigned int>(size.x), .height = static_cast<unsigned int>(size.y), .depth = 1};

        ClearValue clrCol = ClearValue{
            .clearColor = Color(0.1f, 0.1f, 0.1f, 1.0f),
            .isColor    = true,
        };

        ClearValue clrDepth = ClearValue{
            .isColor = false,
            .depth   = 1.0f,
        };

        Attachment colorAttachment = Attachment{
            .format         = Backend::Get()->GetMainSwapchain().format,
            .loadOp         = LoadOp::Clear,
            .storeOp        = StoreOp::Store,
            .stencilLoadOp  = LoadOp::DontCare,
            .stencilStoreOp = StoreOp::DontCare,
            .initialLayout  = ImageLayout::Undefined,
            .finalLayout    = ImageLayout::ShaderReadOnlyOptimal,
        };

        Attachment depthAttachment = Attachment{
            .format         = Format::D32_SFLOAT,
            .loadOp         = LoadOp::Clear,
            .storeOp        = StoreOp::DontCare,
            .stencilLoadOp  = LoadOp::DontCare,
            .stencilStoreOp = StoreOp::DontCare,
            .initialLayout  = ImageLayout::Undefined,
            .finalLayout    = ImageLayout::DepthStencilOptimal,
        };

        SubPass subpass = SubPass{.bindPoint = PipelineBindPoint::Graphics};
        subpass.AddColorAttachmentRef(0, ImageLayout::ColorOptimal).SetDepthStencilAttachmentRef(1, ImageLayout::DepthStencilOptimal).Create();

        SubPassDependency dep1 = SubPassDependency{
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0,
            .srcStageMask    = GetPipelineStageFlags(PipelineStageFlags::FragmentShader),
            .dstStageMask    = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
            .srcAccessMask   = GetAccessFlags(AccessFlags::ShaderRead),
            .dstAccessMask   = GetAccessFlags(AccessFlags::ColorAttachmentWrite),
            .dependencyFlags = GetDependencyFlags(DependencyFlag::ByRegion),
        };

        SubPassDependency dep2 = SubPassDependency{
            .srcSubpass      = 0,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
            .dstStageMask    = GetPipelineStageFlags(PipelineStageFlags::FragmentShader),
            .srcAccessMask   = GetAccessFlags(AccessFlags::ColorAttachmentWrite),
            .dstAccessMask   = GetAccessFlags(AccessFlags::ShaderRead),
            .dependencyFlags = GetDependencyFlags(DependencyFlag::ByRegion),
        };

        // Create the actual pass.
        pass.AddAttachment(colorAttachment).AddAttachment(depthAttachment).AddSubpass(subpass).AddSubPassDependency(dep1).AddSubPassDependency(dep2).AddClearValue(clrCol).AddClearValue(clrDepth).Create();

        pass._colorTexture = CreateDefaultPassTextureColor();
        pass._depthTexture = CreateDefaultPassTextureDepth();

        const uint32 imgSize = static_cast<uint32>(Backend::Get()->GetMainSwapchain()._images.size());

        for (uint32 i = 0; i < imgSize; i++)
        {
            pass.framebuffers.push_back(Framebuffer());
            auto& fb = pass.framebuffers[i];
            fb       = Framebuffer{
                      .width  = static_cast<uint32>(size.x),
                      .height = static_cast<uint32>(size.y),
                      .layers = 1,
            };

            fb.AttachRenderPass(pass);
            fb.AddImageView(pass._colorTexture->GetImage()._ptrImgView).AddImageView(pass._depthTexture->GetImage()._ptrImgView);
            fb.Create();
        }
    }

    void VulkanUtility::SetupAndCreateFinalRenderPass(RenderPass& pass)
    {
        const Vector2i size = Backend::Get()->GetMainSwapchain().size;

        Extent3D ext = Extent3D{.width = static_cast<unsigned int>(size.x), .height = static_cast<unsigned int>(size.y), .depth = 1};

        ClearValue clrCol = ClearValue{
            .clearColor = Color(0.1f, 0.1f, 0.1f, 1.0f),
            .isColor    = true,
        };

        ClearValue clrDepth = ClearValue{
            .isColor = false,
            .depth   = 1.0f,
        };

        Attachment colorAttachment = Attachment{
            .format         = Backend::Get()->GetMainSwapchain().format,
            .loadOp         = LoadOp::Clear,
            .storeOp        = StoreOp::Store,
            .stencilLoadOp  = LoadOp::DontCare,
            .stencilStoreOp = StoreOp::DontCare,
            .initialLayout  = ImageLayout::Undefined,
            .finalLayout    = ImageLayout::PresentSurface,
        };

        Attachment depthAttachment = Attachment{
            .format         = Format::D32_SFLOAT,
            .loadOp         = LoadOp::Clear,
            .storeOp        = StoreOp::Store,
            .stencilLoadOp  = LoadOp::Clear,
            .stencilStoreOp = StoreOp::DontCare,
            .initialLayout  = ImageLayout::Undefined,
            .finalLayout    = ImageLayout::DepthStencilOptimal,
        };

        SubPass subpass = SubPass{.bindPoint = PipelineBindPoint::Graphics};
        subpass.AddColorAttachmentRef(0, ImageLayout::ColorOptimal).SetDepthStencilAttachmentRef(1, ImageLayout::DepthStencilOptimal).Create();

        SubPassDependency dep1 = SubPassDependency{
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0,
            .srcStageMask    = GetPipelineStageFlags(PipelineStageFlags::BottomOfPipe),
            .dstStageMask    = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
            .srcAccessMask   = GetAccessFlags(AccessFlags::MemoryRead),
            .dstAccessMask   = GetAccessFlags(AccessFlags::ColorAttachmentRead) | GetAccessFlags(AccessFlags::ColorAttachmentWrite),
            .dependencyFlags = GetDependencyFlags(DependencyFlag::ByRegion),
        };

        SubPassDependency dep2 = SubPassDependency{
            .srcSubpass      = 0,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
            .dstStageMask    = GetPipelineStageFlags(PipelineStageFlags::BottomOfPipe),
            .srcAccessMask   = GetAccessFlags(AccessFlags::ColorAttachmentRead) | GetAccessFlags(AccessFlags::ColorAttachmentWrite),
            .dstAccessMask   = GetAccessFlags(AccessFlags::MemoryRead),
            .dependencyFlags = GetDependencyFlags(DependencyFlag::ByRegion),
        };

        // Create the actual pass.
        pass.AddAttachment(colorAttachment).AddAttachment(depthAttachment).AddSubpass(subpass).AddSubPassDependency(dep1).AddSubPassDependency(dep2).AddClearValue(clrCol).AddClearValue(clrDepth).Create();

        pass._colorTexture = CreateDefaultPassTextureColor();
        pass._depthTexture = CreateDefaultPassTextureDepth();

        const uint32 imgSize = static_cast<uint32>(Backend::Get()->GetMainSwapchain()._imageViews.size());

        for (uint32 i = 0; i < imgSize; i++)
        {
            pass.framebuffers.push_back(Framebuffer());
            auto& fb = pass.framebuffers[i];
            fb       = Framebuffer{
                      .width  = static_cast<uint32>(size.x),
                      .height = static_cast<uint32>(size.y),
                      .layers = 1,
            };

            fb.AttachRenderPass(pass);
            fb.AddImageView(Backend::Get()->GetMainSwapchain()._imageViews[i]).AddImageView(pass._depthTexture->GetImage()._ptrImgView);
            fb.Create();
        }
    }

    void VulkanUtility::SetupAndCreateDefaultRenderPass(RenderPass& pass)
    {
        // const Vector2i size = Vector2i(Backend::Get()->GetMainSwapchain().width, Backend::Get()->GetMainSwapchain().height);

        // Attachment att = Attachment{
        //     .format = m_backend->m_swapchain.format,
        //     .loadOp = LoadOp::Clear,
        // };
        //
        // Attachment depthStencil = Attachment{.format         = Format::D32_SFLOAT,
        //                                      .loadOp         = LoadOp::Clear,
        //                                      .storeOp        = StoreOp::Store,
        //                                      .stencilLoadOp  = LoadOp::Clear,
        //                                      .stencilStoreOp = StoreOp::DontCare,
        //                                      .initialLayout  = ImageLayout::Undefined,
        //                                      .finalLayout    = ImageLayout::DepthStencilOptimal};
        //
        // SubPassDependency dep = SubPassDependency{
        //     .dstSubpass    = 0,
        //     .srcStageMask  = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
        //     .srcAccessMask = 0,
        //     .dstStageMask  = GetPipelineStageFlags(PipelineStageFlags::ColorAttachmentOutput),
        //     .dstAccessMask = GetAccessFlags(AccessFlags::ColorAttachmentWrite),
        // };
        //
        // const uint32 depthFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);
        //
        // SubPassDependency depDepth = SubPassDependency{
        //     .dstSubpass    = 0,
        //     .srcStageMask  = depthFlags,
        //     .srcAccessMask = 0,
        //     .dstStageMask  = depthFlags,
        //     .dstAccessMask = GetAccessFlags(AccessFlags::DepthStencilAttachmentWrite),
        // };
        //
        // ClearValue clrCol = ClearValue{
        //     .clearColor = Color(0.1f, 0.1f, 0.1f, 1.0f),
        //     .isColor    = true,
        // };
        //
        // ClearValue clrDepth = ClearValue{
        //     .isColor = false,
        //     .depth   = 1.0f,
        // };
        //
        // m_passes[RenderPassType::Main]  = Pass();
        // m_passes[RenderPassType::Final] = Pass();
        //
        // auto& mainPass  = m_passes[RenderPassType::Main];
        // auto& finalPass = m_passes[RenderPassType::Final];
        //
        // // Subpasses
        // mainPass.subPass = SubPass{.bindPoint = PipelineBindPoint::Graphics};
        // mainPass.subPass.AddColorAttachmentRef(0, ImageLayout::ColorOptimal).SetDepthStencilAttachmentRef(1, ImageLayout::DepthStencilOptimal).Create();
        //
        // finalPass.subPass = SubPass{.bindPoint = PipelineBindPoint::Graphics};
        // finalPass.subPass.AddColorAttachmentRef(0, ImageLayout::ColorOptimal).SetDepthStencilAttachmentRef(1, ImageLayout::DepthStencilOptimal).Create();
        //
        // // Renderpasses
        // mainPass.renderPass = RenderPass{};
        // mainPass.renderPass.AddAttachment(att)
        //     .AddAttachment(depthStencil)
        //     .AddSubpass(mainPass.subPass)
        //     .AddSubPassDependency(dep)
        //     .AddSubPassDependency(depDepth)
        //     .AddClearValue(clrCol)
        //     .AddClearValue(clrDepth)
        //     .Create();
        //
        // finalPass.renderPass = RenderPass{};
        // finalPass.renderPass.AddAttachment(att)
        //     .AddAttachment(depthStencil)
        //     .AddSubpass(finalPass.subPass)
        //     .AddSubPassDependency(dep)
        //     .AddSubPassDependency(depDepth)
        //     .AddClearValue(clrCol)
        //     .AddClearValue(clrDepth)
        //     .Create();
        //
        // // Images
        // ImageSubresourceRange mainPassRange, mainPassDepthRange;
        // mainPassRange.aspectFlags      = GetImageAspectFlags(ImageAspectFlags::AspectColor);
        // mainPassDepthRange.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectDepth);
        //
        // Image mainPassColor = Image{
        //     .format              = m_backend->m_swapchain.format,
        //     .tiling              = ImageTiling::Optimal,
        //     .extent              = ext,
        //     .imageUsageFlags     = GetImageUsage(ImageUsageFlags::ColorAttachment),
        //     .memoryUsageFlags    = MemoryUsageFlags::GpuOnly,
        //     .memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
        //     .subresRange         = mainPassRange,
        // };
        //
        // Sampler mainPassSampler = Sampler{};
        // mainPass.colorTexture   = new Texture();
        // mainPass.colorTexture->CreateFromRuntime(mainPassColor, mainPassSampler);
        //
        // Image mainPassDepthImg = Image{
        //     .format              = Format::D32_SFLOAT,
        //     .tiling              = ImageTiling::Optimal,
        //     .extent              = ext,
        //     .imageUsageFlags     = GetImageUsage(ImageUsageFlags::DepthStencilAttachment),
        //     .memoryUsageFlags    = MemoryUsageFlags::GpuOnly,
        //     .memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
        //     .subresRange         = mainPassDepthRange,
        // };
        // mainPass.depthTexture = new Texture();
        // mainPass.depthTexture->CreateFromRuntime(mainPassDepthImg, Sampler());
        //
        // // Framebuffers
        // mainPass.frameBuffer = Framebuffer{
        //     .width  = static_cast<uint32>(size.x),
        //     .height = static_cast<uint32>(size.y),
        //     .layers = 1,
        // };
        //
        // mainPass.frameBuffer.AttachRenderPass(mainPass.renderPass);
        // mainPass.frameBuffer.AddImageView(mainPass.colorTexture->GetImage()._ptrImgView).AddImageView(mainPass.depthTexture->GetImage()._ptrImgView).Create();
    }

    VertexInputDescription VulkanUtility::GetVertexDescription()
    {
        VertexInputDescription description;

        VertexInputBinding mainBinding = VertexInputBinding{
            .binding   = 0,
            .stride    = sizeof(Vertex),
            .inputRate = VertexInputRate::Vertex,
        };

        description.bindings.push_back(mainBinding);

        VertexInputAttribute positionAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 0,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, pos),
        };

        VertexInputAttribute normalAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 1,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, normal),
        };

        VertexInputAttribute colorAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 2,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, color),
        };

        VertexInputAttribute uvAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 3,
            .format   = Format::R32G32_SFLOAT,
            .offset   = offsetof(Vertex, uv),
        };

        description.attributes.push_back(positionAtt);
        description.attributes.push_back(normalAtt);
        description.attributes.push_back(colorAtt);
        description.attributes.push_back(uvAtt);

        return description;
    }

    VertexInputDescription VulkanUtility::GetEmptyVertexDescription()
    {
        VertexInputDescription description;
        return description;
    }

    VertexInputDescription VulkanUtility::GetGUIVertexDescription()
    {
        VertexInputDescription description;

        VertexInputBinding mainBinding = VertexInputBinding{
            .binding   = 0,
            .stride    = sizeof(LinaVG::Vertex),
            .inputRate = VertexInputRate::Vertex,
        };

        description.bindings.push_back(mainBinding);

        VertexInputAttribute positionAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 0,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(LinaVG::Vertex, pos),
        };

        VertexInputAttribute uvAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 1,
            .format   = Format::R32G32_SFLOAT,
            .offset   = offsetof(LinaVG::Vertex, uv),
        };

        VertexInputAttribute colorAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 2,
            .format   = Format::R32G32B32A32_SFLOAT,
            .offset   = offsetof(LinaVG::Vertex, col),
        };

        description.attributes.push_back(positionAtt);
        description.attributes.push_back(uvAtt);
        description.attributes.push_back(colorAtt);
        return description;
    }

    void VulkanUtility::GetDescriptionStructs(const VertexInputDescription& desc, Vector<VkVertexInputBindingDescription>& bindingDescs, Vector<VkVertexInputAttributeDescription>& attDescs)
    {
        for (const auto& d : desc.bindings)
        {
            VkVertexInputBindingDescription binding = VkVertexInputBindingDescription{
                .binding   = d.binding,
                .stride    = d.stride,
                .inputRate = GetVertexInputRate(d.inputRate),
            };

            bindingDescs.push_back(binding);
        }

        for (const auto& a : desc.attributes)
        {
            VkVertexInputAttributeDescription att = VkVertexInputAttributeDescription{
                .location = a.location,
                .binding  = a.binding,
                .format   = GetFormat(a.format),
                .offset   = a.offset,
            };

            attDescs.push_back(att);
        }
    }

    VkImageCreateInfo VulkanUtility::GetImageCreateInfo(Format format, uint32 usageFlags, ImageTiling tiling, Extent3D extent, ImageLayout initialLayout, SharingMode sharingMode)
    {
        VkImageCreateInfo info = VkImageCreateInfo{
            .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext         = nullptr,
            .imageType     = VK_IMAGE_TYPE_2D,
            .format        = GetFormat(format),
            .extent        = VkExtent3D{extent.width, extent.height, extent.depth},
            .mipLevels     = 1,
            .arrayLayers   = 1,
            .samples       = VK_SAMPLE_COUNT_1_BIT,
            .tiling        = GetImageTiling(tiling),
            .usage         = usageFlags,
            .sharingMode   = GetSharingMode(sharingMode),
            .initialLayout = GetImageLayout(initialLayout),
        };
        return info;
    }

    VkImageViewCreateInfo VulkanUtility::GetImageViewCreateInfo(VkImage img, Format format, ImageSubresourceRange subres)
    {
        VkImageSubresourceRange subResRange = VkImageSubresourceRange{
            .aspectMask     = subres.aspectFlags,
            .baseMipLevel   = subres.baseMipLevel,
            .levelCount     = subres.levelCount,
            .baseArrayLayer = subres.baseArrayLayer,
            .layerCount     = subres.layerCount,
        };

        VkImageViewCreateInfo info = VkImageViewCreateInfo{
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext            = nullptr,
            .image            = img,
            .viewType         = VK_IMAGE_VIEW_TYPE_2D,
            .format           = GetFormat(format),
            .subresourceRange = subResRange,
        };

        return info;
    }

    VkSubpassDependency VulkanUtility::GetSubpassDependency(SubPassDependency& dependency)
    {
        VkSubpassDependency dep = VkSubpassDependency{
            .srcSubpass      = dependency.srcSubpass,
            .dstSubpass      = dependency.dstSubpass,
            .srcStageMask    = dependency.srcStageMask,
            .dstStageMask    = dependency.dstStageMask,
            .srcAccessMask   = dependency.srcAccessMask,
            .dstAccessMask   = dependency.dstAccessMask,
            .dependencyFlags = dependency.dependencyFlags,
        };

        return dep;
    }

    VkBufferCopy VulkanUtility::GetBufferCopy(const BufferCopy& copy)
    {
        VkBufferCopy c = VkBufferCopy{
            .srcOffset = copy.sourceOffset,
            .dstOffset = copy.destinationOffset,
            .size      = copy.size,
        };

        return c;
    }

    VkBufferImageCopy VulkanUtility::GetBufferImageCopy(const BufferImageCopy& copy)
    {
        VkBufferImageCopy c = VkBufferImageCopy{
            .bufferOffset      = copy.bufferOffset,
            .bufferRowLength   = copy.bufferRowLength,
            .bufferImageHeight = copy.bufferImageHeight,
            .imageSubresource  = GetImageSubresourceLayers(copy.imageSubresource),
            .imageOffset       = GetOffset3D(copy.imageOffset),
            .imageExtent       = GetExtent3D(copy.imageExtent),
        };
        return c;
    }

    VkImageSubresourceLayers VulkanUtility::GetImageSubresourceLayers(const ImageSubresourceRange& r)
    {
        VkImageSubresourceLayers srl = VkImageSubresourceLayers{
            .aspectMask     = r.aspectFlags,
            .mipLevel       = r.baseMipLevel,
            .baseArrayLayer = r.baseArrayLayer,
            .layerCount     = r.layerCount,
        };
        return srl;
    }

    VkDescriptorSetLayoutBinding VulkanUtility::GetDescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& binding)
    {
        VkDescriptorSetLayoutBinding _binding = VkDescriptorSetLayoutBinding{
            .binding         = binding.binding,
            .descriptorType  = GetDescriptorType(binding.type),
            .descriptorCount = binding.descriptorCount,
            .stageFlags      = binding.stageFlags,
        };

        return _binding;
    }

    VkMemoryBarrier VulkanUtility::GetMemoryBarrier(const DefaultMemoryBarrier& bar)
    {
        VkMemoryBarrier b = VkMemoryBarrier{
            .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext         = nullptr,
            .srcAccessMask = bar.srcAccessMask,
            .dstAccessMask = bar.dstAccessMask,
        };
        return b;
    }

    VkBufferMemoryBarrier VulkanUtility::GetBufferMemoryBarrier(const BufferMemoryBarrier& bar)
    {
        VkBufferMemoryBarrier b = VkBufferMemoryBarrier{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                                                        .pNext               = nullptr,
                                                        .srcAccessMask       = bar.srcAccessMask,
                                                        .dstAccessMask       = bar.dstAccessMask,
                                                        .srcQueueFamilyIndex = bar.srcQueueFamilyIndex,
                                                        .dstQueueFamilyIndex = bar.dstQueueFamilyIndex,
                                                        .buffer              = bar.buffer,
                                                        .offset              = bar.offset,
                                                        .size                = bar.size};

        return b;
    }

    VkImageMemoryBarrier VulkanUtility::GetImageMemoryBarrier(const ImageMemoryBarrier& bar)
    {
        VkImageSubresourceRange subresRange = VkImageSubresourceRange{
            .aspectMask     = bar.subresourceRange.aspectFlags,
            .baseMipLevel   = bar.subresourceRange.baseMipLevel,
            .levelCount     = bar.subresourceRange.levelCount,
            .baseArrayLayer = bar.subresourceRange.baseArrayLayer,
            .layerCount     = bar.subresourceRange.layerCount,
        };

        VkImageMemoryBarrier b = VkImageMemoryBarrier{

            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext               = nullptr,
            .srcAccessMask       = bar.srcAccessMask,
            .dstAccessMask       = bar.dstAccessMask,
            .oldLayout           = GetImageLayout(bar.oldLayout),
            .newLayout           = GetImageLayout(bar.newLayout),
            .srcQueueFamilyIndex = bar.srcQueueFamilyIndex,
            .dstQueueFamilyIndex = bar.dstQueueFamilyIndex,
            .image               = bar.img,
            .subresourceRange    = subresRange,
        };
        return b;
    }

    VkViewport VulkanUtility::GetViewport(Viewport& vp)
    {
        VkViewport vkp = VkViewport{
            .x        = vp.x,
            .y        = vp.y,
            .width    = vp.width,
            .height   = vp.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        return vkp;
    }

    VkRect2D VulkanUtility::GetRect(Recti& rect)
    {
        VkRect2D r;
        r.offset.x      = rect.pos.x;
        r.offset.y      = rect.pos.y;
        r.extent.width  = rect.size.x;
        r.extent.height = rect.size.y;
        return r;
    }

    VkExtent3D VulkanUtility::GetExtent3D(Extent3D e)
    {
        VkExtent3D ex = VkExtent3D{
            .width  = e.width,
            .height = e.height,
            .depth  = e.depth,
        };

        return ex;
    }

    VkOffset3D VulkanUtility::GetOffset3D(Offset3D o)
    {
        VkOffset3D off = VkOffset3D{
            .x = o.x,
            .y = o.y,
            .z = o.z,
        };
        return off;
    }

    size_t VulkanUtility::PadUniformBufferSize(size_t originalSize)
    {
        size_t minUboAlignment = static_cast<size_t>(Backend::Get()->GetMinUniformBufferOffsetAlignment());
        size_t alignedSize     = originalSize;
        if (minUboAlignment > 0)
        {
            alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
        }
        return alignedSize;
    }

} // namespace Lina::Graphics
