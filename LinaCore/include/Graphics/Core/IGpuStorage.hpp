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

#ifndef IGpuStorage_HPP
#define IGpuStorage_HPP

#include "Core/SizeDefinitions.hpp"
#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

namespace Lina
{
	class Texture;
	class Material;
	class Shader;
	struct ShaderByteCode;

	class IGpuStorage
	{
	public:
		IGpuStorage()		   = default;
		virtual ~IGpuStorage() = default;

		virtual void Initilalize() = 0;
		virtual void Shutdown()	   = 0;

		virtual uint32 GenerateMaterial(Material* mat, uint32 existingHandle)										 = 0;
		virtual void   UpdateMaterialProperties(Material* mat, uint32 imageIndex)									 = 0;
		virtual void   UpdateMaterialTextures(Material* mat, uint32 imageIndex, const Vector<uint32>& dirtyTextures) = 0;
		virtual void   DestroyMaterial(uint32 handle)																 = 0;

		virtual uint32 GeneratePipeline(Shader* shader)																									 = 0;
		virtual void   DestroyPipeline(uint32 handle)																									 = 0;
		virtual void   CompileShader(const char* str, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode) = 0;

		virtual uint32 GenerateImage(Texture* txt, uint32 aspectFlags, uint32 imageUsageFlags) = 0;
		virtual uint32 GenerateImageAndUpload(Texture* txt)									   = 0;
		virtual void   DestroyImage(uint32 handle)											   = 0;
	};
} // namespace Lina

#endif
