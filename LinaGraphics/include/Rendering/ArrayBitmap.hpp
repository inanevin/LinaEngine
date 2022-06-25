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

#include <Data/String.hpp>

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
        int Load(const String& fileName);

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
        void Save(const String& path, int quality = 100);

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
