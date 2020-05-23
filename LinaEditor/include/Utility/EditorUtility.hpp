/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: EditorUtility
Timestamp: 5/9/2020 1:22:23 AM

*/
#pragma once

#ifndef EditorUtility_HPP
#define EditorUtility_HPP

// Headers here.

#include <string>
#include <vector>


namespace LinaEditor::Utility
{
	class EditorUtility
	{
		
	public:
		
		EditorUtility();
		~EditorUtility();
	
		static bool CreateFolderInPath(const std::string& path);
		static bool GetDirectories(std::vector<std::string>& vec, const std::string& path);
	private:
	
	};
}


#endif
