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

#include "Graphics/Platform/Vulkan/Utility/VulkanUtility.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Graphics/Platform/Vulkan/Core/GfxBackend.hpp"
#include "Graphics/Platform/Vulkan/Objects/RenderPass.hpp"
#include "Graphics/Platform/Vulkan/Objects/Framebuffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/Sampler.hpp"
#include "Graphics/Platform/Vulkan/Objects/Image.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina
{
	VkDevice_T*				   VulkanUtility::s_device				   = nullptr;
	VkAllocationCallbacks*	   VulkanUtility::s_allCb				   = nullptr;
	VmaAllocator_T*			   VulkanUtility::s_vma					   = nullptr;
	PFN_vkCmdBeginRenderingKHR VulkanUtility::s_vkCmdBeginRenderingKHR = nullptr;
	PFN_vkCmdEndRenderingKHR   VulkanUtility::s_vkCmdEndRenderingKHR   = nullptr;

	VkAttachmentDescription VulkanUtility::CreateAttachmentDescription(const Attachment& att)
	{
		return VkAttachmentDescription{
			.flags			= att.flags,
			.format			= GetFormat(att.format),
			.samples		= VK_SAMPLE_COUNT_1_BIT,
			.loadOp			= GetLoadOp(att.loadOp),
			.storeOp		= GetStoreOp(att.storeOp),
			.stencilLoadOp	= GetLoadOp(att.stencilLoadOp),
			.stencilStoreOp = GetStoreOp(att.stencilStoreOp),
			.initialLayout	= GetImageLayout(att.initialLayout),
			.finalLayout	= GetImageLayout(att.finalLayout),
		};
	}
	VkPipelineShaderStageCreateInfo VulkanUtility::CreatePipelineShaderStageCreateInfo(ShaderStage stage, VkShaderModule shaderModule)
	{
		VkPipelineShaderStageCreateInfo info = VkPipelineShaderStageCreateInfo{
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext	= nullptr,
			.stage	= static_cast<VkShaderStageFlagBits>(GetShaderStage(stage)),
			.module = shaderModule,
			.pName	= "main",
		};

		return info;
	}
	VkPipelineVertexInputStateCreateInfo VulkanUtility::CreatePipelineVertexInputStateCreateInfo(const Vector<VkVertexInputBindingDescription>& bindingDescs, const Vector<VkVertexInputAttributeDescription>& attDescs)
	{
		VkPipelineVertexInputStateCreateInfo info = VkPipelineVertexInputStateCreateInfo{
			.sType							 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext							 = nullptr,
			.vertexBindingDescriptionCount	 = static_cast<uint32>(bindingDescs.size()),
			.pVertexBindingDescriptions		 = bindingDescs.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32>(attDescs.size()),
			.pVertexAttributeDescriptions	 = attDescs.data(),
		};

		return info;
	}
	VkPipelineInputAssemblyStateCreateInfo VulkanUtility::CreatePipelineInputAssemblyCreateInfo(Topology top, bool primitiveRestart)
	{
		VkPipelineInputAssemblyStateCreateInfo info = VkPipelineInputAssemblyStateCreateInfo{
			.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext					= nullptr,
			.topology				= GetTopology(top),
			.primitiveRestartEnable = primitiveRestart ? VK_TRUE : VK_FALSE,
		};

		return info;
	}

	VkPipelineRasterizationStateCreateInfo VulkanUtility::CreatePipelineRasterStateCreateInfo(PolygonMode pm, CullMode cm, FrontFace frontFace)
	{
		VkPipelineRasterizationStateCreateInfo info = VkPipelineRasterizationStateCreateInfo{
			.sType					 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext					 = nullptr,
			.depthClampEnable		 = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode			 = GetPolygonMode(pm),
			.cullMode				 = GetCullMode(cm),
			.frontFace				 = GetFrontFace(frontFace),
			.depthBiasEnable		 = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp			 = 0.0f,
			.depthBiasSlopeFactor	 = 0.0f,
			.lineWidth				 = 1.0f,
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
			.minSampleShading	   = 1.0f,
			.pSampleMask		   = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable	   = VK_FALSE,

		};
		return info;
	}

	VkPipelineColorBlendAttachmentState VulkanUtility::CreatePipelineBlendAttachmentState(ColorBlendAttachmentState blendAtt)
	{
		VkPipelineColorBlendAttachmentState info = VkPipelineColorBlendAttachmentState{
			.blendEnable		 = blendAtt.blendEnable ? VK_TRUE : VK_FALSE,
			.srcColorBlendFactor = GetBlendFactor(blendAtt.srcColorBlendFactor),
			.dstColorBlendFactor = GetBlendFactor(blendAtt.dstColorBlendFactor),
			.colorBlendOp		 = GetBlendOp(blendAtt.colorBlendOp),
			.srcAlphaBlendFactor = GetBlendFactor(blendAtt.srcAlphaBlendFactor),
			.dstAlphaBlendFactor = GetBlendFactor(blendAtt.dstAlphaBlendFactor),
			.alphaBlendOp		 = GetBlendOp(blendAtt.alphaBlendOp),
			.colorWriteMask		 = GetColorComponentFlags(blendAtt.componentFlags),
		};
		return info;
	}

	VkPipelineDepthStencilStateCreateInfo VulkanUtility::CreatePipelineDepthStencilStateCreateInfo(bool depthTest, bool depthWrite, CompareOp op)
	{
		VkPipelineDepthStencilStateCreateInfo info = {
			.sType				   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext				   = nullptr,
			.depthTestEnable	   = depthTest ? VK_TRUE : VK_FALSE,
			.depthWriteEnable	   = depthWrite ? VK_TRUE : VK_FALSE,
			.depthCompareOp		   = depthTest ? GetCompareOp(op) : VK_COMPARE_OP_ALWAYS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable	   = VK_FALSE,
			.minDepthBounds		   = 0.0f, // Optional
			.maxDepthBounds		   = 1.0f, // Optional
		};

		return info;
	}

	Texture* VulkanUtility::CreateDefaultPassTextureColor(Format format, int width, int height)
	{
		Extent3D ext = Extent3D{.depth = 1};
		ext.width	 = width;
		ext.height	 = height;

		ImageSubresourceRange range;
		range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);

		Image image = Image{
			.format				 = format,
			.tiling				 = ImageTiling::Optimal,
			.extent				 = ext,
			.imageUsageFlags	 = GetImageUsage(ImageUsageFlags::ColorAttachment) | GetImageUsage(ImageUsageFlags::Sampled),
			.memoryUsageFlags	 = MemoryUsageFlags::GpuOnly,
			.memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
			.subresRange		 = range,
		};

		// Color texture
		auto	 sd	 = DEFAULT_SAMPLER_DATA;
		Texture* txt = new Texture(ext, sd, format, ImageTiling::Optimal);
		txt->GenerateImage(GetImageAspectFlags(ImageAspectFlags::AspectColor), GetImageUsage(ImageUsageFlags::ColorAttachment) | GetImageUsage(ImageUsageFlags::Sampled));
		return txt;
	}

	Texture* VulkanUtility::CreateDefaultPassTextureDepth(int width, int height)
	{
		Extent3D ext = Extent3D{.depth = 1};
		ext.width	 = width;
		ext.height	 = height;

		ImageSubresourceRange range;
		range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectDepth); // | GetImageAspectFlags(ImageAspectFlags::AspectStencil);

		Image image = Image{
			.format				 = DEFAULT_DEPTH_FORMAT,
			.tiling				 = ImageTiling::Optimal,
			.extent				 = ext,
			.imageUsageFlags	 = GetImageUsage(ImageUsageFlags::DepthStencilAttachment),
			.memoryUsageFlags	 = MemoryUsageFlags::GpuOnly,
			.memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
			.subresRange		 = range,
		};

		// Texture
		auto	 sd	 = DEFAULT_SAMPLER_DATA;
		Texture* txt = new Texture(ext, sd, DEFAULT_DEPTH_FORMAT, ImageTiling::Optimal);
		txt->GenerateImage(GetImageAspectFlags(ImageAspectFlags::AspectDepth), GetImageUsage(ImageUsageFlags::DepthStencilAttachment));
		return txt;
	}

	void VulkanUtility::CreateDefaultPassImageDepth(Image& img, int width, int height)
	{
		Extent3D ext = Extent3D{.depth = 1};
		ext.width	 = width;
		ext.height	 = height;

		ImageSubresourceRange range;
		range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectDepth); // | GetImageAspectFlags(ImageAspectFlags::AspectStencil);

		img = Image{
			.vmaAllocator		 = s_vma,
			.device				 = s_device,
			.allocationCb		 = s_allCb,
			.format				 = DEFAULT_DEPTH_FORMAT,
			.tiling				 = ImageTiling::Optimal,
			.extent				 = ext,
			.imageUsageFlags	 = GetImageUsage(ImageUsageFlags::DepthStencilAttachment),
			.memoryUsageFlags	 = MemoryUsageFlags::GpuOnly,
			.memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
			.subresRange		 = range,
		};

		img.Create(true);
	}

	void VulkanUtility::CreateDefaultPassImageColor(Format format, Image& img, int width, int height)
	{
		Extent3D ext = Extent3D{.depth = 1};
		ext.width	 = width;
		ext.height	 = height;

		ImageSubresourceRange range;
		range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);

		img = Image{
			.vmaAllocator		 = s_vma,
			.device				 = s_device,
			.allocationCb		 = s_allCb,
			.format				 = format,
			.tiling				 = ImageTiling::Optimal,
			.extent				 = ext,
			.imageUsageFlags	 = GetImageUsage(ImageUsageFlags::ColorAttachment) | GetImageUsage(ImageUsageFlags::Sampled),
			.memoryUsageFlags	 = MemoryUsageFlags::GpuOnly,
			.memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
			.subresRange		 = range,
		};

		img.Create(true);
	}

	VertexInputDescription VulkanUtility::GetVertexDescription()
	{
		VertexInputDescription description;

		VertexInputBinding mainBinding = VertexInputBinding{
			.binding   = 0,
			.stride	   = sizeof(Vertex),
			.inputRate = VertexInputRate::Vertex,
		};

		description.bindings.push_back(mainBinding);

		VertexInputAttribute positionAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 0,
			.format	  = Format::R32G32B32_SFLOAT,
			.offset	  = offsetof(Vertex, pos),
		};

		VertexInputAttribute normalAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 1,
			.format	  = Format::R32G32B32_SFLOAT,
			.offset	  = offsetof(Vertex, normal),
		};

		VertexInputAttribute colorAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 2,
			.format	  = Format::R32G32B32_SFLOAT,
			.offset	  = offsetof(Vertex, color),
		};

		VertexInputAttribute uvAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 3,
			.format	  = Format::R32G32_SFLOAT,
			.offset	  = offsetof(Vertex, uv),
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
			.stride	   = sizeof(LinaVG::Vertex),
			.inputRate = VertexInputRate::Vertex,
		};

		description.bindings.push_back(mainBinding);

		VertexInputAttribute positionAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 0,
			.format	  = Format::R32G32B32_SFLOAT,
			.offset	  = offsetof(LinaVG::Vertex, pos),
		};

		VertexInputAttribute uvAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 1,
			.format	  = Format::R32G32_SFLOAT,
			.offset	  = offsetof(LinaVG::Vertex, uv),
		};

		VertexInputAttribute colorAtt = VertexInputAttribute{
			.binding  = 0,
			.location = 2,
			.format	  = Format::R32G32B32A32_SFLOAT,
			.offset	  = offsetof(LinaVG::Vertex, col),
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
				.stride	   = d.stride,
				.inputRate = GetVertexInputRate(d.inputRate),
			};

			bindingDescs.push_back(binding);
		}

		for (const auto& a : desc.attributes)
		{
			VkVertexInputAttributeDescription att = VkVertexInputAttributeDescription{
				.location = a.location,
				.binding  = a.binding,
				.format	  = GetFormat(a.format),
				.offset	  = a.offset,
			};

			attDescs.push_back(att);
		}
	}

	VkImageCreateInfo VulkanUtility::GetImageCreateInfo(Format format, uint32 usageFlags, ImageTiling tiling, Extent3D extent, ImageLayout initialLayout, SharingMode sharingMode, uint32 mipLevels)
	{
		VkImageCreateInfo info = VkImageCreateInfo{
			.sType		   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext		   = nullptr,
			.imageType	   = VK_IMAGE_TYPE_2D,
			.format		   = GetFormat(format),
			.extent		   = VkExtent3D{extent.width, extent.height, extent.depth},
			.mipLevels	   = mipLevels,
			.arrayLayers   = 1,
			.samples	   = VK_SAMPLE_COUNT_1_BIT,
			.tiling		   = GetImageTiling(tiling),
			.usage		   = usageFlags,
			.sharingMode   = GetSharingMode(sharingMode),
			.initialLayout = GetImageLayout(initialLayout),
		};
		return info;
	}

	VkImageViewCreateInfo VulkanUtility::GetImageViewCreateInfo(VkImage img, Format format, ImageSubresourceRange subres)
	{
		VkImageSubresourceRange subResRange = VkImageSubresourceRange{
			.aspectMask		= subres.aspectFlags,
			.baseMipLevel	= subres.baseMipLevel,
			.levelCount		= subres.levelCount,
			.baseArrayLayer = subres.baseArrayLayer,
			.layerCount		= subres.layerCount,
		};

		VkImageViewCreateInfo info = VkImageViewCreateInfo{
			.sType			  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext			  = nullptr,
			.image			  = img,
			.viewType		  = VK_IMAGE_VIEW_TYPE_2D,
			.format			  = GetFormat(format),
			.subresourceRange = subResRange,
		};

		return info;
	}

	VkSubpassDependency VulkanUtility::GetSubpassDependency(SubPassDependency& dependency)
	{
		VkSubpassDependency dep = VkSubpassDependency{
			.srcSubpass		 = dependency.srcSubpass,
			.dstSubpass		 = dependency.dstSubpass,
			.srcStageMask	 = dependency.srcStageMask,
			.dstStageMask	 = dependency.dstStageMask,
			.srcAccessMask	 = dependency.srcAccessMask,
			.dstAccessMask	 = dependency.dstAccessMask,
			.dependencyFlags = dependency.dependencyFlags,
		};

		return dep;
	}

	VkBufferCopy VulkanUtility::GetBufferCopy(const BufferCopy& copy)
	{
		VkBufferCopy c = VkBufferCopy{
			.srcOffset = copy.sourceOffset,
			.dstOffset = copy.destinationOffset,
			.size	   = copy.size,
		};

		return c;
	}

	VkBufferImageCopy VulkanUtility::GetBufferImageCopy(const BufferImageCopy& copy)
	{
		VkBufferImageCopy c = VkBufferImageCopy{
			.bufferOffset	   = copy.bufferOffset,
			.bufferRowLength   = copy.bufferRowLength,
			.bufferImageHeight = copy.bufferImageHeight,
			.imageSubresource  = GetImageSubresourceLayers(copy.imageSubresource),
			.imageOffset	   = GetOffset3D(copy.imageOffset),
			.imageExtent	   = GetExtent3D(copy.imageExtent),
		};
		return c;
	}

	VkImageSubresourceLayers VulkanUtility::GetImageSubresourceLayers(const ImageSubresourceRange& r)
	{
		VkImageSubresourceLayers srl = VkImageSubresourceLayers{
			.aspectMask		= r.aspectFlags,
			.mipLevel		= r.baseMipLevel,
			.baseArrayLayer = r.baseArrayLayer,
			.layerCount		= r.layerCount,
		};
		return srl;
	}

	VkDescriptorSetLayoutBinding VulkanUtility::GetDescriptorSetLayoutBinding(const UserBinding& binding)
	{
		uint32 stageFlags = 0;
		for (auto sf : binding.stages)
			stageFlags |= GetShaderStage(sf);

		VkDescriptorSetLayoutBinding _binding = VkDescriptorSetLayoutBinding{
			.binding		 = binding.binding,
			.descriptorType	 = GetDescriptorType(binding.type),
			.descriptorCount = binding.descriptorCount,
			.stageFlags		 = stageFlags,
		};

		return _binding;
	}

	VkMemoryBarrier VulkanUtility::GetMemoryBarrier(const DefaultMemoryBarrier& bar)
	{
		VkMemoryBarrier b = VkMemoryBarrier{
			.sType		   = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
			.pNext		   = nullptr,
			.srcAccessMask = bar.srcAccessMask,
			.dstAccessMask = bar.dstAccessMask,
		};
		return b;
	}

	VkBufferMemoryBarrier VulkanUtility::GetBufferMemoryBarrier(const BufferMemoryBarrier& bar)
	{
		VkBufferMemoryBarrier b = VkBufferMemoryBarrier{.sType				 = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
														.pNext				 = nullptr,
														.srcAccessMask		 = bar.srcAccessMask,
														.dstAccessMask		 = bar.dstAccessMask,
														.srcQueueFamilyIndex = bar.srcQueueFamilyIndex,
														.dstQueueFamilyIndex = bar.dstQueueFamilyIndex,
														.buffer				 = bar.buffer,
														.offset				 = bar.offset,
														.size				 = bar.size};

		return b;
	}

	VkImageMemoryBarrier VulkanUtility::GetImageMemoryBarrier(const ImageMemoryBarrier& bar)
	{
		VkImageSubresourceRange subresRange = VkImageSubresourceRange{
			.aspectMask		= bar.subresourceRange.aspectFlags,
			.baseMipLevel	= bar.subresourceRange.baseMipLevel,
			.levelCount		= bar.subresourceRange.levelCount,
			.baseArrayLayer = bar.subresourceRange.baseArrayLayer,
			.layerCount		= bar.subresourceRange.layerCount,
		};

		VkImageMemoryBarrier b = VkImageMemoryBarrier{

			.sType				 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext				 = nullptr,
			.srcAccessMask		 = bar.srcAccessMask,
			.dstAccessMask		 = bar.dstAccessMask,
			.oldLayout			 = GetImageLayout(bar.oldLayout),
			.newLayout			 = GetImageLayout(bar.newLayout),
			.srcQueueFamilyIndex = bar.srcQueueFamilyIndex,
			.dstQueueFamilyIndex = bar.dstQueueFamilyIndex,
			.image				 = bar.img,
			.subresourceRange	 = subresRange,
		};
		return b;
	}

	VkViewport VulkanUtility::GetViewport(const Viewport& vp)
	{
		VkViewport vkp = VkViewport{
			.x		  = vp.x,
			.y		  = vp.y,
			.width	  = vp.width,
			.height	  = vp.height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		return vkp;
	}

	VkRect2D VulkanUtility::GetRect(const Recti& rect)
	{
		VkRect2D r;
		r.offset.x		= rect.pos.x;
		r.offset.y		= rect.pos.y;
		r.extent.width	= rect.size.x;
		r.extent.height = rect.size.y;
		return r;
	}

	VkExtent3D VulkanUtility::GetExtent3D(const Extent3D& e)
	{
		VkExtent3D ex = VkExtent3D{
			.width	= e.width,
			.height = e.height,
			.depth	= e.depth,
		};

		return ex;
	}

	VkOffset3D VulkanUtility::GetOffset3D(const Offset3D& o)
	{
		VkOffset3D off = VkOffset3D{
			.x = o.x,
			.y = o.y,
			.z = o.z,
		};
		return off;
	}

	Offset3D VulkanUtility::GetOffset3D(const Extent3D& e)
	{
		Offset3D off = Offset3D{
			.x = static_cast<int>(e.width),
			.y = static_cast<int>(e.height),
			.z = static_cast<int>(e.depth),
		};
		return off;
	}

	size_t VulkanUtility::PadUniformBufferSize(size_t originalSize, uint32 minUniformBufferOffsetAlignment)
	{
		size_t minUboAlignment = static_cast<size_t>(minUniformBufferOffsetAlignment);
		size_t alignedSize	   = originalSize;
		if (minUboAlignment > 0)
		{
			alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}
		return alignedSize;
	}

	VkClearValue VulkanUtility::GetClearValue(const ClearValue& cv)
	{
		VkClearValue clearValue;
		if (cv.isColor)
			clearValue.color = {{cv.clearColor.x, cv.clearColor.y, cv.clearColor.z, cv.clearColor.w}};
		else
		{
			clearValue.depthStencil.depth	= cv.depth;
			clearValue.depthStencil.stencil = cv.stencil;
		}

		return clearValue;
	}

	VkRenderingAttachmentInfo VulkanUtility::GetRenderingAttachmentInfo(const RenderingAttachmentInfo& inf)
	{
		VkRenderingAttachmentInfo info = VkRenderingAttachmentInfo{
			.sType				= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.pNext				= nullptr,
			.imageView			= inf.imageView,
			.imageLayout		= GetImageLayout(inf.imageLayout),
			.resolveMode		= GetResolveModeFlags(inf.resolveMode),
			.resolveImageView	= inf.resolveView,
			.resolveImageLayout = GetImageLayout(inf.resolveLayout),
			.loadOp				= GetLoadOp(inf.loadOp),
			.storeOp			= GetStoreOp(inf.storeOp),
			.clearValue			= GetClearValue(inf.clearValue),
		};
		return info;
	} // namespace Lina::Graphics

	VkRenderingInfo VulkanUtility::GetRenderingInfo(const RenderingInfo& inf)
	{
		VkRenderingInfo info = VkRenderingInfo{
			.sType				= VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
			.pNext				= nullptr,
			.flags				= inf.flags,
			.renderArea			= GetRect(inf.renderArea),
			.layerCount			= inf.layerCount,
			.viewMask			= inf.viewMask,
			.pDepthAttachment	= nullptr,
			.pStencilAttachment = nullptr,
		};

		return info;
	}

} // namespace Lina