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
Class: ECAudioSource

Entity component representing an audio source.

Timestamp: 12/22/2020 8:02:34 PM
*/

#pragma once

#ifndef ECAudioSource_HPP
#define ECAudioSource_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Core/AudioBackendFwd.hpp"
#include "Math/Vector.hpp"

namespace Lina::ECS
{
	struct ECAudioSource
	{
		enum class PlayState
		{
			Initial = 1 << 0,
			Playing = 1 << 1,
			Paused = 1 << 2,
			Stopped = 1 << 3
		};

		StringID m_resource = SID_EMPTY;
		PlayState m_playState = PlayState::Initial;
		float m_pitch = 1.0f;
		float m_gain = 1.0f;
		float m_3dFactor = 0.0f;
		bool m_loop = false;

	private:

		friend class AUDIO_ENGINE_FWD;
		friend class cereal::access;

		// INTERNAL USE ONLY!
		unsigned int m_source = 0;
		bool m_isPlaying = false;
		Vector3 m_previousPosition = V3_ZERO;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_pitch, m_gain, m_playState, m_3dFactor);
		}


	};
}

#endif
