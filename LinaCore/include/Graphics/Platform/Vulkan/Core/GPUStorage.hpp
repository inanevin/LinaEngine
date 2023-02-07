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

#ifndef GPUStorage_HPP
#define GPUStorage_HPP

#include "Graphics/Platform/Vulkan/Objects/Image.hpp"
#include "Graphics/Platform/Vulkan/Objects/Sampler.hpp"
#include "Graphics/Platform/Vulkan/Objects/Buffer.hpp"
#include "Graphics/Platform/Vulkan/Objects/Pipeline.hpp"
#include "Graphics/Platform/Vulkan/Objects/DescriptorSetLayout.hpp"
#include "Graphics/Platform/Vulkan/Objects/DescriptorPool.hpp"
#include "Graphics/Platform/Vulkan/Objects/DescriptorSet.hpp"
#include "Graphics/Platform/Vulkan/Objects/PipelineLayout.hpp"
#include "Data/IDList.hpp"
#include "Data/Vector.hpp"
#include "Data/Mutex.hpp"

struct VkImage_T;
struct VkBuffer_T;
struct VkShaderModule_T;

namespace Lina
{
	class Texture;
	class Shader;
	class CommandBuffer;
	class Material;

	struct GeneratedTexture
	{
		Image	img;
		Sampler sampler;
		Buffer	cpuBuffer;
	};

	struct GeneratedShader
	{
		Pipeline								pipeline;
		PipelineLayout							pipelineLayout;
		HashMap<ShaderStage, VkShaderModule_T*> modulesPtr;
		DescriptorSetLayout						materialLayout;
	};

	struct GeneratedMaterial
	{
		Buffer		   uniformBuffer;
		DescriptorPool descriptorPool;
		DescriptorSet  descriptorSet;
	};

	class GfxManager;

	class GPUStorage
	{
	public:
#define DEFAULT_TXT_POOL	20
#define DEFAULT_SHADER_POOL 10
#define DEFAULT_MAT_POOL	20

		/************************ ENGINE API ************************/
		GPUStorage(GfxManager* manager) : m_gfxManager(manager), m_textures(DEFAULT_TXT_POOL, GeneratedTexture()), m_shaders(DEFAULT_SHADER_POOL, GeneratedShader()), m_materials(DEFAULT_MAT_POOL, GeneratedMaterial()){};
		virtual ~GPUStorage() = default;
		/************************ ENGINE API ************************/

		uint32 GenerateMaterial(Material* mat, uint32 existingHandle);
		void   UpdateMaterialProperties(Material* mat);
		void   UpdateMaterialTextures(Material* mat, const Vector<uint32>& dirtyTextures);
		void   DestroyMaterial(uint32 handle);

		uint32 GeneratePipeline(Shader* shader);
		void   DestroyPipeline(uint32 handle);

		uint32 GenerateImage(Texture* txt, uint32 aspectFlags, uint32 imageUsageFlags);
		uint32 GenerateImageAndUpload(Texture* txt);
		void   DestroyImage(uint32 handle);

	private:
		void CopyImage(VkImage_T* img, VkBuffer_T* buf, const CommandBuffer& cmd, Texture* txt, uint32 baseMip);

	private:
		GfxManager*				  m_gfxManager = nullptr;
		IDList<GeneratedTexture>  m_textures;
		Mutex					  m_textureMtx;
		IDList<GeneratedShader>	  m_shaders;
		Mutex					  m_shaderMtx;
		IDList<GeneratedMaterial> m_materials;
		Mutex					  m_materialMtx;
	};
} // namespace Lina

#endif
