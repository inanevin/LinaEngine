/*
Class: Vector


Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresFixedVector_HPP
#define DataStructuresFixedVector_HPP

#include <EASTL/fixed_vector.h>

namespace Lina
{
    template <typename T, size_t nodeCount>
    using FixedVector = eastl::fixed_vector<T, nodeCount>;
} // namespace Lina

#endif
