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

#include "Audio/Audio.hpp"
#include "Log/Log.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <fstream>
#include <cereal/archives/portable_binary.hpp>

namespace Lina::Audio
{
	std::map<StringIDType, Audio> Audio::s_loadedAudios;

	Audio::~Audio()
	{
		alDeleteBuffers(1, &m_buffer);
	}

	AudioParameters Audio::LoadParameters(const std::string& path)
	{
		AudioParameters params;
		std::ifstream stream(path);
		{
			cereal::PortableBinaryInputArchive iarchive(stream);
			iarchive(params);
		}
		return params;
	}

	void Audio::SaveParameters(const std::string& path, AudioParameters params)
	{
		std::ofstream stream(path);
		{
			cereal::PortableBinaryOutputArchive oarchive(stream);
			oarchive(params);
		}
	}

	AudioParameters Audio::LoadParametersFromMemory(unsigned char* data, size_t dataSize)
	{
		AudioParameters params;

		{
			std::string data((char*)data, dataSize);
			std::istringstream stream(data);
			{
				cereal::PortableBinaryInputArchive iarchive(stream);
				iarchive(params);
			}
		}

		return params;
	}


	Audio& Audio::CreateAudioFromMemory(const std::string& path, unsigned char* data, size_t dataSize, AudioParameters& params)
	{
		ALsizei size;
		ALfloat freq;
		ALenum format;
		ALvoid* aldata = alutLoadMemoryFromFileImage(data, dataSize, &format, &size, &freq);

		ALenum err = alutGetError();
		LINA_ASSERT(err == ALUT_ERROR_NO_ERROR, "[Audio Loader] -> Failed loading audio from file memory: {0} {1}", path, alutGetErrorString(err));

		StringIDType sid = StringID(path.c_str()).value();
		Audio& aud = s_loadedAudios[sid];
		aud.m_data = aldata;
		aud.m_format = format;
		aud.m_size = size;
		aud.m_freq = freq;
		aud.m_sid = sid;
		aud.m_params = params;

		alGenBuffers((ALuint)1, &aud.m_buffer);
		alBufferData(aud.m_buffer, format, aldata, size, freq);
		free(aldata);

#ifndef LINA_PRODUCTION_BUILD
		CheckForError();
#endif

		return s_loadedAudios[sid];
	}

	Audio& Audio::CreateAudio(const std::string& path, AudioParameters& params)
    {
		ALsizei size;
		ALfloat freq;
		ALenum format;
		ALvoid* data = alutLoadMemoryFromFile(path.c_str(), &format, &size, &freq);

		ALenum err = alutGetError();
		LINA_ASSERT(err == ALUT_ERROR_NO_ERROR, "[Audio Loader] -> Failed loading audio from file: {0} {1}", path, alutGetErrorString(err));

		StringIDType sid = StringID(path.c_str()).value();
		Audio& aud = s_loadedAudios[sid];
		aud.m_data = data;
		aud.m_format = format;
		aud.m_size = size;
		aud.m_freq = freq;
		aud.m_sid = sid;
		aud.m_params = params;

		alGenBuffers((ALuint)1, &aud.m_buffer);
		alBufferData(aud.m_buffer, format, data, size, freq);
		free(data);

#ifndef LINA_PRODUCTION_BUILD
		CheckForError();
#endif

		return s_loadedAudios[sid];
    }

	void Audio::UnloadAudio(StringIDType sid)
	{
		bool exists = AudioExists(sid);
		LINA_ASSERT(exists, "Audio does not exist!");
		s_loadedAudios.erase(sid);
	}

	void Audio::UnloadAudio(const std::string& path)
	{
		UnloadAudio(StringID(path.c_str()).value());
	}

	void Audio::UnloadAll()
	{
		s_loadedAudios.clear();
	}

	bool Audio::AudioExists(const std::string& path)
	{
		return AudioExists(StringID(path.c_str()).value());
	}

	bool Audio::AudioExists(StringIDType sid)
	{
		if (sid < 0) return false;
		return !(s_loadedAudios.find(sid) == s_loadedAudios.end());
	}

	Audio& Audio::GetAudio(StringIDType sid)
	{
		bool exists = AudioExists(sid);
		LINA_ASSERT(exists, "Audio does not exist!");
		return s_loadedAudios[sid];
	}

	Audio& Audio::GetAudio(const std::string& path)
	{
		return GetAudio(StringID(path.c_str()).value());
	}

	void Audio::CheckForError()
	{
		ALCenum error;
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			LINA_ERR("[Audio Engine OpenAL] -> Open AL Error {0}", alutGetErrorString(error));
		}
	}
}