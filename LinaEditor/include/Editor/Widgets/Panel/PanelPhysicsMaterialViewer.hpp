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

#include "Editor/Widgets/Panel/PanelResourceViewer.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	class EntityWorld;
	class Entity;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	class WorldDisplayer;

	class PanelPhysicsMaterialViewer : public PanelResourceViewer
	{
	public:
		enum class DisplayModel
		{
			Sphere,
			Cube,
		};

		PanelPhysicsMaterialViewer() : PanelResourceViewer(PanelType::PhysicsMaterialViewer, GetTypeID<PhysicsMaterial>(), GetTypeID<PanelPhysicsMaterialViewer>()){};
		virtual ~PanelPhysicsMaterialViewer() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Initialize() override;
		virtual void StoreEditorActionBuffer() override;
		virtual void UpdateResourceProperties() override;
		virtual void RebuildContents() override;
		void		 SetupWorld();

	private:
		void UpdateMaterial();

	private:
		LINA_REFLECTION_ACCESS(PanelPhysicsMaterialViewer);

		String		 m_materialName = "";
		DisplayModel m_displayModel = DisplayModel::Sphere;
		OStream		 m_previousStream;

		EntityWorld*	m_world			 = nullptr;
		WorldDisplayer* m_worldDisplayer = nullptr;
		Entity*			m_displayEntity	 = nullptr;
		Material*		m_floorMaterial	 = nullptr;
		Material*		m_objectMaterial = nullptr;
	};

	LINA_CLASS_BEGIN(PanelPhysicsMaterialViewerDisplayModel)
	LINA_PROPERTY_STRING(PanelPhysicsMaterialViewer::DisplayModel, 0, "Sphere")
	LINA_PROPERTY_STRING(PanelPhysicsMaterialViewer::DisplayModel, 1, "Cube")
	LINA_CLASS_END(PanelPhysicsMaterialViewerDisplayModel)

	LINA_WIDGET_BEGIN(PanelPhysicsMaterialViewer, Hidden)
	LINA_FIELD(PanelPhysicsMaterialViewer, m_materialName, "Name", FieldType::StringFixed, 0)
	LINA_FIELD(PanelPhysicsMaterialViewer, m_displayModel, "Display Model", FieldType::Enum, GetTypeID<PanelPhysicsMaterialViewer::DisplayModel>())
	LINA_CLASS_END(PanelPhysicsMaterialViewer)

} // namespace Lina::Editor
