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

#include "Editor/EditorLocale.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{
	HashMap<LocaleStr, String> Locale::s_englishMap = {
		{LocaleStr::Save, "Save"},
		{LocaleStr::Load, "Load"},
		{LocaleStr::Yes, "Yes"},
		{LocaleStr::No, "No"},
		{LocaleStr::Hex, "Hex"},
		{LocaleStr::File, "File"},
		{LocaleStr::Edit, "Edit"},
		{LocaleStr::View, "View"},
		{LocaleStr::Panels, "Panels"},
		{LocaleStr::About, "About"},
		{LocaleStr::Create, "Create"},
		{LocaleStr::Delete, "Delete"},
		{LocaleStr::Duplicate, "Duplicate"},
		{LocaleStr::Open, "Open"},
		{LocaleStr::Cancel, "Cancel"},
		{LocaleStr::Name, "Name"},
		{LocaleStr::Location, "Location"},
		{LocaleStr::Select, "Select"},
		{LocaleStr::CreatingProject, "Creating project"},
		{LocaleStr::ProjectSelect, "Project Selection"},
		{LocaleStr::CreateNewProject, "Create new project"},
		{LocaleStr::OpenExistingProject, "Open existing project"},
		{LocaleStr::SelectDirectory, "Select directory"},
		{LocaleStr::CreateANewProject, "Create a new Lina project"},
		{LocaleStr::CreateANewShader, "Create a new Lina shader."},
		{LocaleStr::CreateANewWorld, "Create a new Lina World"},
		{LocaleStr::SelectProjectFile, "Select project to open"},
		{LocaleStr::DirectoryNotFound, "Directory not found!"},
		{LocaleStr::FileNotFound, "File not found!"},
		{LocaleStr::NameIsNotValid, "Not a valid name!"},
		{LocaleStr::Empty, "Empty"},
		{LocaleStr::ExitEditor, "Exit Editor"},
		{LocaleStr::UnfinishedWorkTitle, "Before you continue"},
		{LocaleStr::UnfinishedWorkDesc, "Your project contains unfinished work. Would you like to save your changes before you continue?"},
		{LocaleStr::LinaProjectFile, "Lina Project File"},
		{LocaleStr::LinaWorldFile, "Lina World File"},
		{LocaleStr::NoProject, "No Project"},
		{LocaleStr::NoProjectFound, "No project was found. Create or open one."},
		{LocaleStr::NoWorld, "No World"},
		{LocaleStr::NewProject, "New Project"},
		{LocaleStr::LoadProject, "Load Project"},
		{LocaleStr::SaveProject, "Save Project"},
		{LocaleStr::NewWorld, "New World"},
		{LocaleStr::LoadWorld, "Load World"},
		{LocaleStr::SaveWorld, "Save World"},
		{LocaleStr::SaveWorldAs, "Save World As"},
		{LocaleStr::Exit, "Exit"},
		{LocaleStr::Github, "Github"},
		{LocaleStr::Website, "Website"},
		{LocaleStr::More, "More"},
		{LocaleStr::Entities, "Entities"},
		{LocaleStr::Resources, "Resources"},
		{LocaleStr::World, "World"},
		{LocaleStr::Performance, "Performance"},
		{LocaleStr::ResetLayout, "Reset Layout"},
		{LocaleStr::Search, "Search"},
		{LocaleStr::Items, "Items"},
		{LocaleStr::Selected, "Selected"},
		{LocaleStr::NothingInDirectory, "Nothing to see here ¯\\(°_o)/¯"},
		{LocaleStr::Memory, "Memory"},
		{LocaleStr::FrameTime, "Frame Time"},
		{LocaleStr::DrawCalls, "Draw Calls"},
		{LocaleStr::GeneratingThumbnails, "Generating thumbnails..."},
		{LocaleStr::ContentSorting, "Content Sorting"},
		{LocaleStr::Alphabetical, "Alphabetical (A-Z)"},
		{LocaleStr::TypeBased, "Sort By Type"},
		{LocaleStr::Favourites, "Favourites"},
		{LocaleStr::Folder, "Folder"},
		{LocaleStr::Material, "Material"},
		{LocaleStr::PhysicsMaterial, "Physics Material"},
		{LocaleStr::Shader, "Shader"},
		{LocaleStr::Sampler, "Texture Sampler"},
		{LocaleStr::OpaqueSurfaceShader, "Opaque Surface Shader"},
		{LocaleStr::SkyShader, "Sky Shader"},
		{LocaleStr::TransparentSurfaceShader, "Transparent Surface Shader"},
		{LocaleStr::PostProcessShader, "Post Process Shader"},
		{LocaleStr::AddToFavourites, "Add to Favourites"},
		{LocaleStr::RemoveFromFavourites, "Remove from Favourites"},
		{LocaleStr::RemoveAll, "Remove All Items"},
		{LocaleStr::AreYouSureYouWantToDeleteI, "Are you sure you want to delete"},
		{LocaleStr::AreYouSureYouWantToDeleteII, "item(s)?"},
		{LocaleStr::Minimize, "Minimize"},
		{LocaleStr::Maximize, "Maximize"},
		{LocaleStr::MaximizeRestore, "Maximize/Restore"},
		{LocaleStr::Close, "Close"},
		{LocaleStr::Rename, "Rename"},
		{LocaleStr::WidgetEditor, "Widget Editor"},
		{LocaleStr::Import, "Import"},
		{LocaleStr::ReImport, "Reimport"},
		{LocaleStr::ReimportChangedFiles, "Reimport Modified Sources"},
		{LocaleStr::Revert, "Revert"},
		{LocaleStr::Reverting, "Reverting..."},
		{LocaleStr::Reimporting, "Reimporting..."},
		{LocaleStr::New, "New"},
		{LocaleStr::GUIWidget, "GUIWidget"},
		{LocaleStr::SelectWidget, "Select: GUIWidget"},
		{LocaleStr::SaveChanges, "Save changes?"},
		{LocaleStr::SaveChangesDesc, "Would you like to save changes to the current file?"},
		{LocaleStr::SelectColor, "Select: Color"},
		{LocaleStr::StartColor, "Start Color"},
		{LocaleStr::EndColor, "End Color"},
		{LocaleStr::ThemeColor, "Theme Color"},
		{LocaleStr::Both, "Both"},
		{LocaleStr::Export, "Export"},
		{LocaleStr::Refresh, "Refresh"},
		{LocaleStr::ImportingResources, "Importing resources"},
		{LocaleStr::ThisResourceNoLongerExists, "This resource no longer exists :("},
		{LocaleStr::GenerateSDF, "Generate SDF"},
		{LocaleStr::General, "General"},
		{LocaleStr::Resource, "Resource"},
		{LocaleStr::Working, "Working"},
		{LocaleStr::WorkInProgressInAnotherWindow, "Work in progress in another window."},
		{LocaleStr::LoadingProjectData, "Loading project data..."},
		{LocaleStr::VerifyingProjectResources, "Verifying project resources..."},
		{LocaleStr::CreatingCoreResources, "Creating core resources..."},
		{LocaleStr::Saving, "Saving..."},
		{LocaleStr::LoadingCoreResources, "Loading core resources..."},
		{LocaleStr::LoadingWorld, "Loading world..."},
		{LocaleStr::LoadingEngine, "Loading engine..."},
		{LocaleStr::LoadingSettings, "Loading settings..."},
		{LocaleStr::GeneratingAtlases, "Generating atlases..."},
		{LocaleStr::ApplyingChanges, "Applying changes..."},
		{LocaleStr::FailedReimportingResource, "Failed reimporting resource!"},
		{LocaleStr::ReimportedResource, "Reimported resource: "},
	};

	Vector<String> Locale::s_randomFacts = {
		"Nothing to see here ¯\\(°_o)/¯",
		"Pro tip: No, doing that system will take twice longer than you anticipated.",
		"Don't search for LinaGX.",
		"Don't search for LinaVG.",
		"It took me more than 5 years to publish a proper Lina version, do you -really- need that extra feature?",
		"If you notice a bug, please report it to the nearest Github issues kiosk.",
		"I spent my time doing this fun little thing instead of writing useful code.",
		"Pro tip: You are not your target audience.",
	};
	const String& Locale::GetStr(LocaleStr str)
	{
		return s_englishMap[str];
	}

	String Locale::GetStrUnicode(LocaleStr str)
	{
		std::u8string utf8str = u8"";
		std::string	  convertedStr(reinterpret_cast<const char*>(utf8str.c_str()));
		convertedStr += s_englishMap[str];
		return convertedStr;
	}

	String Locale::GetRandomFact()
	{
		const int32 rnd = Math::Rand(0, static_cast<int32>(s_randomFacts.size() - 1));
		return s_randomFacts[rnd];
	}
} // namespace Lina::Editor
