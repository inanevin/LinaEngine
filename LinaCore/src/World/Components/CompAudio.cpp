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

#include "Core/World/Components/CompAudio.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/World/EntityWorld.hpp"
#include <AL/al.h>

namespace Lina
{
#define AL_CHECK_ERR(err)                                                                                                                                                                                                                                          \
	err = alGetError();                                                                                                                                                                                                                                            \
	if (err != AL_NO_ERROR)

	CompAudio::~CompAudio()
	{
		if (m_generated)
		{
			m_generated = false;
			alDeleteSources((ALuint)1, &m_alSource);
		}
	}

	void CompAudio::BeginPlay()
	{
		if (m_playOnBegin)
			Play();
	}

	void CompAudio::CollectReferences(HashSet<ResourceID>& refs)
	{
		refs.insert(m_audio);
	}

	void CompAudio::StoreReferences()
	{
		m_audioPtr = m_resourceManager->GetIfExists<Audio>(m_audio);
		if (!m_audioPtr)
			return;
		int err = 0;

		if (!m_generated)
		{
			m_generated = true;
			alGenSources((ALuint)1, &m_alSource);
			AL_CHECK_ERR(err);
		}

		alSourcei(m_alSource, AL_BUFFER, m_audioPtr->GetHW());
		AL_CHECK_ERR(err);
	}

	void CompAudio::Play()
	{
		CheckSource();
		SetupProperties();
		alSourcePlay(m_alSource);
		int err = 0;
		AL_CHECK_ERR(err);
	}

	void CompAudio::Stop()
	{
		CheckSource();
		alSourcePause(m_alSource);
	}

	void CompAudio::Pause()
	{
		CheckSource();
		alSourcePause(m_alSource);
	}

	void CompAudio::Rewind()
	{
		CheckSource();
		alSourceRewind(m_alSource);
	}

	void CompAudio::SetupProperties()
	{
		if (m_is3D)
			alListener3f(AL_POSITION, m_entity->GetPosition().x, m_entity->GetPosition().y, m_entity->GetPosition().z);
		else
		{
			const Vector3& camPos = m_world->GetWorldCamera().GetPosition();
			alSource3f(m_alSource, AL_POSITION, camPos.x, camPos.y, camPos.z);
		}
		int err = 0;

		alSourcef(m_alSource, AL_PITCH, m_pitch);
		AL_CHECK_ERR(err);
		alSourcef(m_alSource, AL_GAIN, m_gain);
		AL_CHECK_ERR(err);
		alSourcei(m_alSource, AL_LOOPING, (ALint)m_isLooping);
		AL_CHECK_ERR(err);
	}

	void CompAudio::CheckSource()
	{
		if (!m_generated)
		{
			if (!m_audioPtr)
				return;

			int err		= 0;
			m_generated = true;
			alGenSources((ALuint)1, &m_alSource);
			AL_CHECK_ERR(err);

			alSourcei(m_alSource, AL_BUFFER, m_audioPtr->GetHW());
			AL_CHECK_ERR(err);
		}
	}

} // namespace Lina
