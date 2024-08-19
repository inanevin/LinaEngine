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

#include "Common/Data/Vector.hpp"
#include "Common/StringID.hpp"
#include "Editor/CommonEditor.hpp"

namespace LinaGX
{
	class Window;
}

namespace Lina
{
	class GfxManager;
	class Widget;
	class WidgetManager;

} // namespace Lina
namespace Lina::Editor
{
	class Editor;
	class NotificationDisplayer;
	class SurfaceRenderer;

	class EditorPayloadListener
	{
	public:
		virtual void OnPayloadStarted(PayloadType type, Widget* payload)
		{
		}
		virtual void OnPayloadEnded(PayloadType type, Widget* payload)
		{
		}
		virtual bool OnPayloadDropped(PayloadType type, Widget* payload)
		{
			return false;
		}

		virtual LinaGX::Window* OnPayloadGetWindow()
		{
			return nullptr;
		}
	};

	struct PayloadRequest
	{
		Widget*			payload		 = nullptr;
		LinaGX::Window* sourceWindow = nullptr;
		PayloadType		type		 = PayloadType::DockedPanel;
		bool			active		 = false;
		Vector2ui		size		 = Vector2ui::Zero;
	};

	class WindowPanelManager
	{
	public:
		void			 Initialize(Editor* editor);
		void			 PreTick();
		void			 Shutdown();
		SurfaceRenderer* GetSurfaceRenderer(StringID sid);

		// Payload
		void	AddPayloadListener(EditorPayloadListener* listener);
		void	RemovePayloadListener(EditorPayloadListener* listener);
		void	CreatePayload(Widget* payload, PayloadType type, const Vector2ui& size);
		Widget* GetPayloadRoot();

		// Panel and windows
		void	OpenPanel(PanelType type, StringID subData, Widget* requestingWidget);
		Widget* PrepareNewWindowToDock(StringID sid, const Vector2& pos, const Vector2& size, const String& title);
		void	CloseWindow(StringID sid);
		void	CloseAllSubwindows();
		void	OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& size);

		// Notification
		NotificationDisplayer* GetNotificationDisplayer(LinaGX::Window* window);

		inline const Vector<LinaGX::Window*>& GetSubWindows() const
		{
			return m_subWindows;
		}

		inline LinaGX::Window* GetMainWindow() const
		{
			return m_mainWindow;
		}

	private:
		void CreateSurfaceRendererForWindow(LinaGX::Window* window);
		void DestroySurfaceRenderer(LinaGX::Window* window);

	private:
		WidgetManager*							   m_primaryWidgetManager = nullptr;
		Editor*									   m_editor				  = nullptr;
		GfxManager*								   m_gfxManager			  = nullptr;
		LinaGX::Window*							   m_payloadWindow		  = nullptr;
		LinaGX::Window*							   m_mainWindow			  = nullptr;
		Vector<StringID>						   m_windowCloseRequests;
		Vector<LinaGX::Window*>					   m_subWindows = {};
		Vector<EditorPayloadListener*>			   m_payloadListeners;
		HashMap<LinaGX::Window*, SurfaceRenderer*> m_surfaceRenderers;
		PayloadRequest							   m_payloadRequest;
		StringID								   m_subWindowCounter = 0;
	};

} // namespace Lina::Editor
