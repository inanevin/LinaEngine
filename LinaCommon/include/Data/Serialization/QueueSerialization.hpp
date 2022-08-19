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

#ifndef DataStructuresQueueSerialization_HPP
#define DataStructuresQueueSerialization_HPP


#include "cereal/details/helpers.hpp"
#include <EASTL/queue.h>
#include <EASTL/deque.h>
#include <EASTL/priority_queue.h>

// The default comparator for queue is less
#include "cereal/types/functional.hpp"

namespace cereal
{
    namespace queue_detail
    {
        //! Allows access to the protected container in queue
        /*! @internal */
        template <class T, class C> inline
            C const& container(eastl::queue<T, C> const& queue)
        {
            struct H : public eastl::queue<T, C>
            {
                static C const& get(eastl::queue<T, C> const& q)
                {
                    return q.*(&H::c);
                }
            };

            return H::get(queue);
        }

        //! Allows access to the protected container in priority queue
        /*! @internal */
        template <class T, class C, class Comp> inline
            C const& container(eastl::priority_queue<T, C, Comp> const& priority_queue)
        {
            struct H : public eastl::priority_queue<T, C, Comp>
            {
                static C const& get(eastl::priority_queue<T, C, Comp> const& pq)
                {
                    return pq.*(&H::c);
                }
            };

            return H::get(priority_queue);
        }

        //! Allows access to the protected comparator in priority queue
        /*! @internal */
        template <class T, class C, class Comp> inline
            Comp const& comparator(eastl::priority_queue<T, C, Comp> const& priority_queue)
        {
            struct H : public eastl::priority_queue<T, C, Comp>
            {
                static Comp const& get(eastl::priority_queue<T, C, Comp> const& pq)
                {
                    return pq.*(&H::comp);
                }
            };

            return H::get(priority_queue);
        }
    }

    //! Saving for std::deque
    template <class Archive, class T, class A> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::deque<T, A> const& deque)
    {
        ar(make_size_tag(static_cast<size_type>(deque.size())));

        for (auto const& i : deque)
            ar(i);
    }

    //! Loading for std::deque
    template <class Archive, class T, class A> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::deque<T, A>& deque)
    {
        size_type size;
        ar(make_size_tag(size));

        deque.resize(static_cast<size_t>(size));

        for (auto& i : deque)
            ar(i);
    }

    //! Saving for std::queue
    template <class Archive, class T, class C> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::queue<T, C> const& queue)
    {
        ar(CEREAL_NVP_("container", queue_detail::container(queue)));
    }

    //! Loading for std::queue
    template <class Archive, class T, class C> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::queue<T, C>& queue)
    {
        C container;
        ar(CEREAL_NVP_("container", container));
        queue = eastl::queue<T, C>(std::move(container));
    }

    //! Saving for std::priority_queue
    template <class Archive, class T, class C, class Comp> inline
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, eastl::priority_queue<T, C, Comp> const& priority_queue)
    {
        ar(CEREAL_NVP_("comparator", queue_detail::comparator(priority_queue)));
        ar(CEREAL_NVP_("container", queue_detail::container(priority_queue)));
    }

    //! Loading for std::priority_queue
    template <class Archive, class T, class C, class Comp> inline
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, eastl::priority_queue<T, C, Comp>& priority_queue)
    {
        Comp comparator;
        ar(CEREAL_NVP_("comparator", comparator));

        C container;
        ar(CEREAL_NVP_("container", container));

        priority_queue = std::priority_queue<T, C, Comp>(comparator, std::move(container));
    }

} // namespace cereal

#endif
