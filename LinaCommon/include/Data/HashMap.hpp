/*
Class: UnorderedMap



Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresHashMap_HPP
#define DataStructuresHashMap_HPP

#include <parallel_hashmap/phmap.h>

namespace Lina
{
    template <typename T, typename U>
    using HashMap = phmap::flat_hash_map<T, U>;
} // namespace Lina

#endif
