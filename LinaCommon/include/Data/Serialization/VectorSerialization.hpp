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
