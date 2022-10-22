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

#ifndef MaterialProperty_HPP
#define MaterialProperty_HPP

#include "Core/GraphicsCommon.hpp"
#include "Serialization/Archive.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina::Graphics
{
    class Material;

    class MaterialPropertyBase
    {
    public:
        MaterialPropertyBase()                                               = default;
        virtual ~MaterialPropertyBase()                                      = default;
        virtual uint32 GetTypeSize()                                         = 0;
        virtual void*  GetData()                                             = 0;
        virtual void   SaveToArchive(Serialization::Archive<OStream>& arc)   = 0;
        virtual void   LoadFromArchive(Serialization::Archive<IStream>& arc) = 0;
        virtual void   CopyValueFrom(MaterialPropertyBase* base)             = 0;

        inline MaterialPropertyType GetType()
        {
            return m_type;
        }


        inline String GetName()
        {
            return m_name;
        }

    protected:
        friend class Material;

        String               m_name = "";
        MaterialPropertyType m_type = MaterialPropertyType::Float;
    };

    template <typename T> class MaterialProperty : public MaterialPropertyBase
    {
    public:
        MaterialProperty()          = default;
        virtual ~MaterialProperty() = default;

        virtual void SaveToArchive(Serialization::Archive<OStream>& ar) override
        {
            ar(m_value);
        }

        virtual void LoadFromArchive(Serialization::Archive<IStream>& ar) override
        {
            ar(m_value);
        }

        virtual uint32 GetTypeSize() override
        {
            return static_cast<uint32>(sizeof(T));
        }

        virtual void* GetData() override
        {
            return static_cast<void*>(&m_value);
        }

        void SetValue(T val)
        {
            m_value = val;
        }

        virtual void CopyValueFrom(MaterialPropertyBase* base)
        {
            MaterialProperty<T>* derived = static_cast<MaterialProperty<T>*>(base);
            m_value                      = derived->m_value;
        }

    private:
        T m_value = T();
    };

} // namespace Lina::Graphics

#endif