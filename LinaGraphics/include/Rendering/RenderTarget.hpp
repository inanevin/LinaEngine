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

/*
Class: RenderTarget

Represents a frame buffer object created on the GPU backend.

Timestamp: 4/26/2019 9:13:05 PM
*/

#pragma once

#ifndef RenderTarget_HPP
#define RenderTarget_HPP

#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
	class Texture;

	class RenderTarget
	{
	public:

		RenderTarget() {}
		~RenderTarget();

		void Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, bool noReadWrite = false, uint32 attachmentNumber = 0, uint32 mipLevel = 0);
		void Construct(Texture& texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, FrameBufferAttachment rboAttachment, uint32 rbo, uint32 attachmentNumber = 0, uint32 mipLevel = 0);
		void Construct(FrameBufferAttachment rboAttachment, uint32 rbo);
		void CheckCompressed(const Texture& texture);

		uint32 GetID() { return m_engineBoundID; }

	private:

		bool m_constructed = false;
		uint32 m_engineBoundID = 0;
		uint32 m_rbo = 0;

	};
}


#endif