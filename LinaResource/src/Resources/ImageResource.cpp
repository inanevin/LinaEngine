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

#include "Resources/ImageResource.hpp"
#include "EventSystem/Events.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/ResourceManager.hpp"
#include "Log/Log.hpp"
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb/stb_image.h"

namespace Lina::Resources
{
	bool ImageResource::LoadFromMemory(StringIDType m_sid, unsigned char* buffer, size_t bufferSize, Event::EventSystem* eventSys)
	{
		// int32_t w, h, comps;
		// unsigned char* data = stbi_load_from_memory(buffer, bufferSize, &w, &h, &comps, 4);
		// 
		// if (data == nullptr)
		// {
		// 	LINA_ERR("[Image Loader] -> Image can not be loaded from memory!");
		// 	return false;
		// }
		// 
		// // Trigger event & free data.
		// eventSys->Trigger<Event::EImageResourceLoaded>(Event::EImageResourceLoaded{ m_sid, data, w, h });
		// free(data);
		// 
		// LINA_TRACE("[Image Loader] -> Image loaded from memory.");
		return true;
	}

	bool ImageResource::LoadFromFile(const std::string& path, Event::EventSystem* eventSys)
	{
		// int32_t w, h, comps;
		// unsigned char* data = stbi_load(path.c_str(), &w, &h, &comps, 4);
		// 
		// if (data == nullptr)
		// {
		// 	LINA_ERR("[Image Loader] -> Image can not be loaded: {0}", path);
		// 	return false;
		// }
		// 
		// // Trigger event & free data.
		// eventSys->Trigger<Event::EImageResourceLoaded>(Event::EImageResourceLoaded{ StringID(path.c_str()).value(), data, w, h });
		// free(data);
		// 
		// LINA_TRACE("[Image Loader] -> Image loaded from file: {0}", path);
		return true;
	}

}