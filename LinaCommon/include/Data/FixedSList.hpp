/*
Class: List

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresSList_HPP
#define DataStructuresSList_HPP

#include <EASTL/fixed_slist.h>

namespace Lina
{
	template<typename T>
	using FixedSList = eastl::fixed_slist<T>;
} // namespace Lina

#endif
