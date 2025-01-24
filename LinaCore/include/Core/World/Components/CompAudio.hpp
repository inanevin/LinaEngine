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

#pragma once

#include "Core/World/Component.hpp"
#include "Core/Audio/Audio.hpp"

namespace Lina
{
	class CompAudio : public Component
	{
	public:
		CompAudio() : Component(GetTypeID<CompAudio>(), 0){};
		virtual ~CompAudio();

		virtual void BeginPlay() override;
		virtual void CollectReferences(HashSet<ResourceID>& refs) override;
		virtual void StoreReferences() override;

		void Play();
		void Stop();
		void Pause();
		void Rewind();

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_audio << m_is3D << m_isLooping << m_pitch << m_gain << m_playOnBegin;
		}

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_audio >> m_is3D >> m_isLooping >> m_pitch >> m_gain >> m_playOnBegin;
		}

		inline void SetAudio(ResourceID id)
		{
			m_audio = id;
		}

		inline ResourceID GetAudio() const
		{
			return m_audio;
		}

		inline Audio* GetAudioPtr() const
		{
			return m_audioPtr;
		}

		inline bool GetIs3D() const
		{
			return m_is3D;
		}

	private:
		void SetupProperties();

	private:
		LINA_REFLECTION_ACCESS(CompAudio);
		ResourceID m_audio = 0;

		bool  m_playOnBegin = true;
		bool  m_is3D		= true;
		bool  m_isLooping	= false;
		float m_pitch		= 1.0f;
		float m_gain		= 1.0f;

		Audio* m_audioPtr = nullptr;

		bool   m_generated = false;
		uint32 m_alSource  = 0;
	};

	LINA_COMPONENT_BEGIN(CompAudio, "Audio")
	LINA_FIELD(CompAudio, m_audio, "Audio", FieldType::ResourceID, GetTypeID<Audio>())
	LINA_FIELD(CompAudio, m_playOnBegin, "Play On Begin", FieldType::Boolean, 0)
	LINA_FIELD(CompAudio, m_is3D, "Is 3D", FieldType::Boolean, 0)
	LINA_FIELD(CompAudio, m_isLooping, "Loop", FieldType::Boolean, 0)
	LINA_FIELD(CompAudio, m_pitch, "Pitch", FieldType::Float, 0)
	LINA_FIELD(CompAudio, m_gain, "Gain", FieldType::Float, 0)
	LINA_CLASS_END(CompAudio)

} // namespace Lina
