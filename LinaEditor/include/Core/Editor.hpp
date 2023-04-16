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

#ifndef LinaEditor_HPP
#define LinaEditor_HPP

#include "Data/Vector.hpp"
#include "System/ISubsystem.hpp"
#include "EditorCommon.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "GUI/EditorPayloadManager.hpp"

namespace Lina
{
	struct ResourceIdentifier;
	class GfxManager;
	class WindowManager;
	class IWindow;
	class Input;
} // namespace Lina

namespace Lina::Editor
{
	class GUIDrawerBase;
	class GUINode;
	class GUINodePanel;
	class GUINodeDockArea;

	class Editor : public ISubsystem
	{

	private:
		struct CreateWindowRequest
		{
			EditorPanel panelType = EditorPanel::None;
			StringID	sid		  = 0;
			String		title	  = "";
		};

		struct DeleteWindowRequest
		{
			StringID sid;
		};

	public:
		Editor(ISystem* system) : ISubsystem(system, SubsystemType::Editor), m_payloadManager(this){};
		virtual ~Editor() = default;

		void PackageResources(const Vector<ResourceIdentifier>& identifiers);
		void BeginSplashScreen();
		void EndSplashScreen();
		void Tick();

		void OpenPanel(EditorPanel panel, const String& title, StringID sid);
		void CloseWindow(StringID sid);
		void OnWindowDrag(GUIDrawerBase* owner, bool isDragging);

		// Inherited via ISubsystem
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;

		inline const TextureSheetItem GetEditorImage(uint32 index)
		{
			return m_editorImages[index];
		}

		inline EditorPayloadManager& GetPayloadManager()
		{
			return m_payloadManager;
		}

		inline const HashMap<StringID, GUIDrawerBase*>& GetGUIDrawers() const
		{
			return m_guiDrawers;
		}

	private:
		Input*							  m_input = nullptr;
		EditorPayloadManager			  m_payloadManager;
		WindowManager*					  m_windowManager		= nullptr;
		GfxManager*						  m_gfxManager			= nullptr;
		GUIDrawerBase*					  m_guiDrawerMainWindow = nullptr;
		Vector<DeleteWindowRequest>		  m_deleteWindowRequests;
		Vector<CreateWindowRequest>		  m_createWindowRequests;
		Vector<TextureSheetItem>		  m_editorImages;
		HashMap<StringID, GUIDrawerBase*> m_guiDrawers;
	};
} // namespace Lina::Editor

#endif
