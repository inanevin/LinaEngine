/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: DDSTexture
Timestamp: 4/14/2019 4:04:34 PM

*/

#pragma once

#ifndef DDSTexture_HPP
#define DDSTexture_HPP

#include "Core/Common.hpp"
#include "Core/SizeDefinitions.hpp"

namespace LinaEngine::Graphics
{
	class DDSTexture
	{
	public:

		DDSTexture() {};
		~DDSTexture() { CleanUp(); };

		// Loads a .dds texture from resources.
		bool Load(const char* fileName);

		// Accessors & mutators.
		FORCEINLINE uint32 GetMipMapCount() const { return m_MipMapCount; }
		FORCEINLINE uint32 GetFourCC() const { return m_FourCC; }
		FORCEINLINE uint32 GetWidth() const { return m_Width; }
		FORCEINLINE uint32 GetHeight() const { return m_Height; }
		FORCEINLINE const unsigned char* GetBuffer()  const { return m_Buffer; }

	private:

		// Cleans up buffer & mem.
		void CleanUp();

	private:

		// Texture data buffer.
		unsigned char* m_Buffer = nullptr;

		// Texture properties.
		uint32 m_Height = 0;
		uint32 m_Width = 0;
		uint32 m_MipMapCount = 0;
		uint32 m_FourCC = 0;

	};
}


#endif