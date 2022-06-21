/*
Class: List

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresFixedList_HPP
#define DataStructuresFixedList_HPP

#include <EASTL/fixed_list.h>

namespace Lina
{
	template<typename T, size_t nodeCount>
	using FixedList = eastl::fixed_list<T, nodeCount>;
} // namespace Lina

#endif
