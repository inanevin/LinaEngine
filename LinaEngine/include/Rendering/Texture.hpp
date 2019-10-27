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

#pragma once

#ifndef Texture_HPP
#define Texture_HPP

#include "Core/LinaArray.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Core/Common.hpp"
#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine::Graphics
{
	class ArrayBitmap;
	class DDSTexture;

	class Texture
	{

	public:

		Texture() {};
		~Texture();

		void Construct(RenderDevice& deviceIn, const class ArrayBitmap& data, PixelFormat internalPixelFormat, bool generateMipMaps, bool shouldCompress);
		void Construct(RenderDevice& deviceIn, const DDSTexture& ddsTexture);
		void Construct(RenderDevice& deviceIn, const LinaArray<class ArrayBitmap*>& data, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress);

		FORCEINLINE uint32 GetID() const { return m_ID; };
		FORCEINLINE uint32 GetWidth() const { return m_Width; }
		FORCEINLINE uint32 GetHeight() const { return m_Height; }
		FORCEINLINE bool IsCompressed() const { return isCompressed; }
		FORCEINLINE bool HasMipmaps() const { return hasMipMaps; }

	private:
		
	
		RenderDevice* renderDevice = nullptr;
		uint32 m_ID;
		uint32 m_Width;
		uint32 m_Height;
		bool isCompressed = false;
		bool hasMipMaps = true;
	};
}


#endif