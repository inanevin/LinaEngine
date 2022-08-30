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

#pragma once

#ifndef Audio_HPP
#define Audio_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Core/Resource.hpp"
#include "Data/Map.hpp"
#include <Data/String.hpp>

namespace Lina::Audio
{
    class Audio : public Resources::Resource
    {
        struct AssetData
        {
            int dummy = 0;
        };

    public:
        Audio() = default;
        virtual ~Audio();

        virtual Resource* LoadFromMemory(const IStream& stream) override;
        virtual Resource* LoadFromFile(const String& path) override;
        virtual void      LoadAssetData() override;
        virtual void      SaveAssetData() override;

        unsigned int GetBuffer()
        {
            return m_buffer;
        }

    private:
        static void CheckForError();

    private:
        AssetData    m_assetData;
        int          m_size   = 0;
        int          m_format = 0;
        unsigned int m_buffer = 0;
        float        m_freq   = 0.0f;
        void*        m_data   = nullptr;
    };
} // namespace Lina::Audio

#endif
