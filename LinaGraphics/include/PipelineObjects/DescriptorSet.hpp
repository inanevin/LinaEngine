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

#ifndef DescriptorSet_HPP
#define DescriptorSet_HPP

#include "Core/GraphicsCommon.hpp"

struct VkDescriptorSet_T;
struct VkDescriptorPool_T;
struct VkDescriptorSetLayout_T;

namespace Lina::Graphics
{
    class DescriptorPool;
    class DescriptorSetLayout;
    class Buffer;
    class Texture;
    class Image;
    class Sampler;

    class DescriptorSet
    {
    public:
        void           Create(const DescriptorSetLayout& layout);

        void BeginUpdate();
        void AddBufferUpdate(Buffer& buffer, size_t range, uint32 binding, DescriptorType type);
        void AddTextureUpdate(uint32 binding, Texture* txt);
        void AddTextureUpdate(uint32 binding, Image* img, Sampler* sampler);
        void SendUpdate();

        static void UpdateDescriptorSets(const Vector<WriteDescriptorSet>& v);

        // Description
        uint32               setCount = 0;
        VkDescriptorPool_T*  pool     = nullptr;
        DescriptorSetLayout* layout;

        // Runtime
        VkDescriptorSet_T*         _ptr = nullptr;
        Vector<WriteDescriptorSet> _writes;
    };
} // namespace Lina::Graphics

#endif
