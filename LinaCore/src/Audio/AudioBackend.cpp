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

#include "Core/Audio/AudioBackend.hpp"
#include <AL/al.h>
#include <AL/alc.h>

namespace Lina
{
#define AL_CHECK_ERR(err)                                                                                                                                                                                                                                          \
	err = alGetError();                                                                                                                                                                                                                                            \
	if (err != AL_NO_ERROR)                                                                                                                                                                                                                                        \
	LINA_TRACE("AL Error! {0}", err)
	namespace
	{
		void ListAudioDevices(const char* type, const char* list)
		{
			ALCchar *ptr, *nptr;
			ptr = (ALCchar*)list;
			LINA_TRACE("[Audio Engine OpenAL] -> List of all available {0} devices:", type);

			if (!list)
			{
				LINA_INFO("[Audio Engine OpenAL] -> None");
			}
			else
			{
				nptr = ptr;
				while (*(nptr += strlen(ptr) + 1) != 0)
				{
					LINA_INFO("[Audio Engine OpenAL] -> {0}", ptr);
					ptr = nptr;
				}
				LINA_INFO("[Audio Engine OpenAL] -> {0}", ptr);
			}
		}
	} // namespace

	void AudioBackend::Initialize()
	{
		if (alcIsExtensionPresent(NULL, "ALC_enumeration_EXT") == AL_TRUE)
		{
			char* s;

			if (alcIsExtensionPresent(NULL, "ALC_enumerate_all_EXT") == AL_FALSE)
				s = (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
			else
				s = (char*)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);

			ListAudioDevices("Output", s);

			s = (char*)alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
			ListAudioDevices("Input", s);
		}
		else
			ListAudioDevices("Input", alcGetString(NULL, ALC_DEVICE_SPECIFIER));

		const ALCchar* defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

		// Init device.
		m_device = alcOpenDevice(defaultDeviceName);
		LINA_TRACE("AudioBackend -> Initialized {0}", alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER));

		if (!m_device)
		{
			LINA_ERR("AudioBackend -> Can not open ALC device!");
			return;
		}

		m_context = alcCreateContext(m_device, NULL);
		if (!alcMakeContextCurrent(m_context))
		{
			LINA_ERR("AudioBackend -> Open AL Context Error {0}");
		}

		ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
		ALCenum error;
		alListener3f(AL_POSITION, 0, 0, 0.0f);
		AL_CHECK_ERR(error);
		alListener3f(AL_VELOCITY, 0, 0, 0);
		AL_CHECK_ERR(error);
		alListenerfv(AL_ORIENTATION, listenerOri);
		AL_CHECK_ERR(error);
	}

	void AudioBackend::Shutdown()
	{
		alcCloseDevice(m_device);
		alcDestroyContext(m_context);
	}

} // namespace Lina
