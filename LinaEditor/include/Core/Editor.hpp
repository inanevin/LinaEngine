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

namespace Lina
{
	struct ResourceIdentifier;
}

namespace Lina::Editor
{
	class EditorGUIDrawer;

	class Editor : public ISubsystem
	{
	public:
		Editor(ISystem* system) : ISubsystem(system, SubsystemType::Editor){};
		virtual ~Editor() = default;

		void PackageResources(const Vector<ResourceIdentifier>& identifiers);
		void BeginSplashScreen();
		void EndSplashScreen();

		void OpenPanel(EditorPanel panel);
		void ClosePanel(EditorPanel panel);

		// Inherited via ISubsystem
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;

		inline const TextureSheetItem GetEditorImage(uint32 index)
		{
			return m_editorImages[index];
		}

	private:
		Mutex								   m_mtx;
		EditorGUIDrawer*					   m_mainWindowGUIDrawer = nullptr;
		HashMap<StringID, EditorGUIDrawer*>	   m_guiDrawers;
		HashMap<EditorPanel, EditorGUIDrawer*> m_openPanels;
		Vector<TextureSheetItem>			   m_editorImages;
	};
} // namespace Lina::Editor

#endif
