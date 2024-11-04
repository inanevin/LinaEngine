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
#include "Editor/Widgets/Panel/Panel.hpp"

namespace LinaGX
{
	class Window;
}

namespace Lina
{
	class Widget;
	class WidgetManager;

} // namespace Lina
namespace Lina::Editor
{
	class DockArea;
	class Editor;
	class NotificationDisplayer;
	class SurfaceRenderer;
	class PanelColorWheel;

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

	struct WindowPanelInfo
	{
		Vector2i   position = Vector2i::Zero;
		Vector2ui  size		= Vector2ui::Zero;
		ResourceID subdata	= 0;
	};

	class WindowPanelManager
	{
	public:
		void			 Initialize(Editor* editor);
		void			 PreTick();
		void			 Shutdown();
		SurfaceRenderer* GetSurfaceRenderer(StringID sid);
		Widget*			 LockAllForegrounds(LinaGX::Window* srcWindow, const String& text);
		void			 UnlockAllForegrounds();

		// Payload
		void	AddPayloadListener(EditorPayloadListener* listener);
		void	RemovePayloadListener(EditorPayloadListener* listener);
		void	CreatePayload(Widget* payload, PayloadType type, const Vector2ui& size);
		Widget* GetPayloadRoot();

		// Panel and windows
		Panel*	OpenPanel(PanelType type, ResourceID subData, Widget* requestingWidget);
		Widget* PrepareNewWindowToDock(const Vector2& pos, const Vector2& size);
		void	CloseWindow(StringID sid);
		void	CloseAllSubwindows();
		void	OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& size);
		void	OnPanelDestruct(Panel* panel);
		Panel*	FindPanelOfType(PanelType type, ResourceID subData, DockArea*& owningArea);
		Panel*	FindPanelOfType(PanelType type, ResourceID subData);

		PanelColorWheel* OpenColorWheelPanel(Widget* requester);

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

		inline Widget* GetMainLock() const
		{
			return m_mainLock;
		}

	private:
		void   CreateSurfaceRendererForWindow(LinaGX::Window* window);
		void   DestroySurfaceRenderer(LinaGX::Window* window);
		Panel* FindPanelFromDockAreas(PanelType type, const Vector<DockArea*>& areas, DockArea*& outOwningArea, ResourceID subData);

	private:
		WidgetManager*							   m_primaryWidgetManager = nullptr;
		Editor*									   m_editor				  = nullptr;
		LinaGX::Window*							   m_payloadWindow		  = nullptr;
		LinaGX::Window*							   m_mainWindow			  = nullptr;
		Vector<StringID>						   m_windowCloseRequests;
		Vector<LinaGX::Window*>					   m_subWindows = {};
		Vector<EditorPayloadListener*>			   m_payloadListeners;
		HashMap<LinaGX::Window*, SurfaceRenderer*> m_surfaceRenderers;
		PayloadRequest							   m_payloadRequest;
		StringID								   m_subWindowCounter = 0;
		HashMap<PanelType, WindowPanelInfo>		   m_windowPanelInfos;
		Widget*									   m_panelColorWheelOwner = nullptr;
		PanelColorWheel*						   m_panelColorWheel	  = nullptr;
		Widget*									   m_mainLock			  = nullptr;
	};

} // namespace Lina::Editor
