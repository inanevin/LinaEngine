/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Lina_Core
Timestamp: 12/29/2018 10:43:46 PM

*/

#pragma once

#ifndef Lina_Core_HPP
#define Lina_Core_HPP
 

#ifdef LINA_PLATFORM_WINDOWS
	#ifdef LINA_BUILD_DLL
		#define LINA_API __declspec(dllexport)
	#else
		#define LINA_API __declspec(dllimport)
	#endif
#else
	#error Lina supports Windows only for now.
#endif

namespace LinaEngine
{
	class Lina_Core
	{
	public:

		Lina_Core();

	};
}


#endif