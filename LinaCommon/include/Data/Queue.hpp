/*
Class: Queue



Timestamp: 05/12/2022 11:33:56 PM
*/

#pragma once

#ifndef DataStructuresQueue_HPP
#define DataStructuresQueue_HPP

#include <EASTL/queue.h>
namespace Lina
{
	template<typename T>
	using Queue = eastl::queue<T>;

} // namespace Lina

#endif
