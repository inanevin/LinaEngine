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

#include "Common/Data/Bitmask.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"

namespace Lina
{
	enum class PlayMode
	{
		None,
		Play,
		Physics,
		Other,
	};

	enum EntityFlags
	{
		EF_NONE		 = 0,
		EF_INVISIBLE = 1,
	};

	enum ComponentFlags
	{
		CF_NONE = 1 << 0,
	};

	enum WorldFlags
	{
		WORLD_FLAGS_NONE	= 1 << 0,
		WORLD_FLAGS_LOADING = 1 << 1,
	};

	struct WorldGfxSettings
	{
		ResourceID skyMaterial		= ENGINE_MATERIAL_DEFAULT_SKY_ID;
		ResourceID skyModel			= ENGINE_MODEL_SKYSPHERE_ID;
		Color	   ambientTop		= Color::White;
		Color	   ambientMid		= Color::White;
		Color	   ambientBot		= Color::White;
		float	   ambientIntensity = 1.0f;

		void SaveToStream(OStream& stream) const
		{
			stream << skyMaterial << skyModel << ambientTop << ambientMid << ambientBot << ambientIntensity;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> skyMaterial >> skyModel >> ambientTop >> ambientMid >> ambientBot >> ambientIntensity;
		}
	};

	enum class EntityParameterType
	{
		Float,
		Vector3,
		Vector2,
		Color,
		String,
		ResourceID,
		EntityID,
	};

	struct EntityParameter
	{
		EntityParameterType type	  = EntityParameterType::Float;
		String				name	  = "";
		float				valF	  = 0.0f;
		Vector3				valV3	  = Vector3::Zero;
		Vector2				valV2	  = Vector2::Zero;
		String				valStr	  = "";
		Color				valCol	  = Color::White;
		ResourceID			valRes	  = 0;
		EntityID			valEntity = 0;

		StringID _sid = 0;

		void SaveToStream(OStream& stream) const
		{
			stream << type << name << _sid << valF << valV3 << valV2 << valStr << valCol << valRes << valEntity;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> type >> name >> _sid >> valF >> valV3 >> valV2 >> valStr >> valCol >> valRes >> valEntity;
		}
	};

	struct EntityParameters
	{
		Vector<EntityParameter> params;

		void SaveToStream(OStream& stream) const
		{
			stream << params;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> params;
		}
	};

	LINA_CLASS_BEGIN(EntityParameterType);
	LINA_PROPERTY_STRING(EntityParameterType, 0, "Float")
	LINA_PROPERTY_STRING(EntityParameterType, 1, "Vector3")
	LINA_PROPERTY_STRING(EntityParameterType, 2, "Vector2")
	LINA_PROPERTY_STRING(EntityParameterType, 3, "Color")
	LINA_PROPERTY_STRING(EntityParameterType, 4, "String")
	LINA_PROPERTY_STRING(EntityParameterType, 5, "ResourceID")
	LINA_PROPERTY_STRING(EntityParameterType, 6, "EntityID")
	LINA_CLASS_END(EntityParameterType);

	LINA_CLASS_BEGIN(EntityParameter)
	LINA_FIELD(EntityParameter, type, "Type", FieldType::Enum, GetTypeID<EntityParameterType>())
	LINA_FIELD(EntityParameter, name, "Name", FieldType::String, 0)
	LINA_FIELD(EntityParameter, valF, "Value", FieldType::Float, 0)
	LINA_FIELD(EntityParameter, valV3, "Value", FieldType::Vector3, 0)
	LINA_FIELD(EntityParameter, valV2, "Value", FieldType::Vector2, 0)
	LINA_FIELD(EntityParameter, valCol, "Value", FieldType::Color, 0)
	LINA_FIELD(EntityParameter, valStr, "Value", FieldType::String, 0)
	LINA_FIELD(EntityParameter, valRes, "Value", FieldType::ResourceID, 0)
	LINA_FIELD(EntityParameter, valEntity, "Value", FieldType::EntityID, 0)
	LINA_FIELD_DEPENDENCY(EntityParameter, valF, "type", "0");
	LINA_FIELD_DEPENDENCY(EntityParameter, valV3, "type", "1");
	LINA_FIELD_DEPENDENCY(EntityParameter, valV2, "type", "2");
	LINA_FIELD_DEPENDENCY(EntityParameter, valCol, "type", "3");
	LINA_FIELD_DEPENDENCY(EntityParameter, valStr, "type", "4");
	LINA_FIELD_DEPENDENCY(EntityParameter, valRes, "type", "5");
	LINA_FIELD_DEPENDENCY(EntityParameter, valEntity, "type", "6");
	LINA_CLASS_END(EntityParameter)

	LINA_CLASS_BEGIN(EntityParameters)
	LINA_FIELD_VEC(EntityParameters, params, "Entity Parameters", FieldType::UserClass, EntityParameter, GetTypeID<EntityParameter>())
	LINA_CLASS_END(EntityParameters)

	LINA_CLASS_BEGIN(WorldGfxSettings)
	LINA_FIELD(WorldGfxSettings, skyMaterial, "Sky Material", FieldType::ResourceID, GetTypeID<Material>())
	LINA_FIELD(WorldGfxSettings, skyModel, "Sky Model", FieldType::ResourceID, GetTypeID<Model>())
	LINA_FIELD(WorldGfxSettings, ambientTop, "Ambient Top", FieldType::Color, 0)
	LINA_FIELD(WorldGfxSettings, ambientMid, "Ambient Mid", FieldType::Color, 0)
	LINA_FIELD(WorldGfxSettings, ambientBot, "Ambient Bottom", FieldType::Color, 0)
	LINA_FIELD(WorldGfxSettings, ambientIntensity, "Ambient Intensity", FieldType::Float, 0)
	LINA_FIELD_LIMITS(WorldGfxSettings, ambientIntensity, 0.0f, 5.0f, 0.1f)
	LINA_CLASS_END(WorldGfxSettings)

} // namespace Lina
