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

#include "Core/Backend/OpenAL/OpenALAudioEngine.hpp"  
#include "ECS/ECS.hpp"
#include "Log/Log.hpp"
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"
#include <fstream>

namespace Lina::Audio
{
	void OpenALAudioEngine::Initialize()
	{
		LINA_TRACE("[Initialization] -> Audio Engine ({0})", typeid(*this).name());

		m_ecs = ECS::Registry::Get();

		// List available devices.
		ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");

		if (enumeration == AL_FALSE)
			LINA_WARN("[Audio Engine OpenAL] -> Open AL enumeration is not present!");
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
		LINA_TRACE("[Audio Engine OpenAL] -> Initialized {0}", alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER));

		if (!m_device)
		{
			LINA_ERR("[Audio Engine OpenAL] -> Can not open ALC device!");
			return;
		}

		// Init context.
		m_context = alcCreateContext(m_device, NULL);
		if (!alcMakeContextCurrent(m_context))
		{
			LINA_ERR("[Audio Engine OpenAL] -> Open AL Context Error {0}", alutGetErrorString(alGetError()));
		}

		// Init alut
		alutInit(NULL, NULL);
	}

	void OpenALAudioEngine::Shutdown()
	{
		LINA_TRACE("[Shutdown] -> Audio Engine ({0})", typeid(*this).name());

		// Cleanup buffers.
		for (auto buffer : m_audioBuffers)
		{
			alDeleteBuffers(1, &buffer.second);
			LINA_TRACE("[Audio Engine OpenAL] -> Buffer deleted.");
		}
	}

	void OpenALAudioEngine::LoadAudioSource(const std::string& path)
	{
		ALsizei size;
		ALfloat freq;
		ALenum format;
		ALvoid* data = alutLoadMemoryFromFile(path.c_str(), &format, &size, &freq);

		ALenum err = alutGetError();
		if (err != ALUT_ERROR_NO_ERROR)
		{
			LINA_ERR("[Audio Loader] -> Failed loading audio from file: {0} {1}", path, alutGetErrorString(err));
			return;
		}

		// Trigger event & free data.
		OnAudioSourceLoaded(path, (void*)data, size, format, freq);
		free(data);

		LINA_TRACE("[Audio Loader] -> Audio loaded from file: {0}", path);
	}

	void OpenALAudioEngine::PlayOneShot(const std::string& path, float gain, bool looping, float pitch, Vector3 position, Vector3 velocity)
	{
		if (m_audioBuffers.find(path) == m_audioBuffers.end())
			return;

		if (m_oneShotSources.find(path) == m_oneShotSources.end())
		{
			// Set source properties.
			unsigned int source;
			alGenSources((ALuint)1, &source);
			alSourcef(source, AL_PITCH, pitch);
			alSourcef(source, AL_GAIN, gain);
			alSource3f(source, AL_POSITION, position.x, position.y, position.z);
			alSource3f(source, AL_VELOCITY, 0, 0, 0);
			alSourcei(source, AL_LOOPING, looping);
			alSourcei(source, AL_BUFFER, m_audioBuffers[path]);

			m_oneShotSources[path] = source;
		}
		
		unsigned int ps = m_oneShotSources[path];

		// Set source properties.
		alSourcef(ps, AL_PITCH, pitch);
		alSourcef(ps, AL_GAIN, gain);
		alSource3f(ps, AL_POSITION, position.x, position.y, position.z);
		alSource3f(ps, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		alSourcei(ps, AL_LOOPING, looping);
		alSourcePlay(ps);

	}

	void OpenALAudioEngine::ListAudioDevices(const char* type, const char* list)
	{
		ALCchar* ptr, * nptr;
		ptr = (ALCchar*)list;
		LINA_TRACE("[Audio Engine OpenAL] -> List of all available {0} devices:", type);

		if (!list)
		{
			LINA_TRACE("[Audio Engine OpenAL] -> None");
		}
		else
		{
			nptr = ptr;
			while (*(nptr += strlen(ptr) + 1) != 0)
			{
				LINA_TRACE("[Audio Engine OpenAL] -> {0}", ptr);
				ptr = nptr;
			}
			LINA_TRACE("[Audio Engine OpenAL] -> {0}", ptr);
		}
	}

	void OpenALAudioEngine::OnAudioSourceLoaded(const std::string& path, void* data, int dataSize, int format, float freq)
	{
		// Generate an OpenAL buffer for the loaded audio source.
		std::lock_guard<std::mutex> l(m_mutex);
		ALuint buffer;
		alGenBuffers((ALuint)1, &buffer);
		alBufferData(buffer, format, data, dataSize, freq);
		m_audioBuffers[path] = buffer;
		LINA_TRACE("[Audio Engine OpenAL] -> Buffer generated.");

#ifndef LINA_PRODUCTION_BUILD
		CheckForError();
#endif
	}

	void OpenALAudioEngine::CheckForError()
	{
		ALCenum error;
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			LINA_ERR("[Audio Engine OpenAL] -> Open AL Error {0}", alutGetErrorString(error));
		}
	}
}

