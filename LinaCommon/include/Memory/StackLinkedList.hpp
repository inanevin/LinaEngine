/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Custom Memory Allocators: Copyright (c) 2016 Mariano Trebino

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
Class: StackLinkedList

Linked list for pool allocator.

Timestamp: 12/19/2020 1:44:39 AM
*/

#pragma once

#ifndef StackLinkedList_HPP
#define StackLinkedList_HPP

// Headers here.

namespace Lina
{
    template <class T> class StackLinkedList
    {
    public:
        struct Node
        {
            T     data;
            Node* next = nullptr;
        };

        Node* head = nullptr;

        StackLinkedList()                                 = default;
        StackLinkedList(StackLinkedList& stackLinkedList) = delete;
        void  push(Node* newNode);
        Node* pop();
    };

#include "SinglyLinkedList.hpp"

    template <class T> void StackLinkedList<T>::push(Node* newNode)
    {
        newNode->next = head;
        head          = newNode;
    }

    template <class T> typename StackLinkedList<T>::Node* StackLinkedList<T>::pop()
    {
        Node* top = head;
        head      = head->next;
        return top;
    }
} // namespace Lina

#endif
