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

#ifndef Common_HPP
#define Common_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"
#include "Data/Bitmask.hpp"

namespace Lina
{
	enum class PreferredGPUType
	{
		Discrete = 0,
		Integrated,
		CPU
	};

	enum class VsyncMode
	{
		None = 0,
		EveryVBlank,
		EverySecondVBlank,
	};

	enum class WindowStyle
	{
		None = 0,
		Windowed,
		WindowedNoResize,
		Borderless,
		BorderlessNoResize,
		Fullscreen
	};

	enum class ApplicationMode
	{
		Standalone,
		Editor
	};

	struct SystemInitializationInfo
	{
		const char*		 appName		  = "";
		int				 windowWidth	  = 0;
		int				 windowHeight	  = 0;
		WindowStyle		 windowStyle	  = WindowStyle::Windowed;
		PreferredGPUType preferredGPUType = PreferredGPUType::Discrete;
		VsyncMode		 vsyncMode		  = VsyncMode::None;
	};

	struct MonitorInfo
	{
		bool	 isPrimary	   = false;
		float	 m_dpiScale	   = 0.0f;
		Vector2i size		   = Vector2i::Zero;
		Vector2i workArea	   = Vector2i::Zero;
		Vector2i workTopLeft   = Vector2i::Zero;
		uint32	 m_dpi		   = 0;
		void*	 monitorHandle = nullptr;
	};

	extern const char* VsyncModeToStr(VsyncMode mode);
} // namespace Lina

#endif
