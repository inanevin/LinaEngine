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

#ifndef VulkanStructures_HPP
#define VulkanStructures_HPP

#include "Graphics/Core/CommonGraphics.hpp"
#include "Math/Color.hpp"
#include "Math/Rect.hpp"
#include "Math/Matrix.hpp"

enum VkFormat;
enum VkColorSpaceKHR;
enum VkPresentModeKHR;
enum VkCommandBufferLevel;
enum VkCommandPoolCreateFlagBits;
enum VkQueueFlagBits;
enum VkImageLayout;
enum VkPipelineBindPoint;
enum VkAttachmentLoadOp;
enum VkAttachmentStoreOp;
enum VkCompareOp;
enum VkFenceCreateFlagBits;
enum VkCommandBufferUsageFlagBits;
enum VkShaderStageFlagBits;
enum VkPrimitiveTopology;
enum VkPolygonMode;
enum VkCullModeFlagBits;
enum VkVertexInputRate;
enum VkImageUsageFlagBits;
enum VkImageTiling;
enum VkImageAspectFlagBits;
enum VmaMemoryUsage;
enum VkDescriptorType;
enum VkIndexType;
enum VkFilter;
enum VkSamplerAddressMode;
enum VkFrontFace;
enum VkBorderColor;
enum VkBlendFactor;
enum VkBlendOp;
enum VkLogicOp;
enum VkResult;
enum VkSharingMode;
enum VkSamplerMipmapMode;
enum VkResolveModeFlagBits;

struct VmaAllocation_T;
struct VkBuffer_T;
struct VkImage_T;
struct VkImageView_T;
struct VkSampler_T;
struct VkDescriptorSet_T;

namespace Lina
{
	enum class CommandBufferLevel
	{
		Primary,
		Secondary,
	};

	enum class CommandPoolFlags
	{
		Transient,
		Reset,
		Protected,
	};

	enum class ImageLayout
	{
		Undefined,
		General,
		ColorOptimal,
		DepthStencilOptimal,
		DepthStencilReadOnlyOptimal,
		PresentSurface,
		TransferSrcOptimal,
		TransferDstOptimal,
		ShaderReadOnlyOptimal,
		AttachmentOptimal,
	};

	enum class PipelineBindPoint
	{
		Graphics,
		Computer,
	};

	enum class FenceFlags
	{
		None,
		Signaled,
	};

	enum class CommandBufferFlags
	{
		OneTimeSubmit,
		RenderPassContinue,
		SimultaneousUse,
	};

	enum class ImageUsageFlags
	{
		TransferSrc,
		TransferDest,
		Sampled,
		Storage,
		ColorAttachment,
		DepthStencilAttachment,
		TransientAttachment,
		InputAttachment,
		FragmentDensity,
	};

	enum class ImageAspectFlags
	{
		AspectColor,
		AspectDepth,
		AspectStencil,
		AspectMetadata
	};

	enum class AccessFlags
	{
		None,
		ColorAttachmentRead,
		ColorAttachmentWrite,
		DepthStencilAttachmentRead,
		DepthStencilAttachmentWrite,
		TransferRead,
		TransferWrite,
		ShaderRead,
		MemoryRead,
		HostWrite,
	};

	enum class PipelineStageFlags
	{
		TopOfPipe,
		ColorAttachmentOutput,
		EarlyFragmentTests,
		LateFragmentTests,
		Transfer,
		BottomOfPipe,
		FragmentShader,
		Host,
	};

	enum class BufferUsageFlags
	{
		VertexBuffer,
		IndexBuffer,
		UniformBuffer,
		StorageBuffer,
		TransferSrc,
		TransferDst,
		IndirectBuffer,
	};
	enum class DescriptorSetCreateFlags
	{
		None,
		UpdateAfterBind,
		Push
	};

	enum class DescriptorPoolCreateFlags
	{
		None,
		FreeDescriptorSet,
		UpdateAfterBind,
		HostOnly,
	};
	enum class DependencyFlag
	{
		ByRegion,
		DeviceGroup,
	};

	enum class VulkanResult
	{
		Unknown		  = 0,
		OutOfDateKHR  = 1,
		SuboptimalKHR = 2,
		Success		  = 3,
		Timeout,
		NotReady,
		ErrorOutOfHostMemory,
		ErrorOutOfDeviceMemory,
		ErrorDeviceLost,
		ErrorSurfaceLost,
		ErrorFullscreenExclusiveLost,
	};

	enum class DescriptorSetLayoutBindingFlags
	{
		None,
		UpdateAfterBind,
		UpdateUnusedWhilePending,
		PartiallyBound
	};

	enum class DescriptorSetType
	{
		GlobalSet = 0,
		PassSet	  = 1,
	};

	struct BufferCopy
	{
		uint64 destinationOffset = 0;
		uint64 sourceOffset		 = 0;
		uint64 size				 = 0;
	};

	struct ImageSubresourceRange
	{
		uint32 aspectFlags	  = 0;
		uint32 baseMipLevel	  = 0;
		uint32 levelCount	  = 1;
		uint32 baseArrayLayer = 0;
		uint32 layerCount	  = 1;
	};

	struct BufferImageCopy
	{
		uint64				  bufferOffset		= 0;
		uint32				  bufferRowLength	= 0;
		uint32				  bufferImageHeight = 0;
		ImageSubresourceRange imageSubresource;
		Offset3D			  imageOffset;
		Extent3D			  imageExtent;
	};

	struct DefaultMemoryBarrier
	{
		uint32 srcAccessMask = 0;
		uint32 dstAccessMask = 0;
	};

	struct BufferMemoryBarrier
	{
		uint32		srcAccessMask		= 0;
		uint32		dstAccessMask		= 0;
		uint32		srcQueueFamilyIndex = 0;
		uint32		dstQueueFamilyIndex = 0;
		VkBuffer_T* buffer				= nullptr;
		uint64		offset				= 0;
		uint64		size				= 0;
	};

	struct ImageMemoryBarrier
	{
		uint32				  srcAccessMask		  = 0;
		uint32				  dstAccessMask		  = 0;
		ImageLayout			  oldLayout			  = ImageLayout::Undefined;
		ImageLayout			  newLayout			  = ImageLayout::Undefined;
		uint32				  srcQueueFamilyIndex = 0;
		uint32				  dstQueueFamilyIndex = 0;
		VkImage_T*			  img				  = nullptr;
		ImageSubresourceRange subresourceRange;
	};

	struct ClearValue
	{
		Color  clearColor = Color::White;
		bool   isColor	  = false;
		float  depth	  = 1.0f;
		uint32 stencil	  = 0;
	};

	struct Attachment
	{
	public:
		uint32		flags		   = 0;
		Format		format		   = Format::B8G8R8A8_SRGB;
		LoadOp		loadOp		   = LoadOp::Load;
		StoreOp		storeOp		   = StoreOp::Store;
		LoadOp		stencilLoadOp  = LoadOp::DontCare;
		StoreOp		stencilStoreOp = StoreOp::DontCare;
		ImageLayout initialLayout  = ImageLayout::Undefined;
		ImageLayout finalLayout	   = ImageLayout::PresentSurface;
	};

	struct AttachmentReference
	{
		uint32		attachment;
		ImageLayout layout;
	};

	struct AllocatedBuffer
	{
		VkBuffer_T*		 buffer		= nullptr;
		VmaAllocation_T* allocation = nullptr;
	};

	struct AllocatedImage
	{
		VkImage_T*		 image		= nullptr;
		VmaAllocation_T* allocation = nullptr;
	};

	struct VertexInputBinding
	{
		uint32			binding	  = 0;
		uint32			stride	  = 0;
		VertexInputRate inputRate = VertexInputRate::Vertex;
	};

	struct VertexInputDescription
	{
		Vector<VertexInputBinding>	 bindings;
		Vector<VertexInputAttribute> attributes;
		uint32						 flags = 0;
	};

	struct PushConstantRange
	{
		uint32 offset	  = 0;
		uint32 size		  = 0;
		uint32 stageFlags = 0;
	};

	struct MeshPushConstants
	{
		Vector4 data		 = Vector4::Zero;
		Matrix4	renderMatrix = Matrix4::Identity();
	};

	struct SubPassDependency
	{
		uint32 srcSubpass	   = 0;
		uint32 dstSubpass	   = 0;
		uint32 srcStageMask	   = 0;
		uint32 dstStageMask	   = 0;
		uint32 srcAccessMask   = 0;
		uint32 dstAccessMask   = 0;
		uint32 dependencyFlags = 0;
	};

	struct WriteDescriptorSet
	{
		VkBuffer_T*		   buffer		   = nullptr;
		uint64			   offset		   = 0;
		uint64			   range		   = 0;
		VkDescriptorSet_T* dstSet		   = nullptr;
		uint32			   dstBinding	   = 0;
		uint32			   descriptorCount = 1;
		DescriptorType	   descriptorType  = DescriptorType::UniformBuffer;
		VkImageView_T*	   imageView	   = nullptr;
		ImageLayout		   imageLayout	   = ImageLayout::ColorOptimal;
		VkSampler_T*	   sampler		   = nullptr;
	};

	struct ColorBlendAttachmentState
	{
		bool				blendEnable			= false;
		BlendFactor			srcColorBlendFactor = BlendFactor::Zero;
		BlendFactor			dstColorBlendFactor = BlendFactor::Zero;
		BlendOp				colorBlendOp		= BlendOp::Add;
		BlendFactor			srcAlphaBlendFactor = BlendFactor::Zero;
		BlendFactor			dstAlphaBlendFactor = BlendFactor::Zero;
		BlendOp				alphaBlendOp		= BlendOp::Add;
		ColorComponentFlags componentFlags		= ColorComponentFlags::RGBA;
	};

	struct ImageBlit
	{
		Offset3D			  srcOffsets[2];
		ImageSubresourceRange srcRange;
		Offset3D			  dstOffsets[2];
		ImageSubresourceRange dstRange;
	};

	struct RenderingAttachmentInfo
	{
		VkImageView_T* imageView;
		ImageLayout	   imageLayout;
		ResolveMode	   resolveMode;
		VkImageView_T* resolveView;
		ImageLayout	   resolveLayout;
		LoadOp		   loadOp;
		StoreOp		   storeOp;
		ClearValue	   clearValue;
	};

	struct RenderingInfo
	{
		uint32							flags;
		Recti							renderArea;
		uint32							layerCount;
		uint32							viewMask;
		Vector<RenderingAttachmentInfo> colorAttachments;
		RenderingAttachmentInfo			depthAttachment;
		RenderingAttachmentInfo			stencilAttachment;
		bool							useDepthAttachment	 = false;
		bool							useStencilAttachment = false;
	};

	enum MaterialBindFlag
	{
		BindPipeline   = 1 << 0,
		BindDescriptor = 1 << 1,
	};

	extern Format				GetFormatFromVkFormat(VkFormat f);
	extern VkFormat				GetFormat(Format f);
	extern VkFilter				GetFilter(Filter f);
	extern VkSamplerAddressMode GetSamplerAddressMode(SamplerAddressMode m);
	extern VkColorSpaceKHR		GetColorSpace(ColorSpace f);
	extern VkPresentModeKHR		GetPresentMode(PresentMode m);
	extern VkCommandBufferLevel GetCommandBufferLevel(CommandBufferLevel lvl);
	extern uint32				GetCommandPoolCreateFlags(CommandPoolFlags f);
	extern uint32				GetQueueFamilyBit(QueueFamilies f);
	extern VkImageLayout		GetImageLayout(ImageLayout l);
	extern VkPipelineBindPoint	GetPipelineBindPoint(PipelineBindPoint p);
	extern VkAttachmentLoadOp	GetLoadOp(LoadOp p);
	extern VkAttachmentStoreOp	GetStoreOp(StoreOp op);
	extern VkCompareOp			GetCompareOp(CompareOp op);
	extern uint32				GetFenceFlags(FenceFlags f);
	extern uint32				GetCommandBufferFlags(CommandBufferFlags f);
	extern uint32				GetShaderStage(ShaderStage s);
	extern VkPrimitiveTopology	GetTopology(Topology t);
	extern VkPolygonMode		GetPolygonMode(PolygonMode m);
	extern uint32				GetCullMode(CullMode cm);
	extern VkFrontFace			GetFrontFace(FrontFace face);
	extern VkVertexInputRate	GetVertexInputRate(VertexInputRate rate);
	extern uint32				GetImageUsage(ImageUsageFlags usage);
	extern VkImageTiling		GetImageTiling(ImageTiling tiling);
	extern uint32				GetImageAspectFlags(ImageAspectFlags aspectFlags);
	extern uint32				GetAccessFlags(AccessFlags flags);
	extern uint32				GetPipelineStageFlags(PipelineStageFlags flags);
	extern uint32				GetBufferUsageFlags(BufferUsageFlags flags);
	extern VmaMemoryUsage		GetMemoryUsageFlags(MemoryUsageFlags flags);
	extern uint32				GetMemoryPropertyFlags(MemoryPropertyFlags flags);
	extern uint32				GetDescriptorSetCreateFlags(DescriptorSetCreateFlags flags);
	extern uint32				GetDescriptorPoolCreateFlags(DescriptorPoolCreateFlags flags);
	extern VkDescriptorType		GetDescriptorType(DescriptorType type);
	extern VkIndexType			GetIndexType(IndexType type);
	extern VkBorderColor		GetBorderColor(BorderColor col);
	uint32						GetDependencyFlags(DependencyFlag flag);
	VkBlendFactor				GetBlendFactor(BlendFactor factor);
	VkBlendOp					GetBlendOp(BlendOp blendOp);
	uint32						GetColorComponentFlags(ColorComponentFlags flags);
	VkLogicOp					GetLogicOp(LogicOp op);
	VulkanResult				GetResult(int32 result);
	uint32						GetDescriptorLayoutBindingFlags(DescriptorSetLayoutBindingFlags flags);
	VkResolveModeFlagBits		GetResolveModeFlags(ResolveMode mode);
	VkResolveModeFlagBits		GetResolveModeFlags(ResolveMode mode);
	VkSamplerMipmapMode			GetMipmapMode(MipmapMode mode);
	VkSharingMode				GetSharingMode(SharingMode m);

} // namespace Lina

#endif
