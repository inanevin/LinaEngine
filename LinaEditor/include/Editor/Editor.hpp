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

#include "Meta/EditorSettings.hpp"
#include "Common/System/Subsystem.hpp"

namespace Lina
{
	class ProjectData;
	class Application;
	class GfxManager;
	class WidgetManager;
} // namespace Lina

namespace Lina::Editor
{
	class SplashScreen;

	class Editor : public Subsystem
	{
	public:
		Editor(System* sys) : Subsystem(sys, SubsystemType::Editor){};
		virtual ~Editor() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void CoreResourcesLoaded() override;
		virtual void Shutdown() override;

		void OpenPopupProjectSelector(bool canCancel);

		void OpenProject(const String& projectFile);
		void SaveProjectChanges();
		void CloseCurrentProject();
		void RequestExit();

		inline void SetProjectDirty(bool isDirty)
		{
			m_isProjectDirty = isDirty;
		}

	private:
		void RemoveCurrentProject();
		void CreateEmptyProjectAndOpen(const String& path);

	private:
		GfxManager*	   m_gfxManager			  = nullptr;
		WidgetManager* m_primaryWidgetManager = nullptr;
		EditorSettings m_settings			  = {};
		ProjectData*   m_currentProject		  = nullptr;
		bool		   m_isProjectDirty		  = false;
	};

} // namespace Lina::Editor
