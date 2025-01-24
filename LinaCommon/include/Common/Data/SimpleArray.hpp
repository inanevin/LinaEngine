/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef DataStructuresSimpleArray_HPP
#define DataStructuresSimpleArray_HPP

#include "Common/Memory/Memory.hpp"

namespace Lina
{

	/// <summary>
	/// Simple array/vector implementation that uses malloc/free instead of new/delete.
	/// Inserts are constant, removals are expensive.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T> class SimpleArray
	{
	public:
		SimpleArray() : data_(nullptr), size_(0), capacity_(0)
		{
		}
		~SimpleArray()
		{
			clear();
		}

		void push_back(T value)
		{
			if (size_ == capacity_)
				resize();
			data_[size_] = value;
			size_++;
		}

		void remove(int index)
		{
			for (int i = index; i < size_ - 1; ++i)
				data_[i] = data_[i + 1];
			size_--;
		}

		int size() const
		{
			return size_;
		}

		void reserve(int new_capacity)
		{
			if (new_capacity > capacity_)
			{
				T* new_data = (T*)std::malloc(new_capacity * sizeof(T));
				if (new_data != NULL)
				{
					for (int i = 0; i < size_; ++i)
						new_data[i] = data_[i];
					std::free(data_);
					data_	  = nullptr;
					data_	  = new_data;
					capacity_ = new_capacity;
				}
			}
		}

		void clear()
		{
			if (!data_)
				return;
			std::free(data_);
			data_	  = nullptr;
			size_	  = 0;
			capacity_ = 0;
		}

		T& operator[](int index) const
		{
			if (index < 0 || index >= size_)
				throw std::out_of_range("Array: out of range");
			return data_[index];
		}

	private:
		void resize()
		{
			int newCapacity = (capacity_ == 0) ? 1 : capacity_ * 2;
			T*	new_data	= (T*)std::malloc(newCapacity * sizeof(T));

			if (new_data)
			{
				for (int i = 0; i < size_; ++i)
					new_data[i] = data_[i];
				std::free(data_);
				data_	  = nullptr;
				capacity_ = newCapacity;
				data_	  = new_data;
			}
		}

		T*	data_	  = nullptr;
		int size_	  = 0;
		int capacity_ = 0;
	};
} // namespace Lina

#endif
