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

#include "Editor/Widgets/Panel/Panel.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	class Shader;
	class Text;
	class DirectionalLayout;
	class Button;
	class EntityWorld;
	class WorldRenderer;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class WorldDisplayer;

	class PanelShaderViewer : public Panel
	{
	public:
		PanelShaderViewer() : Panel(PanelType::ShaderViewer, 0){};
		virtual ~PanelShaderViewer() = default;

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Destruct() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void SaveLayoutToStream(OStream& stream) override;
		virtual void LoadLayoutFromStream(IStream& stream) override;

	private:
		void SetRuntimeDirty(bool isDirty);
		void RegenShader(const String& path);
		void SetupScene();

	private:
		LINA_REFLECTION_ACCESS(PanelShaderViewer);

		bool m_generalFold			  = true;
		bool m_shaderFold			  = true;
		bool m_containsRuntimeChanges = false;

		Editor*			m_editor		 = nullptr;
		Shader*			m_shader		 = nullptr;
		Widget*			m_inspector		 = nullptr;
		Widget*			m_saveButton	 = nullptr;
		EntityWorld*	m_world			 = nullptr;
		WorldRenderer*	m_worldRenderer	 = nullptr;
		Vector2ui		m_lastWorldSize	 = Vector2ui::Zero;
		WorldDisplayer* m_worldDisplayer = nullptr;
		String			m_shaderName	 = "";
	};

	LINA_WIDGET_BEGIN(PanelShaderViewer, Hidden)
	LINA_FIELD(PanelShaderViewer, m_shaderName, "Shader Name", FieldType::StringFixed, 0)
	LINA_CLASS_END(PanelShaderViewer)

} // namespace Lina::Editor