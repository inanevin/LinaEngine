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

#ifndef Archive_HPP
#define Archive_HPP

#include <Data/Streams.hpp>
#include <type_traits>

namespace Lina::Serialization
{

    template <typename T>
    inline void Serialize_BasicType(OStream& stream, T& u)
    {
        stream << u;
    };

    template <typename T>
    inline void Serialize_BasicType(IStream& stream, T& u)
    {
        stream >> u;
    };

    template <class Ar, typename T>
    struct Serialize_NonTrivial
    {
        void Serialize(Ar& ar, T& obj)
        {
            LINA_ERR("You are missing a serialize specialization function! {0}", typeid(T).name());
        }
    };

    // https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
    // Primary template with a static assertion
    // for a meaningful error message
    // if it ever gets instantiated.
    // We could leave it undefined if we didn't care.
    template <typename, typename T>
    struct HasSerialize
    {
        static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
    };

    template <typename, typename T>
    struct HasSave
    {
        static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
    };

    template <typename, typename T>
    struct HasLoad
    {
        static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
    };

    // specialization that does the checking
    template <typename C, typename Ret, typename... Args>
    struct HasSerialize<C, Ret(Args...)>
    {
    private:
        template <typename T>
        static constexpr auto check(T*) -> typename std::is_same<decltype(std::declval<T>().Serialize(std::declval<Args>()...)), Ret>::type;

        template <typename>
        static constexpr std::false_type check(...);
        typedef decltype(check<C>(0))    type;

    public:
        static constexpr bool value = type::value;
    };

    template <typename C, typename Ret, typename... Args>
    struct HasSave<C, Ret(Args...)>
    {
    private:
        template <typename T>
        static constexpr auto check(T*) -> typename std::is_same<decltype(std::declval<T>().Save(std::declval<Args>()...)), Ret>::type;

        template <typename>
        static constexpr std::false_type check(...);
        typedef decltype(check<C>(0))    type;

    public:
        static constexpr bool value = type::value;
    };

    template <typename C, typename Ret, typename... Args>
    struct HasLoad<C, Ret(Args...)>
    {
    private:
        template <typename T>
        static constexpr auto check(T*) -> typename std::is_same<decltype(std::declval<T>().Load(std::declval<Args>()...)), Ret>::type;

        template <typename>
        static constexpr std::false_type check(...);
        typedef decltype(check<C>(0))    type;

    public:
        static constexpr bool value = type::value;
    };

    template <typename T, typename U>
    typename std::enable_if<HasSerialize<T, void(U& u)>::value>::type
    SerializeComplex(T& obj, U& archive)
    {
        obj.Serialize(archive);
    }

    template <typename T, typename U>
    typename std::enable_if<!HasSerialize<T, void(U& u)>::value>::type
    SerializeComplex(T& obj, U& archive)
    {
        Serialize_NonTrivial<U, T> a;
        a.Serialize(archive, obj);
    }

    template <typename StreamType>
    class Archive
    {
    public:

        template <typename T>
        void Serialize_Impl(T& arg)
        {
            if (std::is_class<T>::value)
            {
                SerializeComplex(arg, *this);
                return;
            }

            Serialize_BasicType(m_stream, arg);
        }

        template <class... Types>
        void Serialize_T(Types&&... args)
        {
            (Serialize_Impl(args), ...);
        }

        template <class... Types>
        inline Archive& operator()(Types&&... args)
        {
            Serialize_T(std::forward<Types>(args)...);
            return *this;
        }

        StreamType m_stream;
        uint32     m_version = 0;
    };

} // namespace Lina::Serialization

#endif
