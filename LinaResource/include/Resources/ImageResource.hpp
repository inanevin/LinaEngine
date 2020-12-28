/* 
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: ImageResource

Defines the interface for loading & unloading image resources.

Timestamp: 12/19/2020 2:26:58 AM
*/

#pragma once

#ifndef ImageResource_HPP
#define ImageResource_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include <cstdint>
#include <vector>

namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Resources
{

	class ImageResource 
	{
		
	public:

	private:

		friend class ResourceBundle;
		friend class ResourceManager;

		ImageResource() {};
		virtual ~ImageResource() {};

		bool LoadFromFile(const std::string& path, Event::EventSystem* eventSys);
		bool LoadFromMemory(StringIDType m_sid, unsigned char* buffer, size_t bufferSize, Event::EventSystem* eventSys);

	};

	
}

#endif
