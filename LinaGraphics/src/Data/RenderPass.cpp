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

#include "Data/RenderPass.hpp"
#include "Data/Vector.hpp"
#include "Core/Backend.hpp"
#include "Math/Vector.hpp"
#include "Core/Window.hpp"
#include "Data/Framebuffer.hpp"
#include "Data/CommandBuffer.hpp"

namespace Lina::Graphics
{
    RenderPass RenderPass::Create()
    {
        _subpassDescriptions.clear();
        _attachmentDescriptions.clear();

        for (auto& sp : subpasses)
        {
            sp._desc = VkSubpassDescription{
                .pipelineBindPoint    = static_cast<VkPipelineBindPoint>(sp.bindPoint),
                .colorAttachmentCount = static_cast<uint32>(sp._colorAttachments.size()),
                .pColorAttachments    = sp._colorAttachments.data(),
            };
            _subpassDescriptions.push_back(sp._desc);
        }

        for (const auto& att : attachments)
            _attachmentDescriptions.push_back(att._desc);

        VkRenderPassCreateInfo rpInfo = VkRenderPassCreateInfo{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32>(_attachmentDescriptions.size()),
            .pAttachments    = _attachmentDescriptions.data(),
            .subpassCount    = static_cast<uint32>(_subpassDescriptions.size()),
            .pSubpasses      = _subpassDescriptions.data(),
        };

        VkResult result = vkCreateRenderPass(Backend::Get()->GetDevice(), &rpInfo, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Render Pass] -> Could not create Vulkan render pass!");
        return *this;
    }

    RenderPass RenderPass::AddSubpass(SubPass sp)
    {
        subpasses.push_back(sp);
        return *this;
    }

    RenderPass RenderPass::AddAttachment(Attachment att)
    {
        attachments.push_back(att);
        return *this;
    }

    void RenderPass::Destroy()
    {
        if (_ptr != nullptr)
            vkDestroyRenderPass(Backend::Get()->GetDevice(), _ptr, Backend::Get()->GetAllocator());
    }

    void RenderPass::Begin(const ClearValue& clr, const Framebuffer& fb, const CommandBuffer& cmd)
    {
        const Vector2i windowSize = Window::Get()->GetSize();
        const Vector2i pos        = Window::Get()->GetPos();

        VkClearColorValue clearColor = {clr.clearColor.r, clr.clearColor.g, clr.clearColor.b, clr.clearColor.a};
        VkClearValue      clearValue = {};
        clearValue.color             = clearColor;

        VkRenderPassBeginInfo info = VkRenderPassBeginInfo{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext           = nullptr,
            .renderPass      = _ptr,
            .framebuffer     = fb._ptr,
            .clearValueCount = 1,
            .pClearValues    = &clearValue,
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

    SubPass SubPass::Create()
    {
        for (auto att : colorAttachmentRefs)
        {
            VkAttachmentReference ref = VkAttachmentReference{.attachment = att.first, .layout = static_cast<VkImageLayout>(att.second)};
            _colorAttachments.push_back(ref);
        }

        // Description needs to be created within the used renderpass due to invalidated pointers of color attachments array.
        return *this;
    }

    SubPass SubPass::AddColorAttachmentRef(uint32 index, ImageLayout layout)
    {
        colorAttachmentRefs[index] = layout;
        return *this;
    }
} // namespace Lina::Graphics
