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

#include "Editor/Meta/EditorSettings.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Common/Data/Streams.hpp"

#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"

namespace Lina::Editor
{
	void EditorSettings::LoadFromStream(IStream& in)
	{
		uint32 version = 0;
		in >> version;
		in >> m_lastProjectPath;
		in >> m_lastWorldID;
		in >> m_layout;
		in >> m_inputSettings;
		in >> m_paramCollection;
	}

	void EditorSettings::SaveToStream(OStream& out)
	{
		out << VERSION;
		out << m_lastProjectPath;
		out << m_lastWorldID;
		out << m_layout;
		out << m_inputSettings;
		out << m_paramCollection;
	}
} // namespace Lina::Editor
