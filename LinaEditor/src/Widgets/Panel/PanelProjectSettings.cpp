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

#include "Editor/Widgets/Panel/PanelProjectSettings.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Actions/EditorActionSettings.hpp"
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"

namespace Lina::Editor
{
	void PanelProjectSettings::Construct()
	{
		m_editor = Editor::Get();

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(scroll);

		m_layout = m_manager->Allocate<DirectionalLayout>("Inspector");
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);
		m_layout->GetProps().direction				 = DirectionOrientation::Vertical;
		m_layout->GetWidgetProps().childPadding		 = Theme::GetDef().baseIndentInner;
		m_layout->GetWidgetProps().clipChildren		 = true;
		m_layout->GetWidgetProps().childMargins.left = Theme::GetDef().baseBorderThickness;
		m_layout->GetWidgetProps().childMargins.top	 = Theme::GetDef().baseIndent;
		scroll->AddChild(m_layout);

		ProjectData* pj = m_editor->GetProjectManager().GetProjectData();
		if (pj)
			m_settingsPackaging = pj->GetSettingsPackaging();

		BuildSettingsPackaging();
	}

	void PanelProjectSettings::SetSettingsPackaging(const PackagingSettings& settings)
	{
		m_settingsPackaging = settings;
		BuildSettingsPackaging();
		m_editor->GetProjectManager().GetProjectData()->GetSettingsPackaging() = m_settingsPackaging;
		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void PanelProjectSettings::BuildSettingsPackaging()
	{
		if (m_foldPackaging)
		{
			m_manager->Deallocate(m_foldPackaging);
			m_layout->RemoveChild(m_foldPackaging);
		}

		m_foldPackaging								  = CommonWidgets::BuildFoldTitle(this, Locale::GetStr(LocaleStr::Packaging), &m_foldValuePackaging);
		m_foldPackaging->GetCallbacks().onEditStarted = [this]() { m_oldSettingsPackaging = m_settingsPackaging; };
		m_foldPackaging->GetCallbacks().onEditEnded	  = [this]() {
			  EditorActionSettingsPackaging::Create(m_editor, m_oldSettingsPackaging, m_settingsPackaging);
			  m_editor->GetProjectManager().GetProjectData()->GetSettingsPackaging() = m_settingsPackaging;
			  m_editor->GetProjectManager().SaveProjectChanges();
		};
		m_layout->AddChild(m_foldPackaging);

		CommonWidgets::BuildClassReflection(m_foldPackaging, &m_settingsPackaging, ReflectionSystem::Get().Meta<PackagingSettings>());

		Button* buttonPackage = m_manager->Allocate<Button>("Package");
		buttonPackage->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		buttonPackage->SetAlignedPosX(0.5f);
		buttonPackage->SetAlignedSizeX(0.5f);
		buttonPackage->SetAnchorX(Anchor::Center);
		buttonPackage->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		buttonPackage->GetText()->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::BuildPackage));
		buttonPackage->GetProps().onClicked = [this]() { PackageProject(); };
		m_foldPackaging->AddChild(buttonPackage);

		m_foldPackaging->Initialize();
	}

	void PanelProjectSettings::PackageProject()
	{
		EditorTask* task   = m_editor->GetTaskManager().CreateTask();
		task->ownerWindow  = GetWindow();
		task->title		   = Locale::GetStr(LocaleStr::Packaging);
		task->progressText = Locale::GetStr(LocaleStr::Working);

		ProjectData* pj = m_editor->GetProjectManager().GetProjectData();

		const String folder = FileSystem::GetFilePath(pj->GetPath());

		const String configuration = LINA_CONFIGURATION;

#ifdef LINA_PLATFORM_WINDOWS
		const String gameProjectFolder = folder + "GameProject/bin/" + configuration + "/";
#else
		const String gameProjectFolder = folder + "GameProject/bin/" + configuration + "/";
#endif

		if (!FileSystem::FileOrPathExists(gameProjectFolder))
		{
			NotificationDisplayer* notificationDisplayer = m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow());
			notificationDisplayer->AddNotification({
				.icon				= NotificationIcon::Err,
				.title				= Locale::GetStr(LocaleStr::GameProjectOutDoesntExist),
				.autoDestroySeconds = 5,
			});
			return;
		}

		task->task = [task, pj, gameProjectFolder]() {
			OStream package0Stream;
			OStream package1Stream;

			OStream projectDataStream;
			pj->SaveToStream(projectDataStream);

			// Write project data first.
			const uint32 pjSize = static_cast<uint32>(projectDataStream.GetCurrentSize());
			package0Stream << pjSize;
			package0Stream.WriteRaw(projectDataStream.GetDataRaw(), projectDataStream.GetCurrentSize());
			projectDataStream.Destroy();

			const PackagingSettings& packagingSettings = pj->GetSettingsPackaging();

			ResourceDefinitionList allResourceNeeds;

			for (ResourceID id : packagingSettings.worldIDsToPack)
			{
				const String path = pj->GetResourcePath(id);
				if (!FileSystem::FileOrPathExists(path))
					continue;

				EntityWorld w(0, "");
				w.LoadFromFile(path);

				OStream worldStream;
				w.SaveToStream(worldStream);

				const ResourceID wid	   = w.GetID();
				const uint32	 worldSize = static_cast<uint32>(worldStream.GetCurrentSize());
				package1Stream << wid;
				package1Stream << worldSize;
				package1Stream.WriteRaw(worldStream.GetDataRaw(), worldStream.GetCurrentSize());
				worldStream.Destroy();
				LINA_TRACE("Writing resource to package: {0} {1}", w.GetID(), w.GetName());

				const Vector<ResourceDef>& needs = w.GetNeededResourceDefinitions();
				for (const ResourceDef& def : needs)
					allResourceNeeds.insert(def);
			}

			for (const ResourceDef& def : allResourceNeeds)
			{
				const String& path = pj->GetResourcePath(def.id);
				if (!FileSystem::FileOrPathExists(path))
					continue;
				IStream		 istream = Serialization::LoadFromFile(path.c_str());
				const uint32 sz		 = static_cast<uint32>(istream.GetSize());

				package1Stream << def.id;
				package1Stream << sz;
				package1Stream.WriteRaw(istream.GetDataRaw(), istream.GetSize());
				istream.Destroy();

				LINA_TRACE("Writing resource to package: {0} {1}", def.id, def.name);
			}

			const ResourceID eof = static_cast<ResourceID>(PACKAGES_EOF);
			package0Stream << eof;
			package1Stream << eof;

			const String file0 = gameProjectFolder + "LinaPackage0.linapkg";
			const String file1 = gameProjectFolder + "LinaPackage1.linapkg";
			Serialization::SaveToFile(file0.c_str(), package0Stream);
			Serialization::SaveToFile(file1.c_str(), package1Stream);
		};

		task->onComplete = [task, this, gameProjectFolder]() {
			NotificationDisplayer* notificationDisplayer = m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow());
			notificationDisplayer->AddNotification({
				.icon				= NotificationIcon::OK,
				.title				= Locale::GetStr(LocaleStr::SuccessfullyPackagedProject) + ": " + gameProjectFolder,
				.autoDestroySeconds = 5,
			});
		};

		m_editor->GetTaskManager().AddTask(task);
	}
} // namespace Lina::Editor
