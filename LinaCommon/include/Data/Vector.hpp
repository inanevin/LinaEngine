/*
Class: Vector


Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresVector_HPP
#define DataStructuresVector_HPP

#include <EASTL/vector.h>

namespace Lina
{
    template <typename T>
    using Vector = eastl::vector<T>;
} // namespace Lina

#endif
