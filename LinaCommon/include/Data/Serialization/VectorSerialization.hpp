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

/*
Class: VectorSerialization


Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresVectorSerialization_HPP
#define DataStructuresVectorSerialization_HPP

#include <EASTL/vector.h>
#include <cereal/cereal.hpp>

namespace cereal
{
    //! Serialization for eastl::vectors of arithmetic (but not bool) using binary serialization, if supported
    template <class Archive, class T, class A> inline
        typename std::enable_if<traits::is_output_serializable<BinaryData<T>, Archive>::value
        && std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, void>::type
        CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::vector<T, A> const& vector)
    {
        ar(make_size_tag(static_cast<size_type>(vector.size()))); // number of elements
        ar(binary_data(vector.data(), vector.size() * sizeof(T)));
    }

    //! Serialization for eastl::vectors of arithmetic (but not bool) using binary serialization, if supported
    template <class Archive, class T, class A> inline
        typename std::enable_if<traits::is_input_serializable<BinaryData<T>, Archive>::value
        && std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, void>::type
        CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::vector<T, A>& vector)
    {
        size_type vectorSize;
        ar(make_size_tag(vectorSize));

        vector.resize(static_cast<std::size_t>(vectorSize));
        ar(binary_data(vector.data(), static_cast<std::size_t>(vectorSize) * sizeof(T)));
    }

    //! Serialization for non-arithmetic vector types
    template <class Archive, class T, class A> inline
        typename std::enable_if<(!traits::is_output_serializable<BinaryData<T>, Archive>::value
            || !std::is_arithmetic<T>::value) && !std::is_same<T, bool>::value, void>::type
        CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::vector<T, A> const& vector)
    {
        ar(make_size_tag(static_cast<size_type>(vector.size()))); // number of elements
        for (auto&& v : vector)
            ar(v);
    }
    
    //! Serialization for non-arithmetic vector types
    template <class Archive, class T, class A> inline
        typename std::enable_if<(!traits::is_input_serializable<BinaryData<T>, Archive>::value
            || !std::is_arithmetic<T>::value) && !std::is_same<T, bool>::value, void>::type
        CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::vector<T, A>& vector)
    {
        size_type size;
        ar(make_size_tag(size));

        vector.resize(static_cast<std::size_t>(size));
        for (auto&& v : vector)
            ar(v);
    }

    //! Serialization for bool vector types
    template <class Archive, class A> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::vector<bool, A> const& vector)
    {
        ar(make_size_tag(static_cast<size_type>(vector.size()))); // number of elements
        for (const auto v : vector)
            ar(static_cast<bool>(v));
    }

    //! Serialization for bool vector types
    template <class Archive, class A> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::vector<bool, A>& vector)
    {
        size_type size;
        ar(make_size_tag(size));

        vector.resize(static_cast<std::size_t>(size));
        for (auto v : vector)
        {
            bool b;
            ar(b);
            v = b;
        }
    }
} // namespace cereal

#endif
