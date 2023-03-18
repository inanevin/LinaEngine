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

#ifndef TextureSampler_HPP
#define TextureSampler_HPP

#include "Resources/Core/IResource.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

namespace Lina
{
	class Renderer;

	class TextureSampler : public IResource
	{
	public:
		TextureSampler(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid);
		virtual ~TextureSampler();

		void SetSamplerData(const SamplerData& data);

		inline const SamplerData& GetSamplerData()
		{
			return m_samplerData;
		}

		inline uint32 GetGPUHandle()
		{
			return m_gpuHandle;
		}

	protected:
		// Inherited via IResource
		virtual void Upload() override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
	private:
		uint32		m_gpuHandle = 0;
		Renderer*	m_renderer	= nullptr;
		SamplerData m_samplerData;
	};
} // namespace Lina

#endif