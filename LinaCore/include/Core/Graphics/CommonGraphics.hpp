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
#include "Core/Resources/CommonResources.hpp"

namespace Lina
{

#define GUI_TEXTURE_HUE_HORIZONTAL UINT32_MAX - 1
#define GUI_TEXTURE_HUE_VERTICAL   UINT32_MAX - 2
#define GUI_TEXTURE_COLORWHEEL	   UINT32_MAX - 3
#define GUI_IS_SINGLE_CHANNEL	   2.0f

#define DEFAULT_SHADER_GUI_PATH				  "Resources/Core/Shaders/GUI/GUI2D.linashader"
#define DEFAULT_SHADER_GUI3D_PATH			  "Resources/Core/Shaders/GUI/GUI3D.linashader"
#define DEFAULT_SHADER_OBJECT_PATH			  "Resources/Core/Shaders/Object/DefaultObject.linashader"
#define DEFAULT_SHADER_SKY_PATH				  "Resources/Core/Shaders/Sky/DefaultSky.linashader"
#define DEFAULT_SHADER_DEFERRED_LIGHTING_PATH "Resources/Core/Shaders/Object/DeferredLighting.linashader"
#define DEFAULT_SAMPLER_PATH				  "Resources/Core/Samplers/DefaultSampler.linasampler"
#define DEFAULT_SAMPLER_GUI_PATH			  "Resources/Core/Samplers/DefaultGUISampler.linasampler"
#define DEFAULT_SAMPLER_TEXT_PATH			  "Resources/Core/Samplers/DefaultGUITextSampler.linasampler"
#define DEFAULT_MATERIAL_OBJECT_PATH		  "Resources/Core/Materials/DefaultObject.linamaterial"
#define DEFAULT_MATERIAL_SKY_PATH			  "Resources/Core/Materials/DefaultSky.linamaterial"
#define DEFAULT_TEXTURE_CHECKERED_DARK_PATH	  "Resources/Core/Textures/CheckeredDark.png"
#define DEFAULT_FONT_PATH					  "Resources/Core/Fonts/Roboto-Regular.ttf"
#define DEFAULT_FONT_BOLD_PATH				  "Resources/Core/Fonts/Roboto-Bold.ttf"
#define ALT_FONT_PATH						  "Resources/Editor/Fonts/Play-Regular.ttf"
#define ALT_FONT_BOLD_PATH					  "Resources/Editor/Fonts/Play-Bold.ttf"

#define DEFAULT_SHADER_GUI_ID				RESOURCE_ID_ENGINE_SPACE
#define DEFAULT_SHADER_GUI3D_ID				RESOURCE_ID_ENGINE_SPACE + 1
#define DEFAULT_SHADER_OBJECT_ID			RESOURCE_ID_ENGINE_SPACE + 2
#define DEFAULT_SHADER_SKY_ID				RESOURCE_ID_ENGINE_SPACE + 3
#define DEFAULT_SHADER_DEFERRED_LIGHTING_ID RESOURCE_ID_ENGINE_SPACE + 4
#define DEFAULT_SAMPLER_ID					RESOURCE_ID_ENGINE_SPACE + 5
#define DEFAULT_SAMPLER_GUI_ID				RESOURCE_ID_ENGINE_SPACE + 6
#define DEFAULT_SAMPLER_TEXT_ID				RESOURCE_ID_ENGINE_SPACE + 7
#define DEFAULT_MATERIAL_OBJECT_ID			RESOURCE_ID_ENGINE_SPACE + 8
#define DEFAULT_MATERIAL_SKY_ID				RESOURCE_ID_ENGINE_SPACE + 9
#define DEFAULT_TEXTURE_CHECKERED_DARK_ID	RESOURCE_ID_ENGINE_SPACE + 10
#define DEFAULT_TEXTURE_LINALOGO_ID			RESOURCE_ID_ENGINE_SPACE + 11
#define DEFAULT_FONT_ID						RESOURCE_ID_ENGINE_SPACE + 12
#define DEFAULT_FONT_BOLD_ID				RESOURCE_ID_ENGINE_SPACE + 13
#define DEFAULT_ALT_FONT_SID				RESOURCE_ID_ENGINE_SPACE + 14
#define DEFAULT_ALT_FONT_BOLD_SID			RESOURCE_ID_ENGINE_SPACE + 15

#define LINA_MAIN_SWAPCHAIN UINT32_MAX - 1
#define DEFAULT_CLEAR_CLR	Color(0.3f, 0.3f, 0.5f, 1.0f)
#define IDEAL_RT			16667
#define FRAMES_IN_FLIGHT	2
#define BACK_BUFFER_COUNT	3
#define MAX_BOUND_TEXTURES	512
#define MAX_BOUND_SAMPLERS	128

	enum RenderPassDescriptorType
	{
		Main = 0,
		Lighting,
		ForwardTransparency,
		Max,
	};

	constexpr const char* RPTypeToString(RenderPassDescriptorType type)
	{
		switch (type)
		{
		case Main:
			return "Main";
		case Lighting:
			return "Lighting";
		case ForwardTransparency:
			return "ForwardTransparency";
		case Max:
			return "Max";
		default:
			return "Unknown";
		}
	}

	class SemaphoreData
	{
	public:
		SemaphoreData(){};
		SemaphoreData(uint16 semaphore) : m_semaphore(semaphore){};

		inline uint64* GetValuePtr()
		{
			return &m_value;
		}

		inline uint64 GetValue() const
		{
			return m_value;
		}

		inline uint16* GetSemaphorePtr()
		{
			return &m_semaphore;
		}

		inline uint16 GetSemaphore() const
		{
			return m_semaphore;
		}

		inline bool IsModified() const
		{
			return m_modified;
		}

		inline void Increment()
		{
			m_value++;
			m_modified = true;
		}

		inline void ResetModified()
		{
			m_modified = false;
		}

	private:
		uint16 m_semaphore = 0;
		uint64 m_value	   = 0;
		bool   m_modified  = false;
	};

	struct LinaTexture2D
	{
		StringID texture = 0;
		StringID sampler = 0;
	};

	enum class ShaderPropertyType
	{
		Bool,
		Float,
		Int32,
		UInt32,
		Vec2,
		Vec3,
		Vec4,
		IVec2,
		IVec3,
		IVec4,
		Matrix4,
		Texture2D,
	};

} // namespace Lina
