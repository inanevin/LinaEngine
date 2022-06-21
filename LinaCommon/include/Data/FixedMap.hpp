/*
Class: Map

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresFixedMap_HPP
#define DataStructuresFixedMap_HPP

#include <EASTL/fixed_map.h>

namespace Lina
{
	template<typename T, typename U, size_t nodeCount>
	using FixedMap = eastl::fixed_map<T, U, nodeCount>;
} // namespace Lina

#endif
