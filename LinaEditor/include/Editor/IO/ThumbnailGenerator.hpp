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

#include "Common/Data/String.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Map.hpp"

namespace Lina
{
	class Font;
	class Model;
	class Audio;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class ThumbnailGenerator
	{

	public:
		static void					 CreateThumbnailBuffer(RawStream& stream, LinaGX::TextureBuffer b);
		static LinaGX::TextureBuffer GenerateThumbnailForResource(const String& path, TypeID tid);
		static LinaGX::TextureBuffer GenerateThumbnailForTexture(const String& path);
		static LinaGX::TextureBuffer GenerateThumbnailForModel(const String& path);
		static LinaGX::TextureBuffer GenerateThumbnailForMaterial(const String& path);
		static LinaGX::TextureBuffer GenerateThumbnailForAudio(const String& path);
		static LinaGX::TextureBuffer GenerateThumbnailForFont(const String& absPath);
	};

} // namespace Lina::Editor
