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
#include "Rendering/Texture.hpp"  
#include "Rendering/ArrayBitmap.hpp"
#include "PackageManager/PAMRenderEngine.hpp"

namespace LinaEngine::Graphics
{
	

	Texture::Texture(RenderEngine<PAMRenderEngine>& engine, const ArrayBitmap & data, int internalPixelFormat, bool generateMipMaps, bool shouldCompress)
	{
		renderEngine = &engine;
		m_ID = renderEngine->CreateTexture2D(data.GetWidth(), data.GetHeight(), data.GetPixelArray(), PixelFormat::FORMAT_RGBA, static_cast<PixelFormat>(internalPixelFormat), generateMipMaps, shouldCompress);
		m_Width = (uint32)data.GetWidth();
		m_Height = (uint32)data.GetHeight();
		isCompressed = shouldCompress;
		hasMipMaps = generateMipMaps;
	}

	Texture::Texture(RenderEngine<PAMRenderEngine>& engine, const DDSTexture & ddsTexture)
	{
		renderEngine = &engine;
		m_ID = renderEngine->CreateDDSTexture2D(ddsTexture.GetWidth(), ddsTexture.GetHeight(), ddsTexture.GetBuffer(), ddsTexture.GetFourCC(), ddsTexture.GetMipMapCount());
		m_Width = ddsTexture.GetWidth();
		m_Height = ddsTexture.GetHeight();
		isCompressed = true;
		hasMipMaps = ddsTexture.GetMipMapCount() > 1;
	}

	Texture::~Texture()
	{
		//m_ID = renderDevice->ReleaseTexture2D(m_ID);
	}
}

