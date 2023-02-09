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

#include "Graphics/Platform/Vulkan/Core/GPUStorage.hpp"
#include "Graphics/Platform/Vulkan/Objects/Buffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/CommandBuffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/PipelineLayout.hpp"
#include "Graphics/Platform/Vulkan/Objects/Swapchain.hpp"
#include "Graphics/Platform/Vulkan/Core/GfxBackend.hpp"
#include "Graphics/Platform/Vulkan/Core/GfxManager.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Material.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Data/HashSet.hpp"
#include <vulkan/vulkan.h>

namespace Lina
{

	uint32 GPUStorage::GenerateMaterial(Material* mat, uint32 existingHandle)
	{
		if (existingHandle != -1)
			m_materials.GetItemR(existingHandle).descriptorPool.Destroy();

		// Note: no need to mtx lock, this is called from the main thread.
		const uint32	   index   = existingHandle == -1 ? m_materials.AddItem(GeneratedMaterial()) : existingHandle;
		GeneratedMaterial& genData = m_materials.GetItemR(index);

		if (existingHandle == -1)
		{
			genData.uniformBuffer = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = mat->GetTotalPropertySize() == 0 ? sizeof(float) * 10 : mat->GetTotalPropertySize(),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};

			genData.uniformBuffer.Create();
			genData.uniformBuffer.boundSet	   = &genData.descriptorSet;
			genData.uniformBuffer.boundBinding = 0;
			genData.uniformBuffer.boundType	   = DescriptorType::UniformBuffer;
		}

		genData.descriptorPool = DescriptorPool{
			.device		  = m_gfxManager->GetBackend()->GetDevice(),
			.allocationCb = m_gfxManager->GetBackend()->GetAllocationCb(),
			.maxSets	  = 5,
			.flags		  = DescriptorPoolCreateFlags::None,
		};
		genData.descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 1).AddPoolSize(DescriptorType::CombinedImageSampler, 10).Create();

		genData.descriptorSet = DescriptorSet{
			.device	  = m_gfxManager->GetBackend()->GetDevice(),
			.setCount = 1,
			.pool	  = genData.descriptorPool._ptr,
		};

		const auto& textures = mat->GetTextures();
		auto		rm		 = m_gfxManager->GetSystem()->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		auto shader = rm->GetResource<Shader>(mat->GetShaderHandle());

		auto layout = m_shaders.GetItemR(shader->GetGPUHandle()).materialLayout;
		genData.descriptorSet.Create(layout);

		// Update material properties.
		uint8* data		= nullptr;
		size_t dataSize = 0;
		mat->GetPropertyBlob(data, dataSize);

		// Possible materials/shaders can have no properties.
		if (dataSize != 0)
		{
			genData.uniformBuffer.CopyInto(data, dataSize);
			delete[] data;
		}

		// Skip update if nothings there.
		if (dataSize == 0 && textures.empty())
			return index;

		genData.descriptorSet.BeginUpdate();

		if (dataSize != 0)
			genData.descriptorSet.AddBufferUpdate(genData.uniformBuffer, genData.uniformBuffer.size, 0, DescriptorType::UniformBuffer);

		uint32 binding = 0;
		for (const auto& txtProp : textures)
		{
			const uint32 handle = rm->GetResource<Texture>(txtProp.GetValue())->GetGPUHandle();
			auto&		 gpuTxt = m_textures.GetItemR(handle);

			// Binding 0 is reserved to material uniform, so +1
			genData.descriptorSet.AddTextureUpdate(binding + 1, gpuTxt.img._ptrImgView, gpuTxt.sampler._ptr);
			binding++;
		}

		genData.descriptorSet.SendUpdate();
		return index;
	}

	void GPUStorage::UpdateMaterialProperties(Material* mat)
	{
		uint8* data		= nullptr;
		size_t dataSize = 0;
		mat->GetPropertyBlob(data, dataSize);

		// Possible materials/shaders can have no properties.
		if (dataSize != 0)
		{
			m_materials.GetItemR(mat->GetGPUHandle()).uniformBuffer.CopyInto(data, dataSize);
			delete[] data;
		}
	}

	void GPUStorage::UpdateMaterialTextures(Material* mat, const Vector<uint32>& dirtyTextures)
	{
		if (dirtyTextures.empty())
		{
			LINA_WARN("[GPU Storage] -> Can't update material textures of size 0.");
			return;
		}

		auto&			   gpuMat	= m_materials.GetItemR(mat->GetGPUHandle());
		const auto&		   textures = mat->GetTextures();
		auto			   rm		= m_gfxManager->GetSystem()->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		GeneratedMaterial& genData	= m_materials.GetItemR(mat->GetGPUHandle());

		gpuMat.descriptorSet.BeginUpdate();

		for (auto index : dirtyTextures)
		{
			const uint32 handle = rm->GetResource<Texture>(textures[index].GetValue())->GetGPUHandle();
			auto&		 gpuTxt = m_textures.GetItemR(handle);

			// Binding 0 is reserved to material uniform, so +1
			genData.descriptorSet.AddTextureUpdate(index + 1, gpuTxt.img._ptrImgView, gpuTxt.sampler._ptr);
		}

		gpuMat.descriptorSet.SendUpdate();
	}

	DescriptorSet& GPUStorage::GetDescriptor(Material* mat)
	{
		return m_materials.GetItemR(mat->GetGPUHandle()).descriptorSet;
	}

	void GPUStorage::DestroyMaterial(uint32 handle)
	{
		// Note: no need to mtx lock, this is called from the main thread.
		const uint32 index	 = handle;
		auto&		 genData = m_materials.GetItemR(index);
		genData.uniformBuffer.Destroy();
		genData.descriptorPool.Destroy();
		m_materials.RemoveItem(index);
	}

	uint32 GPUStorage::GeneratePipeline(Shader* shader)
	{
		LOCK_GUARD(m_shaderMtx);

		const auto&		 stages	 = shader->GetStages();
		const uint32	 index	 = m_shaders.AddItem(GeneratedShader());
		GeneratedShader& genData = m_shaders.GetItemR(index);

		// Generate byte code.
		for (const auto& [stage, str] : stages)
		{
			auto bc = shader->GetCompiledCode(stage);

			VkShaderModuleCreateInfo vtxCreateInfo = {
				.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.pNext	  = nullptr,
				.codeSize = bc.size() * sizeof(uint32_t),
				.pCode	  = bc.data(),
			};

			VkResult res = vkCreateShaderModule(m_gfxManager->GetBackend()->GetDevice(), &vtxCreateInfo, nullptr, &genData.modulesPtr[stage]);
			if (res != VK_SUCCESS)
			{
				LINA_ERR("[Shader] -> Could not create shader module!");
				return false;
			}
		}

		genData.pipelineLayout.device		= m_gfxManager->GetBackend()->GetDevice();
		genData.pipelineLayout.allocationCb = m_gfxManager->GetBackend()->GetAllocationCb();
		genData.materialLayout.device		= m_gfxManager->GetBackend()->GetDevice();
		genData.materialLayout.allocationCb = m_gfxManager->GetBackend()->GetAllocationCb();

		const Vector<UserBinding>& bindings = shader->GetBindings();
		for (auto& b : bindings)
			genData.materialLayout.AddBinding(b);

		genData.materialLayout.Create();

		genData.pipelineLayout.AddDescriptorSetLayout(m_gfxManager->GetLayout(DescriptorSetType::GlobalSet));
		genData.pipelineLayout.AddDescriptorSetLayout(m_gfxManager->GetLayout(DescriptorSetType::PassSet));
		genData.pipelineLayout.AddDescriptorSetLayout(genData.materialLayout);
		genData.pipelineLayout.Create();

		ColorBlendAttachmentState blendAttachment = ColorBlendAttachmentState{
			.blendEnable		 = true,
			.srcColorBlendFactor = BlendFactor::SrcAlpha,
			.dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
			.colorBlendOp		 = BlendOp::Add,
			.srcAlphaBlendFactor = BlendFactor::One,
			.dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha,
			.alphaBlendOp		 = BlendOp::Add,
			.componentFlags		 = ColorComponentFlags::RGBA,
		};

		const PipelineType pipelineType = shader->GetPipelineType();

		genData.pipeline = Pipeline{
			.swapchainFormat		  = m_gfxManager->GetBackend()->GetSwapchain(LINA_MAIN_SWAPCHAIN)->format,
			.device					  = m_gfxManager->GetBackend()->GetDevice(),
			.allocationCb			  = m_gfxManager->GetBackend()->GetAllocationCb(),
			.pipelineType			  = pipelineType,
			.depthTestEnabled		  = true,
			.depthWriteEnabled		  = true,
			.depthCompareOp			  = CompareOp::LEqual,
			.topology				  = Topology::TriangleList,
			.polygonMode			  = PolygonMode::Fill,
			.blendAttachment		  = blendAttachment,
			.colorBlendLogicOpEnabled = false,
			.colorBlendLogicOp		  = LogicOp::Copy,
		};

		if (pipelineType == PipelineType::Standard)
		{
			genData.pipeline.cullMode  = CullMode::Back;
			genData.pipeline.frontFace = FrontFace::ClockWise;
		}
		else if (pipelineType == PipelineType::NoVertex)
		{
			genData.pipeline.cullMode  = CullMode::Front;
			genData.pipeline.frontFace = FrontFace::AntiClockWise;
		}
		else if (pipelineType == PipelineType::GUI)
		{
			genData.pipeline.cullMode  = CullMode::None;
			genData.pipeline.frontFace = FrontFace::AntiClockWise;
		}

		// m_pipelines[rp].SetShader(this).SetLayout(m_pipelineLayout).SetRenderPass(RenderEngine::Get()->GetRenderer()->GetRenderPass(rp)).Create();
		genData.pipeline.device		  = m_gfxManager->GetBackend()->GetDevice();
		genData.pipeline.allocationCb = m_gfxManager->GetBackend()->GetAllocationCb();
		genData.pipeline.SetLayout(genData.pipelineLayout).Create(genData.modulesPtr);

		return index;
	}

	Pipeline& GPUStorage::GetPipeline(Shader* shader)
	{
		return m_shaders.GetItemR(shader->GetGPUHandle()).pipeline;
	}

	Pipeline& GPUStorage::GetPipeline(Material* material)
	{
		auto rm		= m_gfxManager->GetSystem()->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		auto shader = rm->GetResource<Shader>(material->GetShaderHandle());
		return m_shaders.GetItemR(shader->GetGPUHandle()).pipeline;
	}

	void GPUStorage::DestroyPipeline(uint32 handle)
	{
		const uint32 index	 = handle;
		auto&		 genData = m_shaders.GetItemR(index);

		for (auto& [stage, mod] : genData.modulesPtr)
		{
			vkDestroyShaderModule(m_gfxManager->GetBackend()->GetDevice(), mod, m_gfxManager->GetBackend()->GetAllocationCb());
		}

		genData.pipelineLayout.Destroy();
		genData.pipeline.Destroy();
		genData.materialLayout.Destroy();

		m_shaders.RemoveItem(index);
	}

	uint32 GPUStorage::GenerateImage(Texture* txt, uint32 aspectFlags, uint32 imageUsageFlags)
	{
		LOCK_GUARD(m_textureMtx);
		const uint32	  index	  = m_textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		// Image & sampler
		ImageSubresourceRange range;
		range.aspectFlags = aspectFlags;
		range.levelCount  = txt->GetMipLevels();

		genData.img = Image{
			.vmaAllocator		 = m_gfxManager->GetBackend()->GetVMA(),
			.device				 = m_gfxManager->GetBackend()->GetDevice(),
			.allocationCb		 = m_gfxManager->GetBackend()->GetAllocationCb(),
			.format				 = static_cast<Format>(meta.GetUInt8("Format"_hs)),
			.tiling				 = static_cast<ImageTiling>(meta.GetUInt8("ImageTiling"_hs)),
			.extent				 = ext,
			.imageUsageFlags	 = imageUsageFlags,
			.memoryUsageFlags	 = MemoryUsageFlags::GpuOnly,
			.memoryPropertyFlags = MemoryPropertyFlags::DeviceLocal,
			.subresRange		 = range,
		};

		genData.sampler = Sampler{
			.device		  = m_gfxManager->GetBackend()->GetDevice(),
			.allocationCb = m_gfxManager->GetBackend()->GetAllocationCb(),
			.samplerData  = txt->GetSampler().GetSamplerData(),
		};

		genData.img.Create(true);
		genData.sampler.Create();

		return index;
	}

	uint32 GPUStorage::GenerateImageAndUpload(Texture* txt)
	{
		LOCK_GUARD(m_textureMtx);
		const uint32	  index	  = m_textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		// Image & sampler
		ImageSubresourceRange range;
		range.aspectFlags = GetImageAspectFlags(ImageAspectFlags::AspectColor);
		range.levelCount  = txt->GetMipLevels();

		genData.img = Image{
			.vmaAllocator	 = m_gfxManager->GetBackend()->GetVMA(),
			.device			 = m_gfxManager->GetBackend()->GetDevice(),
			.allocationCb	 = m_gfxManager->GetBackend()->GetAllocationCb(),
			.format			 = static_cast<Format>(meta.GetUInt8("Format"_hs)),
			.tiling			 = static_cast<ImageTiling>(meta.GetUInt8("ImageTiling"_hs)),
			.extent			 = ext,
			.imageUsageFlags = GetImageUsage(ImageUsageFlags::Sampled) | GetImageUsage(ImageUsageFlags::TransferDest),
			.subresRange	 = range,
			.mipLevels		 = txt->GetMipLevels(),
		};

		genData.sampler = Sampler{
			.device		  = m_gfxManager->GetBackend()->GetDevice(),
			.allocationCb = m_gfxManager->GetBackend()->GetAllocationCb(),
			.samplerData  = txt->GetSampler().GetSamplerData(),
		};
		genData.img.Create(true);
		genData.sampler.Create();

		// CPU Buffer
		uint32		bufferSize = ext.width * ext.height * txt->GetChannels();
		const auto& mipmaps	   = txt->GetMipmaps();
		for (const auto& mm : mipmaps)
			bufferSize += mm.width * mm.height * txt->GetChannels();

		genData.cpuBuffer = Buffer{
			.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
			.size		 = bufferSize,
			.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
			.memoryUsage = MemoryUsageFlags::CpuOnly,
		};

		genData.cpuBuffer.Create();

		// Copy data.
		uint32 offset = ext.width * ext.height * txt->GetChannels();
		genData.cpuBuffer.CopyInto(txt->GetPixels(), offset);
		for (const auto& mm : mipmaps)
		{
			const uint32 size = mm.width * mm.height * txt->GetChannels();
			genData.cpuBuffer.CopyIntoPadded(mm.pixels, size, offset);
			offset += size;
		}

		// Write
		GPUCommand command;

		VkImage_T*	 createdImg = genData.img._allocatedImg.image;
		VkBuffer_T*	 createdBuf = genData.cpuBuffer._ptr;
		const uint32 mipLevels	= txt->GetMipLevels();

		command.Record = [this, createdImg, createdBuf, mipLevels, txt](CommandBuffer& cmd) {
			cmd.CMD_ImageTransition(createdImg, ImageLayout::Undefined, ImageLayout::TransferDstOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::TransferWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::Transfer, mipLevels, 0);

			// Copy and transfer to shader read.
			CopyImage(0, createdImg, createdBuf, cmd, txt->GetExtent(), 0, txt->GetMipLevels());

			uint32		mipLevel   = 1;
			uint32		offset	   = txt->GetExtent().width * txt->GetExtent().height * txt->GetChannels();
			const auto& txtMipmaps = txt->GetMipmaps();

			for (auto& mm : txtMipmaps)
			{
				// Transfer to destination optimal.
				cmd.CMD_ImageTransition(
					createdImg, ImageLayout::Undefined, ImageLayout::TransferDstOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::TransferWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::Transfer, mipLevels, mipLevel);

				const Extent3D mipExtent = Extent3D{mm.width, mm.height, 1};
				// Copy and transfer to shader read.
				CopyImage(offset, createdImg, createdBuf, cmd, mipExtent, mipLevel, txt->GetMipLevels());

				offset += mm.width * mm.height * txt->GetChannels();
				mipLevel++;
			}
		};

		command.OnSubmitted = [index, this]() { m_textures.GetItemR(index).cpuBuffer.Destroy(); };

		m_gfxManager->GetGPUUploader().SubmitImmediate(command);
		return index;
	}

	void GPUStorage::DestroyImage(uint32 handle)
	{
		const uint32	  index = handle;
		GeneratedTexture& gen	= m_textures.GetItemR(index);
		gen.img.Destroy();
		gen.cpuBuffer.Destroy();
		gen.sampler.Destroy();
		m_textures.RemoveItem(index);
	}

	VkImage_T* GPUStorage::GetImage(Texture* txt)
	{
		return m_textures.GetItemR(txt->GetGPUHandle()).img._allocatedImg.image;
	}

	VkImageView_T* GPUStorage::GetImageView(Texture* txt)
	{
		return m_textures.GetItemR(txt->GetGPUHandle()).img._ptrImgView;
	}

	void GPUStorage::CopyImage(uint32 offset, VkImage_T* img, VkBuffer_T* buf, const CommandBuffer& cmd, const Extent3D& ext, uint32 baseMip, uint32 totalMipLevels)
	{
		ImageSubresourceRange copySubres = ImageSubresourceRange{
			.aspectFlags	= GetImageAspectFlags(ImageAspectFlags::AspectColor),
			.baseMipLevel	= baseMip,
			.baseArrayLayer = 0,
			.layerCount		= 1,
		};

		BufferImageCopy copyRegion = BufferImageCopy{
			.bufferOffset	   = offset,
			.bufferRowLength   = 0,
			.bufferImageHeight = 0,
			.imageSubresource  = copySubres,
			.imageOffset	   = Offset3D(),
			.imageExtent	   = ext,
		};

		// Copy the buffer into the image
		cmd.CMD_CopyBufferToImage(buf, img, ImageLayout::TransferDstOptimal, {copyRegion});

		// Transfer to shader read.
		cmd.CMD_ImageTransition(img,
								ImageLayout::TransferDstOptimal,
								ImageLayout::ShaderReadOnlyOptimal,
								ImageAspectFlags::AspectColor,
								AccessFlags::TransferWrite,
								AccessFlags::ShaderRead,
								PipelineStageFlags::Transfer,
								PipelineStageFlags::FragmentShader,
								totalMipLevels,
								baseMip);
	}

} // namespace Lina
