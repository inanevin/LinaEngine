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
