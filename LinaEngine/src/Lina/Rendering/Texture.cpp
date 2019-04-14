/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Texture
Timestamp: 1/7/2019 1:55:47 PM

*/

#include "LinaPch.hpp"
#include "Texture.hpp"  
#include "ArrayBitmap.hpp"
#include "RenderingEngine.hpp"

namespace LinaEngine
{
	

	Texture::Texture(RenderingEngine& re, const ArrayBitmap & data, RenderingEngine::PixelFormat internalPixelFormat, bool generateMipMaps, bool shouldCompress)
	{
		renderingEngine = &re;
		m_ID = renderingEngine->CreateTexture2D(data.GetWidth(), data.GetHeight(), data.GetPixelArray(), RenderingEngine::PixelFormat::FORMAT_RGBA, internalPixelFormat, generateMipMaps, shouldCompress);
		m_Width = (uint32)data.GetWidth();
		m_Height = (uint32)data.GetHeight();
		isCompressed = shouldCompress;
		hasMipMaps = generateMipMaps;
	}

	Texture::~Texture()
	{
		m_ID = renderingEngine->ReleaseTexture2D(m_ID);
	}
}

