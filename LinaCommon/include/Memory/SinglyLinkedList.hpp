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

#ifndef SinglyLinkedList_HPP
#define SinglyLinkedList_HPP

// Headers here.

namespace Lina
{

    template <class T> class SinglyLinkedList
    {
    public:
        struct Node
        {
            T     data;
            Node* next;
        };

        Node* head;

    public:
        SinglyLinkedList();

        void insert(Node* previousNode, Node* newNode);
        void remove(Node* previousNode, Node* deleteNode);
    };

    template <class T> SinglyLinkedList<T>::SinglyLinkedList()
    {
    }

    template <class T> void SinglyLinkedList<T>::insert(Node* previousNode, Node* newNode)
    {
        if (previousNode == nullptr)
        {
            // Is the first node
            if (head != nullptr)
            {
                // The list has more elements
                newNode->next = head;
            }
            else
            {
                newNode->next = nullptr;
            }
            head = newNode;
        }
        else
        {
            if (previousNode->next == nullptr)
            {
                // Is the last node
                previousNode->next = newNode;
                newNode->next      = nullptr;
            }
            else
            {
                // Is a middle node
                newNode->next      = previousNode->next;
                previousNode->next = newNode;
            }
        }
    }

    template <class T> void SinglyLinkedList<T>::remove(Node* previousNode, Node* deleteNode)
    {
        if (previousNode == nullptr)
        {
            // Is the first node
            if (deleteNode->next == nullptr)
            {
                // List only has one element
                head = nullptr;
            }
            else
            {
                // List has more elements
                head = deleteNode->next;
            }
        }
        else
        {
            previousNode->next = deleteNode->next;
        }
    }

} // namespace Lina

#endif
