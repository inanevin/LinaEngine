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
Class: ResourceHandle



Timestamp: 12/30/2021 9:37:53 PM
*/

#pragma once

#ifndef ResourceHandle_HPP
#define ResourceHandle_HPP

// Headers here.
#include "Resources/ResourceStorage.hpp"
#include <cereal/access.hpp>

namespace Lina::Resources
{
    template <typename T>
    class ResourceHandle
    {
    public:
        StringIDType m_sid   = 0;
        T*           m_value = nullptr;

    private:
        friend class cereal::access;

        TypeID m_typeID = 0;

        template <class Archive>
        void save(Archive& archive) const
        {
            archive(m_sid, m_typeID);
        }

        template <class Archive>
        void load(Archive& archive)
        {
            archive(m_sid, m_typeID);

           // if (m_typeID == 0 && m_sid == 0)
           // {
           //     m_typeID = GetTypeID<T>();
           // }
           // else
           // {
           //     m_value = Resources::ResourceStorage::Get()->GetResource<T>(m_sid);
           // }
        }
    };

} // namespace Lina::Resources

#endif
