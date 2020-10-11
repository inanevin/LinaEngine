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

Class: SelectComponentModal
Timestamp: 10/12/2020 12:45:38 AM

*/
#pragma once

#ifndef SelectComponentModal_HPP
#define SelectComponentModal_HPP

// Headers here.
#include "Drawers/EntityDrawer.hpp"

namespace LinaEditor
{
	class SelectComponentModal
	{
		
	public:
		
		SelectComponentModal();
		~SelectComponentModal();
	
		static void Draw(const std::vector<std::string>& types);

	private:
	
	};
}

#endif
