/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: GUILayer

This class is pushed as an overlay layer to the render engine and is responsible for drawing the editor.
It inits panels, drawers etc. and is the main bridge of communication between editor components.

*/

#ifndef GUILAYER_HPP
#define GUILAYER_HPP


#include "Core/Layer.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Panels/ECSPanel.hpp"
#include "Panels/ResourcesPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/LogPanel.hpp"
#include "Panels/HeaderPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/GlobalSettingsPanel.hpp"
#include "World/DefaultLevel.hpp"
#include <vector>

namespace Lina
{
	namespace World
	{
		class Level;
	}
}

namespace Lina::Editor
{
	

	class GUILayer : public Lina::Layer
	{

	public:

		GUILayer() : Lina::Layer("GUILayer") {}
		~GUILayer();

		// Overrides
		void Attach() override;
		void Detach() override;
		void Render() override;

		// Menu bar item callback from header panel.
		void DispatchMenuBarClickedAction(EMenuBarItemClicked event);

		void Refresh();
		ScenePanel& GetScenePanel() { return m_scenePanel; }

	private:

		void DrawFPSCounter(int corner = 0);
		void DrawCentralDockingSpace();
		void CreateObjectInLevel(const std::string& modelPath);

	private:

		Lina::Graphics::DrawParams m_drawParameters;
		ECSPanel m_ecsPanel;
		ResourcesPanel m_resourcesPanel;
		ScenePanel m_scenePanel;
		PropertiesPanel m_propertiesPanel;
		LogPanel m_logPanel;
		HeaderPanel m_headerPanel;
		ProfilerPanel m_profilerPanel;
		GlobalSettingsPanel m_globalSettingsPanel;

		Lina::World::DefaultLevel m_defaultLevel;
	};
}

#endif