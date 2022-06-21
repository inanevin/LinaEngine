/*
Class: EditorSettings



Timestamp: 3/21/2022 7:02:03 PM
*/

#pragma once

#ifndef EditorSettings_HPP
#define EditorSettings_HPP

// Headers here.
#include <Data/String.hpp>
#include "Core/CommonReflection.hpp"

namespace Lina::Editor
{
    LINA_CLASS("Editor")
	class EditorSettings
	{
		
	public:
		
		EditorSettings() {};
		~EditorSettings() {};

		template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_textEditorPath);
        }
	
		LINA_PROPERTY("Text Editor", "StringPath", "Default text editor to open shader & similar files.")
		String m_textEditorPath = "";
	
	};
}

#endif
