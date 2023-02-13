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

#include "Graphics/Platform/DirectX12/Core/D12GpuStorage.hpp"
#include "Graphics/Resource/Texture.hpp"

namespace Lina
{
	uint32 D12GpuStorage::GenerateMaterial(Material* mat, uint32 existingHandle)
	{
		LOCK_GUARD(m_shaderMtx);

		// Note: no need to mtx lock, this is called from the main thread.
		const uint32	   index   = existingHandle == -1 ? m_materials.AddItem(GeneratedMaterial()) : existingHandle;
		GeneratedMaterial& genData = m_materials.GetItemR(index);

		return index;
	}

	void D12GpuStorage::UpdateMaterialProperties(Material* mat, uint32 imageIndex)
	{
	}

	void D12GpuStorage::UpdateMaterialTextures(Material* mat, uint32 imageIndex, const Vector<uint32>& dirtyTextures)
	{
	}

	void D12GpuStorage::DestroyMaterial(uint32 handle)
	{
		// Note: no need to mtx lock, this is called from the main thread.
		const uint32 index	 = handle;
		auto&		 genData = m_materials.GetItemR(index);

		m_materials.RemoveItem(index);
	}

	uint32 D12GpuStorage::GeneratePipeline(Shader* shader)
	{
		LOCK_GUARD(m_shaderMtx);
		
		const uint32 index	 = m_shaders.AddItem(GeneratedShader());
		auto&		 genData = m_materials.GetItemR(index);

		return index;
	}

	void D12GpuStorage::DestroyPipeline(uint32 handle)
	{
		const uint32 index	 = handle;
		auto&		 genData = m_shaders.GetItemR(index);

		m_shaders.RemoveItem(index);
	}

	uint32 D12GpuStorage::GenerateImage(Texture* txt, uint32 aspectFlags, uint32 imageUsageFlags)
	{
		LOCK_GUARD(m_textureMtx);

		const uint32	  index	  = m_textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		return index;
	}

	uint32 D12GpuStorage::GenerateImageAndUpload(Texture* txt)
	{
		LOCK_GUARD(m_textureMtx);
		
		const uint32	  index	  = m_textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		return index;
	}

	void D12GpuStorage::DestroyImage(uint32 handle)
	{
		const uint32	  index = handle;
		GeneratedTexture& gen	= m_textures.GetItemR(index);

		m_textures.RemoveItem(index);
	}

} // namespace Lina
