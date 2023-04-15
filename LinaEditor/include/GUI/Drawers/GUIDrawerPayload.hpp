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

#ifndef GUIDrawerPayload_HPP
#define GUIDrawerPayload_HPP

#include "GUIDrawerBase.hpp"
#include "Core/EditorCommon.hpp"

namespace Lina::Editor
{
	class GUINodeTitleSection;

	class GUIDrawerPayload : public GUIDrawerBase
	{
	public:
		GUIDrawerPayload(Editor* editor, ISwapchain* swap);
		virtual ~GUIDrawerPayload(){};
		virtual void DrawGUI(int threadID) override;

		inline void SetPayloadType(PayloadType type)
		{
			m_payloadType = type;
		}

	private:
		PayloadType m_payloadType = PayloadType::Panel;
	};
} // namespace Lina::Editor

#endif
