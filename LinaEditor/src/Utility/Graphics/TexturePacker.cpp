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
#include "Resource/Texture.hpp"
#include "Utility/stb/stb_image.h"

namespace Lina::Editor
{

    Graphics::Texture* TexturePacker::PackFilesOrdered(Vector<String>& paths, float maxWidth, Vector<PackedTexture>& packed)
    {
        Graphics::Texture* txt = new Graphics::Texture();

        Vector<InFlightPixelData> pixelData;
        const uint32              fileSize = static_cast<uint32>(paths.size());

        Vector<PackRow> rows;

        rows.push_back(PackRow());

        for (uint32 i = 0; i < fileSize; i++)
        {
            auto& r = rows[rows.size() - 1];

            InFlightPixelData data;
            int               texChannels;
            data.pixels = stbi_load(paths[i].c_str(), &data.width, &data.height, &texChannels, STBI_rgb_alpha);

            float previousTotalHeight = 0.0f;
            for (uint32 k = 0; k < rows.size() - 1; k++)
                previousTotalHeight += rows[k].currentY;

            data.bufferOffset = maxWidth * previousTotalHeight * 4 + r.currentX * 4;

            PackedTexture pt;
            data.startY = previousTotalHeight;
            r.currentX += data.width;

            if (r.currentX == maxWidth)
            {
                r.currentY = Math::Max(r.currentY, static_cast<float>(data.height));
                rows.push_back(PackRow());

                pt.uvTL.x = (r.currentX - data.width) / maxWidth;
                pt.uvBR.x = r.currentX / maxWidth;
            }
            else if (r.currentX > maxWidth)
            {
                r.currentX -= data.width;
                data.bufferOffset = maxWidth * (previousTotalHeight + r.currentY) * 4;
                data.startY       = previousTotalHeight + r.currentY;

                rows.push_back(PackRow());
                auto& newRow    = rows[rows.size() - 1];
                newRow.currentX = data.width;
                newRow.currentY = static_cast<float>(data.height);

                pt.uvTL.x = 0.0f;
                pt.uvBR.x = data.width / maxWidth;
            }
            else
            {
                r.currentY = Math::Max(r.currentY, static_cast<float>(data.height));
                pt.uvTL.x  = (r.currentX - data.width) / maxWidth;
                pt.uvBR.x  = r.currentX / maxWidth;
            }

            pt.size = Vector2(data.width, data.height);
            pixelData.push_back(data);
            packed.push_back(pt);
        }

        float totalHeight = 0.0f;

        for (auto& r : rows)
            totalHeight += r.currentY;

        for (uint32 i = 0; i < pixelData.size(); i++)
        {
            packed[i].uvTL.y = pixelData[i].startY / totalHeight;
            packed[i].uvBR.y = (pixelData[i].startY + pixelData[i].height) / totalHeight;
        }

        rows.clear();

        const float atlasWidth  = maxWidth;
        const float atlasHeight = totalHeight;

        // Generate icon texture.
        Graphics::Sampler sampler = Graphics::Sampler{
            .minFilter     = Graphics::Filter::Linear,
            .magFilter     = Graphics::Filter::Linear,
            .u             = Graphics::SamplerAddressMode::ClampToEdge,
            .v             = Graphics::SamplerAddressMode::ClampToEdge,
            .w             = Graphics::SamplerAddressMode::ClampToEdge,
            .mipLodBias    = 0.0f,
            .maxAnisotropy = 15.0f,
            .minLod        = 0.0f,
            .maxLod        = 1.0f,
            .borderColor   = Graphics::BorderColor::FloatOpaqueWhite,
        };
        txt->GenerateCustomBuffers(atlasWidth, atlasHeight, 4, Graphics::Format::R8G8B8A8_SRGB, sampler, Graphics::ImageTiling::Linear);

        // Buffer data
        for (uint32 i = 0; i < fileSize; i++)
        {
            auto&  pd        = pixelData[i];
            uint32 rowOffset = 0;

            for (int k = 0; k < pd.height; k++)
            {
                txt->GetCPUBuffer().CopyIntoPadded(&pd.pixels[pd.height * k * 4], pd.width * 4, rowOffset + pd.bufferOffset);
                rowOffset += atlasWidth * 4;
            }

            stbi_image_free(pd.pixels);
        }

        pixelData.clear();

        // Upload to gpu
        txt->WriteToGPUImage(0, nullptr, 0, Graphics::Offset3D{.x = 0, .y = 0, .z = 0}, txt->GetExtent(), true);

        return txt;
    }
} // namespace Lina::Editor
