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
		{LocaleStr::Open, "Open"},
		{LocaleStr::Cancel, "Cancel"},
		{LocaleStr::Name, "Name"},
		{LocaleStr::Location, "Location"},
		{LocaleStr::Select, "Select"},
		{LocaleStr::ProjectSelect, "Project Selection"},
		{LocaleStr::CreateNewProject, "Create new project"},
		{LocaleStr::OpenExistingProject, "Open existing project"},
		{LocaleStr::SelectDirectory, "Select directory"},
		{LocaleStr::CreateANewProject, "Create a new Lina project"},
		{LocaleStr::SelectProjectFile, "Select project to open"},
		{LocaleStr::DirectoryNotFound, "Directory not found!"},
		{LocaleStr::FileNotFound, "File not found!"},
		{LocaleStr::NameIsNotValid, "Not a valid name!"},
		{LocaleStr::Empty, "Empty"},
		{LocaleStr::ExitEditor, "Exit Editor"},
		{LocaleStr::UnfinishedWorkTitle, "Before you continue"},
		{LocaleStr::UnfinishedWorkDesc, "Your project contains unfinished work. Would you like to save your changes before you continue?"},
		{LocaleStr::LinaProjectFile, "Lina Project File"},
		{LocaleStr::NoProject, "No Project"},
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
	};

	const String& Locale::GetStr(LocaleStr str)
	{
		return s_englishMap[str];
	}
} // namespace Lina::Editor
