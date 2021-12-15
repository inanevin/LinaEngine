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
Class: Audio



Timestamp: 12/15/2021 2:12:28 PM
*/

#pragma once

#ifndef Audio_HPP
#define Audio_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include <string>
#include <map>

namespace Lina::Audio
{
	class Audio
	{
		
	public:
		
		Audio() {}
		~Audio();
	
		static Audio& CreateAudio(const std::string& path);
		static std::map<StringIDType, Audio>& GetLoadedAudios() { return s_loadedAudios; }
		static void UnloadAudio(StringIDType sid);
		static void UnloadAudio(const std::string& path);
		static void UnloadAll();
		static bool AudioExists(const std::string& path);
		static bool AudioExists(StringIDType sid);
		static Audio& GetAudio(StringIDType sid);
		static Audio& GetAudio(const std::string& path);

		unsigned int GetBuffer() { return m_buffer; }
		StringIDType GetSID() { return m_sid; }
	private:

		static void CheckForError();

	private:

		static std::map<StringIDType, Audio> s_loadedAudios;
		int m_size = 0;
		StringIDType m_sid;
		float m_freq = 0.0f;
		int m_format = 0;
		void* m_data = nullptr;
		unsigned int m_buffer = 0;
	};
}

#endif
