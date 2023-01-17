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

#ifndef DockSetup_HPP
#define DockSetup_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Math/Vector.hpp"
#include "Data/Vector.hpp"
#include "Resource/Core/Resource.hpp"
#include "Serialization/ArchiveCommon.hpp"
#include "Serialization/VectorSerialization.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina::Editor
{
    class DockSetup : public Resources::Resource
    {
    public:
        struct DockedWindowData
        {
            Vector2  localPos  = Vector2::Zero;
            Vector2  localSize = Vector2::Zero;
            StringID sid       = 0;

            template <typename Ar> void Serialize(Ar& ar)
            {
                ar(sid, localPos, localSize);
            }
        };

        struct DockSwapchain
        {
            String                   swapchainName = "";
            Vector<DockedWindowData> windows;
            Vector2i                 size = Vector2i::Zero;
            Vector2i                 pos  = Vector2i::Zero;

            template <typename Ar> void Serialize(Ar& ar)
            {
                ar(swapchainName, windows, size, pos);
            }
        };

        inline const Vector<DockSwapchain>& GetSwapchains()
        {
            return m_dockSwapchains;
        }

    protected:
        virtual Resource* LoadFromFile(const char* path) override;
        virtual Resource* LoadFromMemory(Serialization::Archive<IStream>& archive) override;
        virtual void      WriteToPackage(Serialization::Archive<OStream>& archive) override;

    private:
        FRIEND_ARCHIVE;

        template <typename Ar> void Serialize(Ar& ar)
        {
            ar(m_dockSwapchains);
        }

    private:
        Vector<DockSwapchain> m_dockSwapchains;
    };
} // namespace Lina::Editor

#endif
