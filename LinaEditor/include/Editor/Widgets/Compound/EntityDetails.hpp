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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Common/Data/String.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/Physics/CommonPhysics.hpp"
#include "Core/World/CommonWorld.hpp"

namespace Lina
{
	class DirectionalLayout;
	class Entity;
	class EntityWorld;
	class Text;
	class Component;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class EntityDetails : public Widget
	{
	public:
		struct DummyDetails
		{
			String	name  = "";
			Vector3 pos	  = Vector3::Zero;
			Vector3 rot	  = Vector3::Zero;
			Vector3 scale = Vector3::Zero;
		};

	public:
		EntityDetails()			 = default;
		virtual ~EntityDetails() = default;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void PreTick() override;
		void		 SetWorld(EntityWorld* w);
		void		 OnEntitySelectionChanged(const Vector<Entity*>& entities);
		void		 RefreshDetails();

	private:
		void UpdateDetails();
		void UpdateEntities();
		void StartEditingTransform();
		void StopEditingTransform();

		void StartEditingName();
		void StopEditingName();
		void StartEditingComponents();
		void StopEditingComponents();
		void StartEditingEntityPhysicsSettings();
		void StopEditingEntityPhyiscsSettings();
		void StartEditingEntityParams();
		void StopEditingEntityParams();

	private:
		LINA_REFLECTION_ACCESS(EntityDetails);

		Editor*						  m_editor = nullptr;
		DirectionalLayout*			  m_layout = nullptr;
		Vector<Entity*>				  m_selectedEntities;
		Vector<Transformation>		  m_storedTransformations;
		Vector<EntityPhysicsSettings> m_storedPhysicsSettings;
		Vector<String>				  m_storedNames;
		EntityWorld*				  m_world					= nullptr;
		DummyDetails				  m_dummyDetails			= {};
		bool						  m_physicsSettingsFold		= false;
		bool						  m_paramsFold				= false;
		Text*						  m_noDetailsText			= nullptr;
		bool						  m_isEditing				= false;
		Vector<OStream>				  m_editingComponentsBuffer = {};
		Vector<Component*>			  m_editingComponents		= {};
		Vector<EntityParameters>	  m_storedParams			= {};
	};

	LINA_WIDGET_BEGIN(EntityDetails, Hidden)
	LINA_CLASS_END(EntityDetails)

} // namespace Lina::Editor
