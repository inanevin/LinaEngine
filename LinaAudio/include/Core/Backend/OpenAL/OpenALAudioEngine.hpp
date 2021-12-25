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

/*
Class:  Audio

Responsible for initializing, running and cleaning up the audio world.

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef AudioEngine_HPP
#define AudioEngine_HPP

#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"
#include <map>
#include <unordered_map>
#include <mutex>

struct ALCcontext;
struct ALCdevice;


namespace Lina
{
	class Engine;

	namespace World
	{
		class Level;
	}

	namespace ECS
	{
		class Registry;
	}

	namespace Event
	{
		struct ELoadResourceFromFile;
		struct ELoadResourceFromMemory;
	}
}

namespace Lina::Audio
{
	class Audio;

	class OpenALAudioEngine
	{
	public:

		void PlayOneShot(Audio& audio, float gain = 1.0f, bool looping = false, float pitch = 1.0f, Vector3 position = Vector3::Zero, Vector3 velocity = Vector3::Zero);
		static OpenALAudioEngine* Get() { return s_audioEngine; }

	private:

		void OnLoadResourceFromFile(const Event::ELoadResourceFromFile& ev);
		void OnLoadResourceFromMemory(const Event::ELoadResourceFromMemory& ev);
		void ListAudioDevices(const char* type, const char* list);

	private:

		friend class Engine;
		OpenALAudioEngine() {};
		~OpenALAudioEngine() {};
		void Initialize();	
		void Shutdown();

	private:

		static OpenALAudioEngine* s_audioEngine;
		ECS::Registry* m_ecs = nullptr;
		mutable std::mutex m_mutex;
		ALCcontext* m_context = nullptr;
		ALCdevice* m_device = nullptr;
		Vector3 m_mainListenerLastPos = Vector3::Zero;
		std::map<StringIDType, unsigned int> m_generatedSources;
	};
}


#endif