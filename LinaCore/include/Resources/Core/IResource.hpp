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

#ifndef IResource_HPP
#define IResource_HPP

#include "Core/StringID.hpp"
#include "Data/String.hpp"
#include "Serialization/ISerializable.hpp"

namespace Lina
{
    class IResource : public ISerializable
    {
    public:
        IResource()          = default;
        virtual ~IResource() = default;
        virtual void Flush() = 0;

        inline void SetPath(const String& path)
        {
            m_path = path;
        }

        inline void SetSID(StringID sid)
        {
            m_sid = sid;
        }

        inline void SetTID(TypeID tid)
        {
            m_tid = tid;
        }

        inline void SetCacheID(uint32 id)
        {
            m_cacheID = id;
        }

        inline const String& GetPath() const
        {
            return m_path;
        }

        inline StringID GetSID() const
        {
            return m_sid;
        }

        inline TypeID GetTID() const
        {
            return m_tid;
        }

        inline uint32 GetCacheID() const
        {
            return m_cacheID;
        }

    protected:
        uint32   m_cacheID = 0;
        String   m_path    = "";
        TypeID   m_tid     = 0;
        StringID m_sid     = 0;
    };
} // namespace Lina

#endif
