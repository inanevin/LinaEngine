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
Class: RenderSettings

Contains the internal render settings, responsible for their serialization.

Timestamp: 10/30/2020 9:17:05 AM
*/

#pragma once

#ifndef RenderSettings_HPP
#define RenderSettings_HPP

#include <string>

namespace LinaEngine
{
	class RenderSettings
	{
		
	public:
		
		RenderSettings() {};
		~RenderSettings() {};

		static void SerializeRenderSettings(RenderSettings& settings, const std::string& path, const std::string& fileName);
		static RenderSettings DeserializeRenderSettings(const std::string& path, const std::string& fileName);
		
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_bloomEnabled, m_fxaaEnabled, m_fxaaReduceMin, m_fxaaReduceMul, m_fxaaSpanMax, m_gamma, m_exposure);
		}
	
	private:
	
		bool m_bloomEnabled = false;
		bool m_fxaaEnabled = false;
		float m_fxaaReduceMin = 1.0f / 128.0f;
		float m_fxaaReduceMul = 1.0f / 8.0f;
		float m_fxaaSpanMax = 8.0f;
		float m_gamma = 2.2f;
		float m_exposure = 1.0f;
	};
}

#endif
