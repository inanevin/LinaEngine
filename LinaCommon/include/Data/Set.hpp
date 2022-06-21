/*
Class: Dequeue

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresSet_HPP
#define DataStructuresSet_HPP
#include <EASTL/set.h>

namespace Lina
{
	template<typename T>
	using Set = eastl::set<T>;
} // namespace Lina

#endif
