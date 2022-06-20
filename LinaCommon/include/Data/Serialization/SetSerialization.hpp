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
#ifndef DataStructuresSetSerialization_HPP
#define DataStructuresSetSerialization_HPP

#include "cereal/cereal.hpp"
#include <EASTL/set.h>
#include <EASTL/hash_set.h>

namespace cereal
{
    namespace set_detail
    {
        //! @internal
        template <class Archive, class SetT> inline
            void save(Archive& ar, SetT const& set)
        {
            ar(make_size_tag(static_cast<size_type>(set.size())));

            for (const auto& i : set)
                ar(i);
        }

        //! @internal
        template <class Archive, class SetT> inline
            void load(Archive& ar, SetT& set)
        {
            size_type size;
            ar(make_size_tag(size));

            set.clear();

            auto hint = set.begin();
            for (size_type i = 0; i < size; ++i)
            {
                typename SetT::key_type key;

                ar(key);
#ifdef CEREAL_OLDER_GCC
                hint = set.insert(hint, eastl::move(key));
#else // NOT CEREAL_OLDER_GCC
                hint = set.emplace_hint(hint, eastl::move(key));
#endif // NOT CEREAL_OLDER_GCC
            }
        }
    }

    //! Saving for eastl::set
    template <class Archive, class K, class C, class A> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::set<K, C, A> const& set)
    {
        set_detail::save(ar, set);
    }

    //! Loading for eastl::set
    template <class Archive, class K, class C, class A> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::set<K, C, A>& set)
    {
        set_detail::load(ar, set);
    }

    //! Saving for eastl::multiset
    template <class Archive, class K, class C, class A> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::multiset<K, C, A> const& multiset)
    {
        set_detail::save(ar, multiset);
    }

    //! Loading for eastl::multiset
    template <class Archive, class K, class C, class A> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::multiset<K, C, A>& multiset)
    {
        set_detail::load(ar, multiset);
    }

    //! Saving for eastl::hashset
    template <class Archive, class K, class C, class A> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::hash_set<K, C, A> const& multiset)
    {
        set_detail::save(ar, multiset);
    }

    //! Loading for eastl::hashset
    template <class Archive, class K, class C, class A> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::hash_set<K, C, A>& multiset)
    {
        set_detail::load(ar, multiset);
    }
} // namespace cereal

#endif // CEREAL_TYPES_SET_HPP_
