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

#include "Utility/Graphics/TexturePacker.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Math/Math.hpp"
#include "Graphics/Utility/stb/stb_image.h"

namespace Lina::Editor
{

    Graphics::Texture* TexturePacker::PackFilesOrdered(Vector<String>& paths, int maxWidth, Vector<PackedTexture>& packed)
    {
        Graphics::Texture* txt = new Graphics::Texture();

        Vector<InFlightPixelData> pixelData;
        const uint32              fileSize = static_cast<uint32>(paths.size());

        // Each row represents (maxWidth, maxHeightForCurrentRow)
        Vector<PackRow> rows;
        rows.push_back(PackRow());

        for (uint32 i = 0; i < fileSize; i++)
        {
            auto& r = rows[rows.size() - 1];

            InFlightPixelData data;
            int               texChannels;

            const String& path = paths[i];

            if (Resources::Resource::MetaArchiveExists(path))
            {
                auto arch = Resources::Resource::GetMetaArchive(path);

                uint32 size = 0;
                arch(size);
                arch(data.width);
                arch(data.height);

                if (size != 0)
                {
                    data.pixels = new unsigned char[size];
                    arch.GetStream().ReadEndianSafe(data.pixels, size);
                }

                arch.GetStream().Destroy();
            }
            else
            {
                data.pixels = stbi_load(path.c_str(), &data.width, &data.height, &texChannels, STBI_rgb_alpha);

                // Save as meta archive.
                Serialization::Archive<OStream> arch;
                const uint32                    size = data.width * data.height * 4;
                arch(size);
                arch(data.width);
                arch(data.height);

                if (size != 0)
                    arch.GetStream().WriteEndianSafe(data.pixels, size);

                Resources::Resource::SaveMetaArchive(arch, paths[i]);
                arch.GetStream().Destroy();
            }

            int previousTotalHeight = 0;
            for (uint32 k = 0; k < rows.size() - 1; k++)
                previousTotalHeight += rows[k].currentY;

            data.bufferOffset = maxWidth * previousTotalHeight * 4 + r.currentX * 4;

            PackedTexture pt;
            pt.name     = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(paths[i]));
            data.startY = previousTotalHeight;
            r.currentX += data.width;

            if (r.currentX == maxWidth)
            {
                r.currentY = Math::Max(r.currentY, data.height);
                rows.push_back(PackRow());

                pt.uvTL.x = static_cast<float>(r.currentX - data.width) / static_cast<float>(maxWidth);
                pt.uvBR.x = static_cast<float>(r.currentX) / static_cast<float>(maxWidth);
            }
            else if (r.currentX > maxWidth)
            {
                r.currentX -= data.width;
                data.bufferOffset = maxWidth * (previousTotalHeight + r.currentY) * 4;
                data.startY       = previousTotalHeight + r.currentY;

                rows.push_back(PackRow());
                auto& newRow    = rows[rows.size() - 1];
                newRow.currentX = data.width;
                newRow.currentY = data.height;

                pt.uvTL.x = 0.0f;
                pt.uvBR.x = static_cast<float>(data.width) / static_cast<float>(maxWidth);
            }
            else
            {
                r.currentY = Math::Max(r.currentY, data.height);
                pt.uvTL.x  = static_cast<float>(r.currentX - data.width) / static_cast<float>(maxWidth);
                pt.uvBR.x  = static_cast<float>(r.currentX) / static_cast<float>(maxWidth);
            }

            pt.size = Vector2(static_cast<float>(data.width), static_cast<float>(data.height));
            pixelData.push_back(data);
            packed.push_back(pt);
        }

        int totalHeight = 0;

        for (auto& r : rows)
            totalHeight += r.currentY;

        for (uint32 i = 0; i < pixelData.size(); i++)
        {
            packed[i].uvTL.y = static_cast<float>(pixelData[i].startY) / static_cast<float>(totalHeight);
            packed[i].uvBR.y = static_cast<float>(pixelData[i].startY + pixelData[i].height) / static_cast<float>(totalHeight);
        }

        rows.clear();

        const int atlasWidth  = maxWidth;
        const int atlasHeight = totalHeight;

        // Generate icon texture.
        Graphics::Sampler sampler = Graphics::Sampler{
            .minFilter         = Graphics::Filter::Linear,
            .magFilter         = Graphics::Filter::Nearest,
            .u                 = Graphics::SamplerAddressMode::ClampToEdge,
            .v                 = Graphics::SamplerAddressMode::ClampToEdge,
            .w                 = Graphics::SamplerAddressMode::ClampToEdge,
            .mipLodBias        = 0.0f,
            .minLod            = 0.0f,
            .maxLod            = 1.0f,
            .anisotropyEnabled = true,
            .maxAnisotropy     = 8.0f,
            .borderColor       = Graphics::BorderColor::FloatOpaqueWhite,
            .mipmapMode        = Graphics::MipmapMode::Linear,
        };
        txt->GenerateCustomBuffers(atlasWidth, atlasHeight, 4, 1, Graphics::Format::R8G8B8A8_SRGB, sampler, Graphics::ImageTiling::Linear);

        // Buffer data
        for (uint32 i = 0; i < fileSize; i++)
        {
            auto&  pd        = pixelData[i];
            uint32 rowOffset = 0;

            for (int k = 0; k < pd.height; k++)
            {
                txt->GetCPUBuffer().CopyIntoPadded(&pd.pixels[pd.width * k * 4], pd.width * 4, rowOffset + pd.bufferOffset);
                rowOffset += atlasWidth * 4;
            }

            stbi_image_free(pd.pixels);
        }

        pixelData.clear();

        // Upload to gpu
        // Might be transferred late, so don't destroy cpu buffer right away
        txt->WriteToGPUImage(Graphics::Offset3D{.x = 0, .y = 0, .z = 0}, txt->GetExtent(), true);

        return txt;
    }
} // namespace Lina::Editor
