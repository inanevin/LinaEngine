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
#include "GUI/EditorLayoutManager.hpp"
#include "EditorShortcutManager.hpp"
#include "Commands/EditorCommandManager.hpp"
#include "Event/IEditorEventDispatcher.hpp"

namespace Lina
{
	struct ResourceIdentifier;
	class GfxManager;
	class LevelManager;
	class ResourceManager;
	class LGXWrapper;
} // namespace Lina

namespace LinaGX
{
	class Window;
}

namespace Lina::Editor
{
	class GUIDrawerBase;
	class GUINode;
	class GUINodePanel;
	class GUINodeDockArea;

	class Editor : public ISubsystem, public IEditorEventDispatcher, public ISystemEventListener
	{

	private:
		struct CreateWindowRequest
		{
			EditorPanel	  panelType = EditorPanel::None;
			StringID	  panelSid	= 0;
			StringID	  windowSid = 0;
			String		  title		= "";
			GUINodePanel* panel		= nullptr;
			Vector2i	  position	= Vector2i::Zero;
			Vector2ui	  size		= Vector2ui::Zero;
		};

		struct DeleteWindowRequest
		{
			StringID sid;
		};

	public:
		Editor(ISystem* system) : ISubsystem(system, SubsystemType::Editor), m_payloadManager(this), m_layoutManager(this), m_shortcutManager(this), m_commandManager(this){};
		virtual ~Editor() = default;

		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;

		void PackageResources(const Vector<ResourceIdentifier>& identifiers);
		void BeginSplashScreen();
		void EndSplashScreen();
		void PreTick();
		void Tick();
		void OnShortcut(Shortcut sc, LinaGX::Window* windowHandle);

		void			OpenPanel(EditorPanel panel, const String& title, StringID sid, GUINodePanel* srcPanel = nullptr, const Vector2i& pos = Vector2i::Zero, const Vector2ui& size = Vector2ui::Zero);
		LinaGX::Window* CreateChildWindow(StringID sid, const String& title, const Vector2i& pos, const Vector2ui& size);
		void			CloseWindow(StringID sid);
		void			CloseAllChildWindows();
		void			CreateNewLevel(const char* path);
		void			LoadLevel(const char* path);
		void			SaveCurrentLevel();
		void			SaveCurrentLevelAs(const char* path);
		void			UninstallCurrentLevel();
		virtual void	OnSystemEvent(SystemEvent eventType, const Event& ev);
		bool			CheckForDockPreviewPayloads(GUIDrawerBase* owner, GUINodePanel* panel);

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_StartFrame;
		}

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

		inline EditorLayoutManager& GetLayoutManager()
		{
			return m_layoutManager;
		}

		inline EditorCommandManager* GetCommandManager()
		{
			return &m_commandManager;
		}

		static uint32 s_childWindowCtr;

	private:
		ResourceManager*				  m_resourceManager = nullptr;
		LevelManager*					  m_levelManager	= nullptr;
		EditorPayloadManager			  m_payloadManager;
		EditorShortcutManager			  m_shortcutManager;
		EditorCommandManager			  m_commandManager;
		LGXWrapper*						  m_lgxWrapper			= nullptr;
		GfxManager*						  m_gfxManager			= nullptr;
		GUIDrawerBase*					  m_guiDrawerMainWindow = nullptr;
		Vector<DeleteWindowRequest>		  m_deleteWindowRequests;
		Vector<CreateWindowRequest>		  m_createWindowRequests;
		Vector<TextureSheetItem>		  m_editorImages;
		HashMap<StringID, GUIDrawerBase*> m_guiDrawers;
		EditorLayoutManager				  m_layoutManager;
	};
} // namespace Lina::Editor

#endif
