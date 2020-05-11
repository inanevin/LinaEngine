/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ISerializable
Timestamp: 5/8/2020 5:50:14 PM

*/
#pragma once

#ifndef ISerializable_HPP
#define ISerializable_HPP

// Headers here.
#include <string>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <fstream>;


#endif

namespace LinaEngine
{
	class ISerializable
	{
		
	public:
		
		ISerializable() {};
		~ISerializable() {};
	
		virtual void WriteObject(char* path) = 0;
		virtual void ReadObject(char* path) = 0;


	private:
	
	};
}