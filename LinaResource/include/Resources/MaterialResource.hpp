/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

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
Class: MaterialResource

Represents a material resource holding rendering information.

Timestamp: 12/22/2020 2:03:09 PM
*/

#pragma once

#ifndef MaterialResource_HPP
#define MaterialResource_HPP

// Headers here.
#include "Resource.hpp"
#include <cereal/cereal.hpp>

namespace Lina::Resources
{
	class MaterialResource : IResource
	{
		
	private:

		friend class ResourceBundle;
		friend class ResourceManager;
		friend class cereal::access;

		MaterialResource() {};
		virtual ~MaterialResource() {};


		virtual bool LoadFromFile(const std::string& path, Event::EventSystem* eventSys) override;
		virtual bool LoadFromMemory(StringIDType m_sid, unsigned char* buffer, size_t bufferSize, Event::EventSystem* eventSys) override;
		bool Export(const std::string& path);
		
		int m_dummy = 0;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_dummy);
		}

	};
}

#endif
