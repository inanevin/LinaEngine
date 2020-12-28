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

#include "Core/Backend/OpenAL/AudioEngineOpenAL.hpp"  
#include "Core/Log.hpp"
#include "Core/MacroDetection.hpp"
#include "Profiling/Profiler.hpp"
#include "Components/ECAudioListener.hpp"
#include "ECS/Components/ECTransform.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Math/Quaternion.hpp"
#include "Components/ECAudioSource.hpp"
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"


namespace Lina::Audio
{

	AudioEngineOpenAL::~AudioEngineOpenAL()
	{
		m_eventSys->Disconnect<Event::EAppLoad>(this);
		m_eventSys->Disconnect<Event::EPostMainLoop>(this);
		m_eventSys->Disconnect<Event::EAudioResourceLoaded>(this);
		m_eventSys->Disconnect<Event::EPreTick>(this);
		m_eventSys->Disconnect<Event::ELevelLoaded>(this);
		m_ecs->on_construct<ECS::ECAudioSource>().disconnect<&AudioEngineOpenAL::OnSourceAdded>(this);
		m_ecs->on_destroy<ECS::ECAudioSource>().disconnect<&AudioEngineOpenAL::OnSourceRemoved>(this);
	}

	void AudioEngineOpenAL::SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs, ApplicationMode appMode)
	{
		m_eventSys = eventSys;
		m_ecs = ecs;
		m_appMode = appMode;

		// Register ecs components.
		ECS::RegisterComponent<ECS::ECAudioSource>();

		// Listen to ecs audio source events
		m_ecs->on_construct<ECS::ECAudioSource>().connect<&AudioEngineOpenAL::OnSourceAdded>(this);
		m_ecs->on_destroy<ECS::ECAudioSource>().connect<&AudioEngineOpenAL::OnSourceRemoved>(this);

		// Listen to events
		m_eventSys->Connect<Event::EAudioResourceLoaded, &AudioEngineOpenAL::OnAudioResourceLoaded>(this);
		m_eventSys->Connect<Event::EAppLoad, &AudioEngineOpenAL::OnAppLoad>(this);
		m_eventSys->Connect<Event::EPostMainLoop, &AudioEngineOpenAL::OnPostMainLoop>(this);
		m_eventSys->Connect<Event::EPreTick, &AudioEngineOpenAL::OnTick>(this);
		m_eventSys->Connect<Event::ELevelLoaded, &AudioEngineOpenAL::OnLevelLoaded>(this);
		m_eventSys->Connect<Event::ELevelUnloaded, &AudioEngineOpenAL::OnLevelUnloaded>(this);
	}

	void AudioEngineOpenAL::OnAppLoad(Event::EAppLoad& e)
	{
		LINA_TRACE("[Audio Engine OpenAL] -> Startup");

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

	void AudioEngineOpenAL::OnPostMainLoop(Event::EPostMainLoop& e)
	{
		CleanUp();
		alutExit();
		m_device = alcGetContextsDevice(m_context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);
		LINA_TRACE("[Audio Engine OpenAL] -> Shutdown");
	}

	void AudioEngineOpenAL::OnTick()
	{
		PROFILER_FUNC();
	
		// Update audio listener position.
		auto& groupListener = m_ecs->group<ECS::ECAudioListener>(ECS_GET<ECS::ECTransform>);
		for (auto entity : groupListener)
		{
			ECS::ECAudioListener& listener = groupListener.get<ECS::ECAudioListener>(entity);
			ECS::ECTransform& transform = groupListener.get<ECS::ECTransform>(entity);
			Quaternion rot = MathV::VecToRadians(transform.m_rotation);
			Vector3 vel = MathV::Normalized(transform.m_position - m_mainListenerLastPos);
			ALfloat listenerOri[] = { rot.x, rot.y, rot.z, rot.w };
			alListener3f(AL_POSITION, transform.m_position.x, transform.m_position.y, transform.m_position.z);
			alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
			alListenerfv(AL_ORIENTATION, listenerOri);
			m_mainListenerLastPos = transform.m_position;
		}
	
		// Check audio sources.
		auto& groupSource = m_ecs->group<ECS::ECAudioSource>(ECS_GET<ECS::ECTransform>);
		for (auto entity : groupSource)
		{
			// Set comp properties.
			ECS::ECAudioSource& source = groupSource.get<ECS::ECAudioSource>(entity);
			ECS::ECTransform& transform = groupSource.get<ECS::ECTransform>(entity);
			Vector3 position = transform.m_position;
			Vector3 velocity = source.m_previousPosition - position;

			// Set source properties.
			alSourcef(source.m_source, AL_PITCH, source.m_pitch);
			alSourcef(source.m_source, AL_GAIN, source.m_gain);
			alSource3f(source.m_source, AL_POSITION, position.x, position.y, position.z);
			alSource3f(source.m_source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
			alSourcei(source.m_source, AL_LOOPING, source.m_loop);
			source.m_previousPosition = position;

			ALint state;
			alGetSourcei(source.m_source, AL_SOURCE_STATE, &state);

			if (source.m_playState == ECS::ECAudioSource::PlayState::Playing && state != AL_PLAYING)
			{
				if (source.m_resource == SID_EMPTY) continue;
				StringIDType sidType = source.m_resource.value();

				if (m_audioBuffers.find(sidType) == m_audioBuffers.end())
					continue;

				alSourcei(source.m_source, AL_BUFFER, m_audioBuffers[sidType]);
				alSourcePlay(source.m_source);
			}
			else if (source.m_playState == ECS::ECAudioSource::PlayState::Paused && state != AL_PAUSED)
			{
				alSourcePause(source.m_source);
			}
			else if (source.m_playState == ECS::ECAudioSource::PlayState::Stopped && state != AL_STOPPED)
			{
				alSourceStop(source.m_source);
			}
			else if (source.m_playState == ECS::ECAudioSource::PlayState::Initial && state != AL_INITIAL)
			{
				alSourceRewind(source.m_source);
			}
		}
	}

	void AudioEngineOpenAL::OnSourceAdded(ECS::Registry& reg, ECS::Entity ent)
	{
		// Generate new source each time a source is added.
		ECS::ECAudioSource& source = reg.get<ECS::ECAudioSource>(ent);
		ECS::ECTransform& transform = reg.get<ECS::ECTransform>(ent);
		Vector3 position = transform.m_position;

		// Set source properties.
		unsigned int sourceBuf;
		alGenSources((ALuint)1, &sourceBuf);
		alSourcef(sourceBuf, AL_PITCH, source.m_pitch);
		alSourcef(sourceBuf, AL_GAIN, source.m_gain);
		alSource3f(sourceBuf, AL_POSITION, position.x, position.y, position.z);
		alSource3f(sourceBuf, AL_VELOCITY, 0, 0, 0);
		alSourcei(sourceBuf, AL_LOOPING, source.m_loop);
		source.m_source = sourceBuf;
		LINA_TRACE("[Audio Engine OpenAL] -> Source generated.");
	}

	void AudioEngineOpenAL::OnSourceRemoved(ECS::Registry& reg, ECS::Entity ent)
	{
		ECS::ECAudioSource& source = reg.get<ECS::ECAudioSource>(ent);
		alDeleteSources(1, &source.m_source);
		LINA_TRACE("[Audio Engine OpenAL] -> Source deleted.");
	}

	void AudioEngineOpenAL::OnAudioResourceLoaded(Event::EAudioResourceLoaded& e)
	{
		// Generate an OpenAL buffer for the loaded audio source.
		std::lock_guard<std::mutex> l(m_mutex);
		ALuint buffer;
		alGenBuffers((ALuint)1, &buffer);
		alBufferData(buffer, e.m_format, e.m_data, e.m_dataSize, e.m_freq);
		m_audioBuffers[e.m_sid] = buffer;
		LINA_TRACE("[Audio Engine OpenAL] -> Buffer generated.");
#ifndef LINA_PRODUCTION_BUILD
		CheckForError();
#endif
	}

	void AudioEngineOpenAL::OnLevelLoaded(Event::ELevelLoaded& e)
	{
		// Initialize groups before main loop.
		m_ecs->group<ECS::ECAudioSource>(ECS_GET<ECS::ECTransform>);
		m_ecs->group<ECS::ECAudioListener>(ECS_GET<ECS::ECTransform>);
	}

	void AudioEngineOpenAL::OnLevelUnloaded(Event::ELevelUnloaded& e)
	{
		// Clear buffers if running on standalone.
		if (m_appMode == ApplicationMode::Standalone)
		{
			CleanUp();
		}
	}

	void AudioEngineOpenAL::CleanUp()
	{
		// Cleanup active sources.
		auto& viewSource = m_ecs->view<ECS::ECAudioSource>();
		for (auto entity : viewSource)
		{
			ECS::ECAudioSource& source = viewSource.get<ECS::ECAudioSource>(entity);
			alDeleteSources(1, &source.m_source);
			LINA_TRACE("[Audio Engine OpenAL] -> Source deleted.");
		}

		// Cleanup buffers.
		for (auto buffer : m_audioBuffers)
		{
			alDeleteBuffers(1, &buffer.second);
			LINA_TRACE("[Audio Engine OpenAL] -> Buffer deleted.");
		}
	}

	void AudioEngineOpenAL::ListAudioDevices(const char* type, const char* list)
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

	void AudioEngineOpenAL::CheckForError()
	{
		ALCenum error;
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			LINA_ERR("[Audio Engine OpenAL] -> Open AL Error {0}", alutGetErrorString(error));
		}
	}


}

