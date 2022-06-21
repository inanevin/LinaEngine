/*
Class: List

Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresList_HPP
#define DataStructuresList_HPP

#include <EASTL/list.h>

namespace Lina
{
	template<typename T>
	using List = eastl::list<T>;
} // namespace Lina

#endif
