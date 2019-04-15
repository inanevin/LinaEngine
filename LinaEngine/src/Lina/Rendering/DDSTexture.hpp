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



namespace LinaEngine::Graphics
{
	class DDSTexture
	{
	public:

		DDSTexture() {};
		virtual ~DDSTexture() { CleanUp(); };
		bool Load(const char* fileName);
		FORCEINLINE uint32 GetMipMapCount() const { return m_MipMapCount; }
		FORCEINLINE uint32 GetFourCC() const { return m_FourCC; }
		FORCEINLINE uint32 GetWidth() const { return m_Width; }
		FORCEINLINE uint32 GetHeight() const { return m_Height; }
		FORCEINLINE const unsigned char* GetBuffer()  const { return m_Buffer; }

	private:

		unsigned char* m_Buffer = nullptr;
		uint32 m_Height;
		uint32 m_Width;
		uint32 m_MipMapCount;
		uint32 m_FourCC;

		void CleanUp();
	};
}


#endif