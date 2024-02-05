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

#ifndef Shader_HPP
#define Shader_HPP

#include "Core/Resources/Resource.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Data/Streams.hpp"

namespace Lina
{
	struct ShaderVariant
	{
		uint32				  gpuHandle	   = 0;
		String				  name		   = "";
		String				  passName	   = "";
		bool				  blendDisable = false;
		bool				  depthDisable = false;
		ShaderVariantPassType passType	   = ShaderVariantPassType::RenderTarget;
		LinaGX::CullMode	  cullMode	   = LinaGX::CullMode::Back;
		LinaGX::FrontFace	  frontFace	   = LinaGX::FrontFace::CCW;

		void SaveToStream(OStream& stream);
		void LoadFromStream(IStream& stream);
	};

	class Shader : public Resource
	{
	public:
		Shader(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : Resource(rm, isUserManaged, path, sid, GetTypeID<Shader>()){};
		virtual ~Shader();

		inline uint32 GetGPUHandle() const
		{
			return m_variants.begin()->second.gpuHandle;
		}

		inline uint32 GetGPUHandle(StringID variant) const
		{
			return m_variants.at(variant).gpuHandle;
		}

		inline uint32 GetGPUHandle(ShaderVariantPassType passType) const
		{
			for (const auto& [sid, var] : m_variants)
			{
				if (var.passType == passType)
					return var.gpuHandle;
			}

			return m_variants.begin()->second.gpuHandle;
		}

		inline const LinaGX::ShaderLayout& GetLayout() const
		{
			return m_layout;
		}

	protected:
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void BatchLoaded() override;
		virtual void Flush() override;

	private:
	private:
		LINAGX_MAP<LinaGX::ShaderStage, LinaGX::DataBlob> m_outCompiledBlobs;
		LinaGX::ShaderLayout							  m_layout = {};
		HashMap<StringID, ShaderVariant>				  m_variants;
	};

} // namespace Lina

#endif
