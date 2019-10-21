/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: DataStructures
Timestamp: 4/17/2019 1:46:20 AM

*/

#pragma once

#ifndef DataStructures_HPP
#define DataStructures_HPP

#include "STLImport.hpp"


#define LinaMap std::map
#define LinaList std::list
#define LinaTuple std::tuple
#define LinaPair std::pair
#define LinaMakePair std::make_pair
#define LinaGet std::get
#define LinaMakeTuple std::make_tuple
#define LinaString std::string
#define LinaHash std::hash

template<typename T>
class LinaArray : public std::vector<T>
{
public:
	LinaArray<T>() : std::vector<T>() {}
	LinaArray<T>(size_t n) : std::vector<T>(n){}

	void swap_remove(size_t index)
	{
		std::swap((*this)[index], (*this)[this->size() - 1]);
		this->pop_back();
	}
};


#endif