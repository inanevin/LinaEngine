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
#include "Common/Serialization/VectorSerialization.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/System/System.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Audio/miniaudio/miniaudio.h"
#include "Core/Audio/AudioManager.hpp"

namespace Lina
{
	void Audio::Metadata::SaveToStream(OStream& out) const
	{
	}

	void Audio::Metadata::LoadFromStream(IStream& in)
	{
	}

	Audio::~Audio()
	{
	}

	void Audio::Upload()
	{
	}

	void Audio::LoadFromFile(const char* path)
	{

		IStream stream = Serialization::LoadFromFile(path);

		ma_decoder decoder;

		ma_result result = ma_decoder_init_memory((void*)stream.GetDataRaw(), stream.GetSize(), NULL, &decoder);
		if (result != MA_SUCCESS)
		{
			return result; // Handle error
		}
		ma_engine& engine = m_resourceManager->GetSystem()->CastSubsystem<AudioManager>(SubsystemType::AudioManager)->GetEngine();

		ma_sound sound;
		result = ma_sound_init_from_data_source(&engine, &decoder, MA_SOUND_FLAG_DECODE, NULL, &sound);
		if (result != MA_SUCCESS)
		{
			ma_decoder_uninit(&decoder); // Clean up decoder if sound initialization fails
			return result;				 // Handle error
		}
	}

	void Audio::LoadFromStream(IStream& stream)
	{
		m_meta.LoadFromStream(stream);
	}

	void Audio::SaveToStream(OStream& stream) const
	{
		m_meta.SaveToStream(stream);
	}

} // namespace Lina
