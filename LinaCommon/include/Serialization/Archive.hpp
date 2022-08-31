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
    // https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
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

    // Specialize this template to serialize types such as vectors, maps and other external data structures.
    template <class Ar, typename T>
    struct Serialize_NonTrivial
    {
        void Serialize(Ar& ar, T& obj)
        {
            LINA_ERR("You are missing a serialize specialization function! {0}", typeid(T).name());
        }
    };

    // Specialization below, for separate Save & Load functions.
    template <class Archive, typename T>
    struct SaveLoad_Complex
    {
        void Serialize(Archive& archive, T& obj)
        {
            LINA_ERR("You are missing a serialize complex function! {0}", typeid(T).name());
        }
    };

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

    // If the type has Serialize() func.
    template <typename T, typename U>
    typename std::enable_if<HasSerialize<T, void(U& u)>::value>::type
    SerializeComplex(T& obj, U& archive)
    {
        obj.Serialize(archive);
    }

    // If the type has separate Save & Load func.
    template <typename T, typename U>
    typename std::enable_if<!HasSerialize<T, void(U& u)>::value && HasLoad<T, void(U& u)>::value && HasSave<T, void(U& u)>::value>::type
    SerializeComplex(T& obj, U& archive)
    {
        SaveLoad_Complex<U, T> s;
        s.Serialize(archive, obj);
    }

    // If the type doesn't have Serialize, Save or Load, then
    // use external.
    template <typename T, typename U>
    typename std::enable_if<!HasSerialize<T, void(U& u)>::value && !HasLoad<T, void(U& u)>::value && !HasSave<T, void(U& u)>::value>::type
    SerializeComplex(T& obj, U& archive)
    {
        Serialize_NonTrivial<U, T> a;
        a.Serialize(archive, obj);
    }

    template <typename T, typename U>
    typename std::enable_if<std::is_class<T>::value>::type
    SerializeType(T& obj, U& archive)
    {
        SerializeComplex(obj, archive);
    }

    template <typename T, typename U>
    typename std::enable_if<!std::is_class<T>::value>::type
    SerializeType(T& obj, U& archive)
    {
        Serialize_BasicType(archive.GetStream(), obj);
    }

    template <typename StreamType>
    class Archive
    {
    public:
        uint32 Version = 0;

        inline StreamType& GetStream()
        {
            return m_stream;
        }

        template <typename T>
        void Serialize_Impl(T& arg)
        {
            SerializeType(arg, *this);
        }

        template <class... Types>
        inline Archive& operator()(Types&&... args)
        {
            (Serialize_Impl(std::forward<Types>(args)), ...);
            return *this;
        }

    private:
        StreamType m_stream;
    };

    template <typename T>
    struct SaveLoad_Complex<Archive<IStream>, T>
    {
        void Serialize(Archive<IStream>& archive, T& obj)
        {
            obj.Load(archive);
        }
    };

    template <typename T>
    struct SaveLoad_Complex<Archive<OStream>, T>
    {
        void Serialize(Archive<OStream>& archive, T& obj)
        {
            obj.Save(archive);
        }
    };

} // namespace Lina::Serialization

#endif
