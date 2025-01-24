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

#include "Core/Graphics/CommonGraphics.hpp"
#include "Editor/Widgets/Panel/PanelResourceViewer.hpp"

namespace Lina
{
	class Model;
	class Text;
	class DirectionalLayout;
	class Button;
	class EntityWorld;
	class WorldRenderer;
	class Entity;
	class CompModel;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class WorldDisplayer;
	class EditorWorldRenderer;

	class PanelModelViewer : public PanelResourceViewer
	{
	public:
		PanelModelViewer() : PanelResourceViewer(PanelType::ModelViewer, GetTypeID<Model>(), GetTypeID<PanelModelViewer>()){};
		virtual ~PanelModelViewer() = default;

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Destruct() override;
		virtual void StoreEditorActionBuffer() override;
		virtual void UpdateResourceProperties() override;
		virtual void RebuildContents() override;
		void		 SetupWorld();

		inline EntityWorld* GetWorld() const
		{
			return m_world;
		}

	private:
		LINA_REFLECTION_ACCESS(PanelModelViewer);

		String			m_modelName				= "";
		String			m_materialDefs			= "";
		String			m_animations			= "";
		String			m_meshes				= "";
		float			m_animationPreviewSpeed = 1.0f;
		WorldDisplayer* m_worldDisplayer		= nullptr;
		EntityWorld*	m_world					= nullptr;
		Entity*			m_displayEntity			= nullptr;
		int32			m_displayAnimation		= -1;
		CompModel*		m_compModel				= nullptr;
		OStream			m_previousStream;
		Material*		m_skyMaterial = nullptr;
	};

	LINA_WIDGET_BEGIN(PanelModelViewer, Hidden)
	LINA_FIELD(PanelModelViewer, m_modelName, "Model Name", FieldType::StringFixed, 0)
	LINA_FIELD(PanelModelViewer, m_animations, "Animation Count", FieldType::StringFixed, 0)
	LINA_FIELD(PanelModelViewer, m_materialDefs, "Material Def Count", FieldType::StringFixed, 0)
	LINA_FIELD(PanelModelViewer, m_meshes, "Mesh Count", FieldType::StringFixed, 0)
	LINA_FIELD(PanelModelViewer, m_animationPreviewSpeed, "Animation PreviewSpeed", FieldType::Float, 0)
	LINA_FIELD_LIMITS(PanelModelViewer, m_animationPreviewSpeed, 0.0f, 10.0f, 0.1f)
	LINA_CLASS_END(PanelModelViewer)

} // namespace Lina::Editor
