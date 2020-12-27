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

/*
Class:  Audio

Responsible for initializing, running and cleaning up the audio world using OpenAL backend.

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef AudioEngineOpenAL_HPP
#define AudioEngineOpenAL_HPP

#include "Utility/StringId.hpp"
#include "ECS/ECS.hpp"
#include "EventSystem/Events.hpp"
#include "Math/Vector.hpp"
#include <unordered_map>
#include <mutex>

struct ALCcontext;
struct ALCdevice;

namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Audio
{
	class AudioEngineOpenAL 
	{
	public:

		
	private:

		friend class Application;

		AudioEngineOpenAL& operator=(const AudioEngineOpenAL&) = delete;
		AudioEngineOpenAL(const AudioEngineOpenAL&) = delete;
		AudioEngineOpenAL() { };
		~AudioEngineOpenAL();

		void SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs, ApplicationMode appMode);

	private:

		void OnPreMainLoop(Event::EPreMainLoop& e);
		void OnPostMainLoop(Event::EPostMainLoop& e);
		void OnTick();
		void ListAudioDevices(const char* type, const char* list);
		void CheckForError();
		void OnSourceAdded(ECS::Registry& reg, ECS::Entity ent);
		void OnSourceRemoved(ECS::Registry& reg, ECS::Entity ent);
		void OnAudioResourceLoaded(Event::EAudioResourceLoaded& e);
		void OnLevelLoaded(Event::ELevelLoaded& e);
		void OnLevelUnloaded(Event::ELevelUnloaded& e);
		void CleanUp();

	private:

		Event::EventSystem* m_eventSys = nullptr;
		ECS::Registry* m_ecs = nullptr;
		ApplicationMode m_appMode = ApplicationMode::Unknown;
		mutable std::mutex m_mutex;
		std::unordered_map<StringIDType, unsigned int> m_audioBuffers;
		ALCcontext* m_context = nullptr;
		ALCdevice* m_device = nullptr;
		Vector3 m_mainListenerLastPos = V3_ZERO;
	};

}



#endif