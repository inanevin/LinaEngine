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

namespace Lina
{

#define GUI_TEXTURE_HUE_HORIZONTAL UINT32_MAX - 1
#define GUI_TEXTURE_HUE_VERTICAL   UINT32_MAX - 2
#define GUI_TEXTURE_COLORWHEEL	   UINT32_MAX - 3
#define GUI_IS_SINGLE_CHANNEL	   2.0f

#define DEFAULT_SHADER_GUI_PATH "Resources/Core/Shaders/GUI/GUI.linashader"
#define DEFAULT_SHADER_GUI_SID	"Resources/Core/Shaders/GUI/GUI.linashader"_hs

#define DEFAULT_SHADER_OBJECT_PATH "Resources/Core/Shaders/Object/DefaultObject.linashader"
#define DEFAULT_SHADER_OBJECT_SID  "Resources/Core/Shaders/Object/DefaultObject.linashader"_hs

#define DEFAULT_SHADER_SKY_PATH "Resources/Core/Shaders/Sky/DefaultSky.linashader"
#define DEFAULT_SHADER_SKY_SID	"Resources/Core/Shaders/Sky/DefaultSky.linashader"_hs

#define DEFAULT_SHADER_DEFERRED_LIGHTING_PATH "Resources/Core/Shaders/Object/DeferredLighting.linashader"
#define DEFAULT_SHADER_DEFERRED_LIGHTING_SID  "Resources/Core/Shaders/Object/DeferredLighting.linashader"_hs

#define DEFAULT_SAMPLER_PATH "Resources/Core/Samplers/DefaultSampler.linasampler"
#define DEFAULT_SAMPLER_SID	 "Resources/Core/Samplers/DefaultSampler.linasampler"_hs

#define DEFAULT_SAMPLER_GUI_PATH "Resources/Core/Samplers/DefaultGUISampler.linasampler"
#define DEFAULT_SAMPLER_GUI_SID	 "Resources/Core/Samplers/DefaultGUISampler.linasampler"_hs

#define DEFAULT_SAMPLER_TEXT_PATH "Resources/Core/Samplers/DefaultGUITextSampler.linasampler"
#define DEFAULT_SAMPLER_TEXT_SID  "Resources/Core/Samplers/DefaultGUITextSampler.linasampler"_hs

#define DEFAULT_MATERIAL_OBJECT_PATH "Resources/Core/Materials/DefaultObject.linamaterial"
#define DEFAULT_MATERIAL_OBJECT_SID	 "Resources/Core/Materials/DefaultObject.linamaterial"_hs

#define DEFAULT_MATERIAL_SKY_PATH "Resources/Core/Materials/DefaultSky.linamaterial"
#define DEFAULT_MATERIAL_SKY_SID  "Resources/Core/Materials/DefaultSky.linamaterial"_hs

#define LINA_MAIN_SWAPCHAIN			UINT32_MAX - 1
#define DEFAULT_TEXTURE_LINALOGO	"Resources/Core/Textures/StubLinaLogo.png"_hs
#define DEFAULT_TEXTURE_EMPTY_BLACK "Resources/Core/Textures/StubBlack.png"_hs
#define DEFAULT_TEXTURE_CHECKERED	"Resources/Core/Textures/Checkered.png"_hs
#define DEFAULT_CLEAR_CLR			Color(0.3f, 0.3f, 0.5f, 1.0f)
#define IDEAL_RT					16667
#define FRAMES_IN_FLIGHT			2
#define BACK_BUFFER_COUNT			3
#define MAX_BOUND_TEXTURES			512
#define MAX_BOUND_SAMPLERS			128

	enum RenderPassDescriptorType
	{
		Gui = 0,
		Main,
		Lighting,
		Max,
	};

	constexpr const char* RPTypeToString(RenderPassDescriptorType type)
	{
		switch (type)
		{
		case Gui:
			return "Gui";
		case Main:
			return "Main";
		case Lighting:
			return "Lighting";
		case Max:
			return "Max";
		default:
			return "Unknown";
		}
	}

	struct SemaphoreData
	{
		uint16 semaphore = 0;
		uint64 value	 = 0;
	};

} // namespace Lina
