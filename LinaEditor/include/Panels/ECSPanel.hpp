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

Class: ECSPanel
Timestamp: 5/23/2020 4:15:06 PM

*/
#pragma once

#ifndef ECSPanel_HPP
#define ECSPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/EditorUtility.hpp"

namespace LinaEngine
{
	namespace Graphics
	{
		class Window;
	}
}
namespace LinaEditor
{
	class ECSPanel : public EditorPanel
	{
		
	public:
		
		ECSPanel() {};
		~ECSPanel() {};
	
		virtual void Draw() override;
		void Setup(LinaEngine::ECS::ECSRegistry& registry, LinaEngine::Graphics::Window& appWindow);

	private:

		void CreateNewEntity();
		void AddComponentToEntity(int componentID);
		void DrawComponents(LinaEngine::ECS::ECSEntity& entity);

	private:

		LinaEngine::Graphics::Window* m_AppWindow;
		LinaEngine::ECS::ECSRegistry* m_ECS;
		std::vector<EditorEntity> m_EditorEntities;
		int m_SelectedEntity = -1;
	};
}

#endif
