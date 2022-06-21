/*
Class: Map

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresMap_HPP
#define DataStructuresMap_HPP
#include <EASTL/map.h>
namespace Lina
{
	template<typename T, typename U, typename Compare = eastl::less<T>>
	using Map = eastl::map<T, U, Compare>;
} // namespace Lina

#endif
