/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ArrayBitmap
Timestamp: 4/14/2019 11:59:32 AM

*/

#pragma once

#ifndef ArrayBitmap_HPP
#define ArrayBitmap_HPP

#include "Core/Common.hpp"
#include "Core/DataStructures.hpp"

namespace LinaEngine::Graphics
{
	class ArrayBitmap
	{
	public:

		DISALLOW_COPY_AND_ASSIGN(ArrayBitmap);

		// Param constructors including width, height, pixel array and offsets
		ArrayBitmap(int32 width = 1, int32 m_Heigth = 1);
		ArrayBitmap(int32 width, int32 m_Heigth, int32* pixels);
		ArrayBitmap(int32 width, int32 m_Heigth, int32* pixels, int32 offsetX, int32 offsetY, int32 rowOffset);

		virtual ~ArrayBitmap();

		// Load the bitmap from a file in resources.
		bool Load(const LinaString& fileName);

		// Save the bitmap into a file in resources.
		bool Save(const LinaString& fileName) const;

		// Clr colors.
		void Clear(int32 color);

		// Accessors & Mutators.
		FORCEINLINE int32 GetWidth() const { return m_Width; };
		FORCEINLINE int32 GetHeight() const { return m_Heigth; };

		FORCEINLINE int32 Get(int32 x, int32 y) const
		{
			LINA_CORE_ASSERT((x > 0 && x < m_Width) && (y >= 0 && y < m_Heigth), "Conditions are not map on array bitmap !");
			return m_Pixels[x + y * m_Width];
		};

		FORCEINLINE void Set(int32 x, int32 y, int32 pixel)
		{
			LINA_CORE_ASSERT((x > 0 && x < m_Width) && (y >= 0 && y < m_Heigth), "Conditions are not map on array bitmap !");
			m_Pixels[x + y * m_Width] = pixel;
		};

		FORCEINLINE int32* GetPixelArray() { return m_Pixels; };
		FORCEINLINE const int32* GetPixelArray() const { return m_Pixels; };

	private:

		// Bitmap array properties.
		int32 m_Width;
		int32 m_Heigth;
		int32* m_Pixels;

	private:

		FORCEINLINE uintptr GetPixelsSize() const { return (uintptr)(m_Width*m_Heigth) * sizeof(m_Pixels[0]); }

	};
}


#endif