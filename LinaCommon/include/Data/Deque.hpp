/*
Class: Dequeue

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresDeque_HPP
#define DataStructuresDeque_HPP
#include <EASTL/deque.h>

namespace Lina
{
	template<typename T>
	using Deque = eastl::deque<T>;
} // namespace Lina

#endif
