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

#ifndef ArchiveCommon_HPP
#define ArchiveCommon_HPP

#include "Data/String.hpp"
#include "Data/Streams.hpp"
#include "Utility/StringId.hpp"
#include <type_traits>
#include <vector>

namespace Lina::Serialization
{

#define SERIALIZATION_LINEARBLOCK_SID TO_SIDC("Lina_SerializationLinearBlock")

    /* Example Specialization for custom 3rd party types
    template <typename Ar, typename T> void SaveOther(Ar& ar, std::vector<T>& vec)
    {
    }
    template <typename Ar, typename T> void LoadOther(Ar& ar, std::vector<T>& vec)
    {
    }
    template <typename Ar, typename T, typename K> void SaveOther(Ar& ar, std::unordered_map<T, K>& vec)
    {
    }
    template <typename Ar, typename T, typename K> void LoadOther(Ar& ar, std::unordered_map<T, K>& vec)
    {
    }
    */

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

    template <typename T> struct HasSerialize
    {
    private:
        template <typename U> static constexpr auto          check(int) -> decltype(std::declval<U>().Serialize(std::declval<T>()), std::true_type());
        template <typename U> static constexpr auto          check(long) -> decltype(std::declval<U&>().Serialize(std::declval<T&>()), std::true_type());
        template <typename> static constexpr std::false_type check(...);

    public:
        static constexpr bool value = decltype(check<T>(0))::value;
    };

    template <typename T> struct HasSave
    {
    private:
        template <typename U> static constexpr auto          check(int) -> typename std::is_same<decltype(std::declval<U>().Save(std::declval<T>())), void>::type;
        template <typename U> static constexpr auto          check(long) -> typename std::is_same<decltype(std::declval<U&>().Save(std::declval<T&>())), void>::type;
        template <typename> static constexpr std::false_type check(...);

    public:
        static constexpr bool value = decltype(check<T>(0))::value;
    };

    template <typename T> struct HasLoad
    {
    private:
        template <typename U> static constexpr auto          check(int) -> typename std::is_same<decltype(std::declval<U>().Load(std::declval<T>())), void>::type;
        template <typename U> static constexpr auto          check(long) -> typename std::is_same<decltype(std::declval<U&>().Load(std::declval<T&>())), void>::type;
        template <typename> static constexpr std::false_type check(...);

    public:
        static constexpr bool value = decltype(check<T>(0))::value;
    };

    class ArchiveBase
    {
    public:
        ArchiveBase()          = default;
        virtual ~ArchiveBase() = default;

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

#define FRIEND_ARCHIVE                                                                                                                                                                                                                                             \
    friend class Serialization::ArchiveBase;                                                                                                                                                                                                                       \
    template <typename> friend struct Serialization::HasSerialize;                                                                                                                                                                                                 \
    template <typename> friend struct Serialization::HasSave;                                                                                                                                                                                                      \
    template <typename> friend struct Serialization::HasLoad

} // namespace Lina::Serialization

#endif
