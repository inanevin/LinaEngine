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

Class: ResourcesPanel
Timestamp: 6/5/2020 12:54:52 AM

*/
#pragma once

#ifndef ResourcesPanel_HPP
#define ResourcesPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"

namespace LinaEditor 
{
	class ResourcesPanel : public EditorPanel
	{
		
	public:
		
		ResourcesPanel(Vector2 position, Vector2 size) : EditorPanel(position, size) {};
		virtual ~ResourcesPanel() {};
	
		virtual void Draw() override;
		void Setup();
	private:
	
	};
}

#endif
