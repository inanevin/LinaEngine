/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ResourceConstants
Timestamp: 1/5/2019 12:42:58 AM

*/

#pragma once

#ifndef ResourceConstants_HPP
#define ResourceConstants_HPP

#include <stdio.h>
#include <string>

namespace LinaEngine
{
	class ResourceConstants
	{

	public:
		static  std::string& ShadersPath;
		static	std::string& BasicTexturePath;
	};

}



#endif