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

#ifndef EditorPayloadManager_HPP
#define EditorPayloadManager_HPP

#include "Core/EditorCommon.hpp"

namespace Lina
{
	class IWindow;
	class WindowManager;
	class GfxManager;
	class Input;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class GUIDrawerPayload;
	class GUINodePanel;

	struct PayloadMeta
	{
		Vector2		delta = Vector2::Zero;
		void*		data  = nullptr;
		PayloadType type  = PayloadType::EPL_None;
	};
	class EditorPayloadManager
	{
	public:
		EditorPayloadManager(Editor* editor);
		~EditorPayloadManager() = default;

		void Initialize();
		void Tick();
		void Shutdown();
		void CreatePayload(PayloadType type, const Vector2i& windowSize, const Vector2i& delta, void* userData);

		inline const PayloadMeta& GetCurrentPayloadMeta() const
		{
			return m_currentPayloadMeta;
		}

	private:
		Editor*			  m_editor		  = nullptr;
		WindowManager*	  m_windowManager = nullptr;
		GfxManager*		  m_gfxManager	  = nullptr;
		Input*			  m_input		  = nullptr;
		IWindow*		  m_window		  = nullptr;
		GUIDrawerPayload* m_guiDrawer	  = nullptr;
		PayloadMeta		  m_currentPayloadMeta;
	};
} // namespace Lina::Editor

#endif
