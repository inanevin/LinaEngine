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

#include "Common/SizeDefinitions.hpp"
#include "Common/Data/Bitmask.hpp"
#include "Common/Math/Color.hpp"

namespace Lina
{
	class ApplicationDelegate;

	enum class WindowStyle
	{
		WindowedApplication,   // Most OS-default application, windowed with title bar and system buttons
		BorderlessApplication, // Borderless-application, which still supports for example OS animations, taskbar interaction etc.
		Borderless,			   // True borderless, a lifeless and soulless application window only displaying what you draw.
		BorderlessAlpha,	   // Borderless but supports alpha blending,
		BorderlessFullscreen,  // Borderless full-screen by default, you can change the size later on
		Fullscreen,			   // Full-screen exclusive by default, you can change the size later on.
	};

	struct SystemInitializationInfo
	{
		const char*			 appName	  = "";
		uint32				 windowWidth  = 0;
		uint32				 windowHeight = 0;
		WindowStyle			 windowStyle  = WindowStyle::WindowedApplication;
		ApplicationDelegate* appDelegate  = nullptr;
	};

	enum class Direction
	{
		Top = 0,
		Bottom,
		Left,
		Right,
		Center
	};

	enum class DirectionOrientation
	{
		Horizontal,
		Vertical
	};

	struct TBLR
	{
		float top	 = 0;
		float bottom = 0;
		float left	 = 0;
		float right	 = 0;

		static TBLR Eq(float m)
		{
			return {.top = m, .bottom = m, .left = m, .right = m};
		}

		inline void SaveToStream(OStream& stream) const
		{
			stream << top << bottom << left << right;
		}

		inline void LoadFromStream(IStream& stream)
		{
			stream >> top >> bottom >> left >> right;
		}
	};

	extern Direction DirectionOpposite(const Direction& dir);
	extern Direction DirectionFromVector(const Vector2& dir);
	extern Vector2	 DirectionToVector(const Direction& dir);

} // namespace Lina
