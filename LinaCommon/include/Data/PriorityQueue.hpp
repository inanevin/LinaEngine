/*
Class: PriorityQueue

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresPriorityQueue_HPP
#define DataStructuresPriorityQueue_HPP

#include <EASTL/priority_queue.h>
namespace Lina
{
	template<typename T, typename Container = eastl::vector<T>, typename Compare = eastl::less<typename Container::value_type>>
	using PriorityQueue = eastl::priority_queue<T, Container, Compare>;

} // namespace Lina

#endif
