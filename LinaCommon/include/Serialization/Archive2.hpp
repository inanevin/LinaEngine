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

#ifndef Archive2_HPP
#define Archive2_HPP
#include "Data/Streams.hpp"
#include <type_traits>
#include <vector>
namespace Lina::Serialization
{
    class OStream;
    class IStream;

    template <typename Ar, typename T> void SaveOther(Ar& ar, std::vector<T>& vec)
    {
        int x = 5;
    }

    template <typename Ar, typename T> void LoadOther(Ar& ar, std::vector<T>& vec)
    {
    }

    template <typename Ar, typename T, typename = void> struct IsSaveOtherDefined : std::false_type
    {
    };

    template <typename Ar, typename T> struct IsSaveOtherDefined<Ar, T, std::void_t<decltype(SaveOther(std::declval<Ar&>(), std::declval<T&>()))>> : std::true_type
    {
    };

    template <typename Ar, typename T, typename = void> struct IsLoadOtherDefined : std::false_type
    {
    };

    template <typename Ar, typename T> struct IsLoadOtherDefined<Ar, T, std::void_t<decltype(LoadOther(std::declval<Ar&>(), std::declval<T&>()))>> : std::true_type
    {
    };

    template <typename T> struct HasSerialize2
    {
    private:
        template <typename U> static constexpr auto          check(int) -> decltype(std::declval<U>().Serialize(std::declval<T>()), std::true_type());
        template <typename U> static constexpr auto          check(long) -> decltype(std::declval<U&>().Serialize(std::declval<T&>()), std::true_type());
        template <typename> static constexpr std::false_type check(...);

    public:
        static constexpr bool value = decltype(check<T>(0))::value;
    };

    template <typename T> struct HasSave2
    {
    private:
        template <typename U> static constexpr auto          check(int) -> typename std::is_same<decltype(std::declval<U>().Save(std::declval<T>())), void>::type;
        template <typename U> static constexpr auto          check(long) -> typename std::is_same<decltype(std::declval<U&>().Save(std::declval<T&>())), void>::type;
        template <typename> static constexpr std::false_type check(...);

    public:
        static constexpr bool value = decltype(check<T>(0))::value;
    };

    template <typename T> struct HasLoad2
    {
    private:
        template <typename U> static constexpr auto          check(int) -> typename std::is_same<decltype(std::declval<U>().Load(std::declval<T>())), void>::type;
        template <typename U> static constexpr auto          check(long) -> typename std::is_same<decltype(std::declval<U&>().Load(std::declval<T&>())), void>::type;
        template <typename> static constexpr std::false_type check(...);

    public:
        static constexpr bool value = decltype(check<T>(0))::value;
    };

    class Archive2Base
    {
    protected:
        template <typename Ar, typename T> void CallSerialize(Ar& ar, T& obj)
        {
            obj.Serialize(ar);
        }

        template <typename Ar, typename T> void CallSave(Ar& ar, T& obj)
        {
            obj.Save(ar);
        }

        template <typename Ar, typename T> void CallLoad(Ar& ar, T& obj)
        {
            obj.Load(ar);
        }
    };

#define FRIEND_ARCHIVE                                                                                                                                                                                                     \
    friend class Serialization::Archive2Base;                                                                                                                                                                              \
    template <typename> friend struct Serialization::HasSerialize2;                                                                                                                                                        \
    template <typename> friend struct Serialization::HasSave2;                                                                                                                                                             \
    template <typename> friend struct Serialization::HasLoad2

    template <typename StreamType> class Archive2 : public Archive2Base
    {
    public:
        uint32 Version = 0;

        inline StreamType& GetStream()
        {
            return m_stream;
        }

        inline void SetStream(StreamType& stream)
        {
            m_stream = stream;
        }

        // Trivial Write
        template <typename U, typename std::enable_if_t<std::is_fundamental_v<U> && std::is_same_v<StreamType, Lina::OStream>, int> = 0> void Serialize(U& arg)
        {
            m_stream << arg;
        }

        // Trivial Read
        template <typename U, typename std::enable_if_t<std::is_fundamental_v<U> && std::is_same_v<StreamType, Lina::IStream>, int> = 0> void Serialize(U& arg)
        {
            m_stream >> arg;
        }

        // Object Serialize only
        template <typename U, typename std::enable_if_t<!std::is_fundamental_v<U> && HasSerialize2<U>::value, int> = 0> void Serialize(U& arg)
        {
            CallSerialize(*this, arg);
        }

        // User write
        template <typename U, typename std::enable_if_t<!std::is_fundamental_v<U> && !HasSerialize2<U>::value && HasSave2<U>::value && HasLoad2<U>::value && std::is_same_v<StreamType, Lina::OStream>, int> = 0>
        void Serialize(U& arg)
        {
            CallSave(*this, arg);
        }

        // User read
        template <typename U, typename std::enable_if_t<!std::is_fundamental_v<U> && !HasSerialize2<U>::value && HasSave2<U>::value && HasLoad2<U>::value && std::is_same_v<StreamType, Lina::IStream>, int> = 0>
        void Serialize(U& arg)
        {
            CallLoad(*this, arg);
        }

        // Others write
        template <typename U, typename std::enable_if_t<!std::is_fundamental_v<U> && !HasSerialize2<U>::value && !HasSave2<U>::value && !HasLoad2<U>::value && std::is_same_v<StreamType, Lina::IStream>, int> = 0>
        void Serialize(U& arg)
        {
            LoadOther(*this, arg);
        }

        // Others read
        template <typename U, typename std::enable_if_t<!std::is_fundamental_v<U> && !HasSerialize2<U>::value && !HasSave2<U>::value && !HasLoad2<U>::value && std::is_same_v<StreamType, Lina::OStream>, int> = 0>
        void Serialize(U& arg)
        {
            SaveOther(*this, arg);
        }

        template <typename U> void Serialize_Impl(U& arg)
        {
            static_assert(std::is_fundamental_v<U> || HasSerialize2<U>::value || (HasSave2<U>::value && HasLoad2<U>::value) ||
                              (IsSaveOtherDefined<Archive<StreamType>, U>::value && IsLoadOtherDefined<Archive<StreamType>, U>::value),
                          "Serialize or Save/Load functions not found!");

            Serialize(arg);
        }

        template <typename U> void Serialize_Impl(U&& arg)
        {
            static_assert(std::is_fundamental_v<U> || HasSerialize2<U>::value || (HasSave2<U>::value && HasLoad2<U>::value) ||
                              (IsSaveOtherDefined<Archive<StreamType>, U>::value && IsLoadOtherDefined<Archive<StreamType>, U>::value),
                          "Serialize or Save/Load functions not found!");

            Serialize(arg);
        }

        template <class... Types> inline Archive2& operator()(Types&&... args)
        {
            (Serialize_Impl(std::forward<Types>(args)), ...);
            return *this;
        }

    private:
        StreamType m_stream;
    };
} // namespace Lina::Serialization

#endif
