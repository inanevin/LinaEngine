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
Class: SpriteRendererComponent
Timestamp: 10/1/2020 9:26:21 AM
*/

#pragma once

#ifndef SpriteRendererComponent_HPP
#define SpriteRendererComponent_HPP

#include "ECS/Component.hpp"
#include <string>

namespace Lina::ECS
{
	struct SpriteRendererComponent : public Component
	{
		int m_materialID = -1;
		std::string m_materialPaths = "";

		// Editor properties, not inside the macro to avoid any struct size mismatch during serialization.
		int m_selectedMatID = -1;
		std::string m_selectedMatPath = "";

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_materialPaths, m_isEnabled); // serialize things by passing them to the archive
		}

	};
}

#endif
