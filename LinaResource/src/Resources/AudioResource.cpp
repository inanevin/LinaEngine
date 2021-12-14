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

#include "Resources/AudioResource.hpp"
#include "EventSystem/Events.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Log/Log.hpp"
#include <AL/alut.h>
#include <fstream>

namespace Lina::Resources
{
	bool AudioResource::LoadFromFile(const std::string& path, Event::EventSystem* eventSys)
	{
		ALsizei size;
		ALfloat freq;
		ALenum format;
		ALvoid* data = alutLoadMemoryFromFile(path.c_str(), &format, &size, &freq);

		ALenum err = alutGetError();
		if (err != ALUT_ERROR_NO_ERROR)
		{
			LINA_ERR("[Audio Loader] -> Failed loading audio from file: {0} {1}", path, alutGetErrorString(err));
			return false;
		}

		// Trigger event & free data.
		eventSys->Trigger<Event::EAudioResourceLoaded>(Event::EAudioResourceLoaded{ StringID(path.c_str()).value(), (void*)data, size, format, freq });
		free(data);

		LINA_TRACE("[Audio Loader] -> Audio loaded from file: {0}", path);
		return true;
	}

	bool AudioResource::LoadFromMemory(StringIDType m_sid, unsigned char* buffer, size_t bufferSize, Event::EventSystem* eventSys)
	{
		ALsizei size;
		ALfloat freq;
		ALenum format;
		ALvoid* data = alutLoadMemoryFromFileImage(buffer, bufferSize, &format, &size, &freq);

		ALenum err = alutGetError();
		if (err != ALUT_ERROR_NO_ERROR)
		{
			LINA_ERR("[Audio Loader] -> Failed loading audio from memory: {0}", alutGetErrorString(err));
			return false;
		}

		// Trigger event & free data.
		eventSys->Trigger<Event::EAudioResourceLoaded>(Event::EAudioResourceLoaded{ m_sid, (void*)data, size, format, freq });
		free(data);
		LINA_TRACE("[Audio Loader] -> Audio loaded from memory");
		return true;
	}



}