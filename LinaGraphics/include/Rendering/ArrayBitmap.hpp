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
Class: ArrayBitmap

Used for loading textures, this class is an intermediate step to hold bitmap data
loaded from the image files. After the data is loaded into memory and given to
GPU backend, its discarded.

Timestamp: 4/14/2019 11:59:32 AM
*/

#pragma once

#ifndef ArrayBitmap_HPP
#define ArrayBitmap_HPP

#include "Core/SizeDefinitions.hpp"

#include <string>

namespace Lina::Graphics
{
    class ArrayBitmap
    {
    public:
        ArrayBitmap() = default;
        ~ArrayBitmap();

        /// <summary>
        /// Sets STBI image flip option.
        /// </summary>
        static void SetImageFlip(bool flip);

        /// <summary>
        /// Load a non-HDRI texture from a file.
        /// </summary>
        int Load(const std::string& fileName);

        /// <summary>
        /// Load a non-HDRI texture from memory.
        /// </summary>
        int Load(unsigned char* data, size_t dataSize);

        /// <summary>
        /// Load an HDRI texture from a file.
        /// </summary>
        int LoadHDRIFromFile(const char* fileName);

        /// <summary>
        /// Load an HDRI texture from memory.
        /// </summary>
        int LoadHDRIFromMemory(unsigned char* data, size_t dataSize);

        /// <summary>
        /// Writes the current texture data to file.
        /// </summary>
        void Save(const std::string& path, int quality = 100);

        // Accessors & Mutators.
        int32 GetWidth() const
        {
            return m_width;
        };
        int32 GetHeight() const
        {
            return m_height;
        };

        void SetPixelData(unsigned char* data)
        {
            m_pixels = data;
        }

        unsigned char* GetPixelArray()
        {
            return m_pixels;
        }

        const unsigned char* GetPixelArray() const
        {
            return m_pixels;
        }

        float* GetHDRIPixelArray()
        {
            return m_hdriPixels;
        }

    private:
        bool           m_isHDRI     = false;
        int            m_width      = 0;
        int            m_height     = 0;
        int            m_numComps   = 0;
        unsigned char* m_pixels     = nullptr;
        float*         m_hdriPixels = nullptr;
    };
} // namespace Lina::Graphics

#endif
