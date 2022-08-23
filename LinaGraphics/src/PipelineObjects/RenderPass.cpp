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

#include "PipelineObjects/RenderPass.hpp"
#include "Data/Vector.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "Math/Vector.hpp"
#include "Core/Window.hpp"
#include "PipelineObjects/Framebuffer.hpp"
#include "PipelineObjects/CommandBuffer.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"

namespace Lina::Graphics
{

    void RenderPass::Create()
    {
        Vector<VkSubpassDescription>    subpassDescriptions;
        Vector<VkAttachmentDescription> attachmentDescriptions;
        Vector<VkAttachmentReference>   colorAttachments;
        Vector<VkAttachmentReference>   depthStencilAttachments;
        uint32                          colorAttIndex = 0;

        for (auto& sp : subpasses)
        {
            uint32 addedSize = 0;
            for (auto pair : sp._colorAttachmentRefs)
            {
                VkAttachmentReference ref = VkAttachmentReference{.attachment = pair.first, .layout = GetImageLayout(pair.second)};
                colorAttachments.push_back(ref);
                addedSize++;
            }

            VkSubpassDescription d = VkSubpassDescription{
                .pipelineBindPoint    = GetPipelineBindPoint(sp.bindPoint),
                .colorAttachmentCount = addedSize,
                .pColorAttachments    = &colorAttachments[colorAttIndex]};

            if (sp._depthStencilAttachmentSet)
            {
                VkAttachmentReference depthStencilRef = VkAttachmentReference{.attachment = sp._depthStencilAttachmentIndex, .layout = GetImageLayout(sp._depthStencilAttachmentLayout)};
                depthStencilAttachments.push_back(depthStencilRef);
                d.pDepthStencilAttachment = &depthStencilAttachments[depthStencilAttachments.size() - 1];
            }

            colorAttIndex += addedSize;
            subpassDescriptions.push_back(d);
        }

        for (const auto& att : attachments)
            attachmentDescriptions.push_back(VulkanUtility::CreateAttachmentDescription(att));

        Vector<VkSubpassDependency> _dependencies;

        for (auto& d : dependencies)
            _dependencies.push_back(VulkanUtility::GetSubpassDependency(d));

        VkRenderPassCreateInfo rpInfo = VkRenderPassCreateInfo{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32>(attachmentDescriptions.size()),
            .pAttachments    = attachmentDescriptions.data(),
            .subpassCount    = static_cast<uint32>(subpassDescriptions.size()),
            .pSubpasses      = subpassDescriptions.data(),
            .dependencyCount = static_cast<uint32>(_dependencies.size()),
            .pDependencies   = _dependencies.data(),
        };

        VkResult result = vkCreateRenderPass(Backend::Get()->GetDevice(), &rpInfo, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Render Pass] -> Could not create Vulkan render pass!");

        VkRenderPass_T* ptr = _ptr;
        RenderEngine::Get()->GetMainDeletionQueue().Push([ptr]() {
            vkDestroyRenderPass(Backend::Get()->GetDevice(), ptr, Backend::Get()->GetAllocator());
        });
    }

    RenderPass& RenderPass::AddSubpass(SubPass sp)
    {
        subpasses.push_back(sp);
        return *this;
    }

    RenderPass& RenderPass::AddAttachment(Attachment att)
    {
        attachments.push_back(att);
        return *this;
    }

    RenderPass& RenderPass::AddSubPassDependency(SubPassDependency& d)
    {
        dependencies.push_back(d);
        return *this;
    }

    RenderPass& RenderPass::AddClearValue(const ClearValue& clear)
    {
        clearValues.push_back(clear);
        return *this;
    }

    void RenderPass::Begin(const Framebuffer& fb, const CommandBuffer& cmd)
    {
        const Vector2i windowSize = Window::Get()->GetSize();
        const Vector2i pos        = Window::Get()->GetPos();

        Vector<VkClearValue> _clearValues;

        for (auto& cv : clearValues)
        {
            VkClearValue clearValue;
            if (cv.isColor)
                clearValue.color = {{cv.clearColor.r, cv.clearColor.g, cv.clearColor.b, cv.clearColor.a}};
            else
            {
                clearValue.depthStencil.depth   = cv.depth;
                clearValue.depthStencil.stencil = cv.stencil;
            }

            _clearValues.push_back(clearValue);
        }

        VkRenderPassBeginInfo info = VkRenderPassBeginInfo{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext           = nullptr,
            .renderPass      = _ptr,
            .framebuffer     = fb._ptr,
            .clearValueCount = static_cast<uint32>(clearValues.size()),
            .pClearValues    = _clearValues.data(),
        };

        info.renderArea.extent.width  = static_cast<uint32>(windowSize.x);
        info.renderArea.extent.height = static_cast<uint32>(windowSize.y);
        info.renderArea.offset.x      = static_cast<uint32>(pos.x);
        info.renderArea.offset.y      = static_cast<uint32>(pos.y);

        // Bind the framebuffer & clear the images
        vkCmdBeginRenderPass(cmd._ptr, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderPass::End(const CommandBuffer& cmd)
    {
        vkCmdEndRenderPass(cmd._ptr);
    }

    void SubPass::Create()
    {
        // Description needs to be created within the used renderpass due to invalidated pointers of color attachments array.
    }

    SubPass& SubPass::AddColorAttachmentRef(uint32 index, ImageLayout layout)
    {
        _colorAttachmentRefs[index] = layout;
        return *this;
    }

    SubPass& SubPass::SetDepthStencilAttachmentRef(uint32 index, ImageLayout layout)
    {
        _depthStencilAttachmentIndex  = index;
        _depthStencilAttachmentSet    = true;
        _depthStencilAttachmentLayout = layout;
        return *this;
    }

} // namespace Lina::Graphics
