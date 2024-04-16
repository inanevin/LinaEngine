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
#include "Common/Data/String.hpp"
#include "Common/Data/Map.hpp"

namespace Lina::Editor
{

	enum class LocaleStr
	{
		Save,
		Load,
		Yes,
		No,
		Hex,
		File,
		Edit,
		View,
		Panels,
		About,
		Create,
		Open,
		Cancel,
		Name,
		Location,
		Select,
		ProjectSelect,
		CreateNewProject,
		OpenExistingProject,
		SelectDirectory,
		CreateANewProject,
		SelectProjectFile,
		DirectoryNotFound,
		FileNotFound,
		NameIsNotValid,
		Empty,
		ExitEditor,
		UnfinishedWorkTitle,
		UnfinishedWorkDesc,
		LinaProjectFile,
		NoProject,
		NewProject,
		LoadProject,
		SaveProject,
		NewWorld,
		LoadWorld,
		SaveWorld,
		SaveWorldAs,
		Exit,
		Github,
		Website,
		More,
		Entities,
		Resources,
		World,
		Performance,
		ResetLayout,
		Search,
		Items,
		Selected,
		NothingInDirectory,
        Memory,
        FrameTime,
        DrawCalls,
	};

	class Locale
	{
	public:
		static const String& GetStr(LocaleStr str);
		static String		 GetStrUnicode(LocaleStr str);

        static String GetRandomFact();
        
	private:
		static HashMap<LocaleStr, String> s_englishMap;
        static Vector<String> s_randomFacts;
	};

} // namespace Lina::Editor
