/*
Class: List

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresSList_HPP
#define DataStructuresSList_HPP

#include <EASTL/slist.h>

namespace Lina
{
	template<typename T>
	using SList = eastl::slist<T>;
} // namespace Lina

#endif
