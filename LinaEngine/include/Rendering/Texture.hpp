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

#include "DDSTexture.hpp"

namespace LinaEngine::Graphics
{
	template<class Derived>
	class RenderEngine;


	class Texture
	{

	public:


		

		DISALLOW_COPY_AND_ASSIGN(Texture);

		Texture(RenderEngine<class PAMRenderEngine>& engine, const class ArrayBitmap& data, int internalPixelFormat, bool generateMipMaps, bool shouldCompress);
		Texture(RenderEngine<class PAMRenderEngine>& engine, const DDSTexture& ddsTexture);

		~Texture();

		FORCEINLINE uint32 GetId() { return m_ID; };
		FORCEINLINE uint32 GetWidth() const { return m_Width; }
		FORCEINLINE uint32 GetHeight() const { return m_Height; }
		FORCEINLINE bool IsCompressed() const { return isCompressed; }
		FORCEINLINE bool HasMipmaps() const { return hasMipMaps; }

	private:

		RenderEngine<class PAMRenderEngine>* renderEngine;
		uint32 m_ID;
		uint32 m_Width;
		uint32 m_Height;
		bool isCompressed;
		bool hasMipMaps;
	};
}


#endif