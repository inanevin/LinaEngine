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
#include "EventSystem/EventSystem.hpp"
#include "ECS/ECS.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Audio/Audio.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <fstream>

namespace Lina::Audio
{
	OpenALAudioEngine* OpenALAudioEngine::s_audioEngine = nullptr;

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

		Event::EventSystem::Get()->Connect<Event::ELoadResourceFromFile, &OpenALAudioEngine::OnLoadResourceFromFile>(this);
		Event::EventSystem::Get()->Connect<Event::ELoadResourceFromMemory, &OpenALAudioEngine::OnLoadResourceFromMemory>(this);
	}

	void OpenALAudioEngine::Shutdown()
	{
		LINA_TRACE("[Shutdown] -> Audio Engine ({0})", typeid(*this).name());

		for (auto& s : m_generatedSources)
			alDeleteSources((ALuint)1, &s.second);

		m_generatedSources.clear();

		Audio::UnloadAll();

	}

	void OpenALAudioEngine::PlayOneShot(Audio& audio, float gain, bool looping, float pitch, Vector3 position, Vector3 velocity)
	{
		unsigned int source = -1;

		StringIDType sid = audio.GetSID();

		if (m_generatedSources.find(sid) == m_generatedSources.end())
		{
			alGenSources((ALuint)1, &source);
			m_generatedSources[sid] = source;
		}
		else
			source = m_generatedSources[sid];

		alSourcef(source, AL_PITCH, pitch);
		alSourcef(source, AL_GAIN, gain);
		alSource3f(source, AL_POSITION, position.x, position.y, position.z);
		alSource3f(source, AL_VELOCITY, 0, 0, 0);
		alSourcei(source, AL_LOOPING, looping);
		alSourcei(source, AL_BUFFER, audio.GetBuffer());
		alSourcePlay(source);
	}

	void OpenALAudioEngine::ListAudioDevices(const char* type, const char* list)
	{
		ALCchar* ptr, * nptr;
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

	void OpenALAudioEngine::OnLoadResourceFromFile(Event::ELoadResourceFromFile ev)
	{
		if (ev.m_resourceType == Resources::ResourceType::Audio)
		{
			LINA_TRACE("[Audio Loader] -> Loading (file): {0}", ev.m_path);

			std::string paramsPath = Utility::GetFileWithoutExtension(ev.m_path) + ".linaaudiodata";
			AudioAssetData params;

			if (Utility::FileExists(paramsPath))
				params = Audio::LoadAssetData(paramsPath);
			else
				Audio::SaveAssetData(paramsPath, params);

			Audio::CreateAudio(ev.m_path, params);
		}
	}

	void OpenALAudioEngine::OnLoadResourceFromMemory(Event::ELoadResourceFromMemory ev)
	{
		if (ev.m_resourceType == Resources::ResourceType::Audio)
		{
			LINA_TRACE("[Audio Loader] -> Loading (memory): {0}", ev.m_path);

			AudioAssetData params;

			if (ev.m_assetDataBuffer != nullptr)
				params = Audio::LoadAssetDataFromMemory(ev.m_assetDataBuffer, ev.m_assetDataSize);

			Audio::CreateAudioFromMemory(ev.m_path, ev.m_data, ev.m_dataSize, params);
		}
	}

}

