/*
Class: HashMap



Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresFixedHashMap_HPP
#define DataStructuresFixedHashMap_HPP

#include <EASTL/fixed_hash_map.h>

namespace Lina
{
    template <typename T, typename U, size_t nodeCount>
    using FixedHashMap = eastl::fixed_hash_map<T, U, nodeCount>;
} // namespace Lina

#endif
