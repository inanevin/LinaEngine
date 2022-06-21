/*
Class: Dequeue

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresFixedSet_HPP
#define DataStructuresFixedSet_HPP
#include <EASTL/set.h>

namespace Lina
{
	template<typename T, size_t nodeCount>
	using FixedSet = eastl::fixed_set<T, nodeCount>;
} // namespace Lina

#endif
