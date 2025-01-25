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

#include "Core/Audio/Audio.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Audio/Loaders/AudioFile.h"
#include <AL/al.h>

namespace Lina
{

#define AL_CHECK_ERR(err)                                                                                                                                                                                                                                          \
	err = alGetError();                                                                                                                                                                                                                                            \
	if (err != AL_NO_ERROR)

	void Audio::Metadata::SaveToStream(OStream& out) const
	{
	}

	void Audio::Metadata::LoadFromStream(IStream& in)
	{
	}

	Audio::~Audio()
	{
	}

	bool Audio::LoadFromFile(const String& path)
	{
		m_audioBuffer.resize(0);
		AudioFile<int16> file;
		file.load(path.c_str());

		m_sampleRate = file.getSampleRate();
		m_bitDepth	 = file.getBitDepth();
		m_samples	 = file.getNumSamplesPerChannel();
		m_length	 = file.getLengthInSeconds();
		m_channels	 = file.getNumChannels();

		m_format = 0;
		if (m_bitDepth == 8)
		{
			if (m_channels == 1)
				m_format = AL_FORMAT_MONO8;
			else if (m_channels == 2)
				m_format = AL_FORMAT_MONO16;
		}
		else if (m_bitDepth == 16)
		{
			if (m_channels == 1)
				m_format = AL_FORMAT_MONO16;
			else if (m_channels == 2)
				m_format = AL_FORMAT_STEREO16;
		}

		if (m_format == 0)
		{
			LINA_ERR("Failed determining audio format!");
			return false;
		}

		for (uint32 i = 0; i < m_samples; i++)
		{
			for (uint32 ch = 0; ch < m_channels; ch++)
			{
				int16_t sample = file.samples[ch][i];
				m_audioBuffer.push_back(static_cast<unsigned char>(sample & 0xFF));		   // Low byte
				m_audioBuffer.push_back(static_cast<unsigned char>((sample >> 8) & 0xFF)); // High byte
			}
		}

		LINA_TRACE("Loaded audio: {0}, {1}, {2}, {3}", m_sampleRate, m_bitDepth, m_samples, m_length);
		return true;
	}

	void Audio::LoadFromStream(IStream& stream)
	{
		m_audioBuffer.resize(0);
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_meta;
		stream >> m_audioBuffer;
		stream >> m_sampleRate >> m_bitDepth >> m_samples >> m_length >> m_channels >> m_format;
	}

	void Audio::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_meta;
		stream << m_audioBuffer;
		stream << m_sampleRate << m_bitDepth << m_samples << m_length << m_channels << m_format;
	}

	size_t Audio::GetSize() const
	{
		return 0;
	}

	void Audio::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");
		m_hwValid = true;
		int err	  = 0;
		alGenBuffers((ALuint)1, &m_buffer);
		AL_CHECK_ERR(err);
		alBufferData(m_buffer, (ALenum)m_format, (ALvoid*)m_audioBuffer.data(), (ALsizei)m_audioBuffer.size(), (ALsizei)m_sampleRate);
		AL_CHECK_ERR(err);

		m_audioBuffer.clear();
	}

	void Audio::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;
		alDeleteBuffers(1, &m_buffer);
	}

} // namespace Lina
