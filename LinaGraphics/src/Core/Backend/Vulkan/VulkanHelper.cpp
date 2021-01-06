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

#include "Core/Backend/Vulkan/VulkanHelper.hpp"

namespace Lina::Graphics
{
	VkRenderPass VulkanHelper::RenderPassCreateGeometryAndPostProcess()
	{
		std::vector<VkAttachmentDescription> attachments_descriptions = {
	  {
		0,                                                // VkAttachmentDescriptionFlags     flags
		VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
		VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
		VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
		VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout                    finalLayout
	  },
	  {
		0,                                                // VkAttachmentDescriptionFlags     flags
		VK_FORMAT_D16_UNORM,                              // VkFormat                         format
		VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
		VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
		VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // VkImageLayout                    finalLayout
	  },
	  {
		0,                                                // VkAttachmentDescriptionFlags     flags
		VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
		VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
		VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
		VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
		VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                    finalLayout
	  },
		};

		VkAttachmentReference depth_stencil_attachment = {
		  1,                                                  // uint32_t                             attachment
		  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
		};

		std::vector<SubpassParameters> subpass_parameters = {
			// #0 subpass
			{
			  VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
			  {},                                               // std::vector<VkAttachmentReference>   InputAttachments
			  {                                                 // std::vector<VkAttachmentReference>   ColorAttachments
				{
				  0,                                              // uint32_t                             attachment
				  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        // VkImageLayout                        layout
				}
			  },
			  {},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
			  &depth_stencil_attachment,                        // const VkAttachmentReference        * DepthStencilAttachment
			  {}                                                // std::vector<uint32_t>                PreserveAttachments
			},
			// #1 subpass
			{
			  VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
			  {                                                 // std::vector<VkAttachmentReference>   InputAttachments
				{
				  0,                                              // uint32_t                             attachment
				  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL        // VkImageLayout                        layout
				}
			  },
			  {                                                 // std::vector<VkAttachmentReference>   ColorAttachments
				{
				  2,                                              // uint32_t                             attachment
				  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        // VkImageLayout                        layout
				}
			  },
			  {},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
			  nullptr,                                          // const VkAttachmentReference        * DepthStencilAttachment
			  {}                                                // std::vector<uint32_t>                PreserveAttachments
			}
		};

		std::vector<VkSubpassDependency> subpass_dependencies = {
		  {
			0,                                              // uint32_t                 srcSubpass
			1,                                              // uint32_t                 dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags     srcStageMask
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,          // VkPipelineStageFlags     dstStageMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags            srcAccessMask
			VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,            // VkAccessFlags            dstAccessMask
			VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags        dependencyFlags
		  }
		};

		return m_logicalDevice->RenderPassCreate(attachments_descriptions, subpass_parameters, subpass_dependencies);
	}

}