/*
Class: Dequeue

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresHashSet_HPP
#define DataStructuresHashSet_HPP
#include <EASTL/hash_set.h>

namespace Lina
{
	template<typename T>
	using HashSet = eastl::hash_set<T>;
} // namespace Lina

#endif
