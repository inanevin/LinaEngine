/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: APIExport
Timestamp: 4/17/2019 1:45:39 AM

*/

#pragma once

#ifndef APIExport_HPP
#define APIExport_HPP

#ifdef LINA_PLATFORM_WINDOWS
#ifdef LINA_BUILDAS_SHARED

	#ifdef LINA_BUILD_DLL
	#define LINA_API __declspec(dllexport)
	#define LINA_API_EXTERN extern "C" __declspec(dllexport)
	#else
	#define LINA_API __declspec(dllimport)
	#define LINA_API_EXTERN __declspec(dllimport)
	#endif
#else
#define LINA_API
#endif
#else
#define LINA_API
#error Lina supports Windows only for now.
#endif

#endif