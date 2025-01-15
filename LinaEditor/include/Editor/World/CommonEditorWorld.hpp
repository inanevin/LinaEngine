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

#include "Core/Resources/CommonResources.hpp"
#include "Common/Reflection/ClassReflection.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"

namespace Lina::Editor
{

#define EDITOR_ENTITY_GUID_START UINT64_MAX - 100
#define GIZMO_GUID_X_AXIS		 EDITOR_ENTITY_GUID_START + 1
#define GIZMO_GUID_Y_AXIS		 EDITOR_ENTITY_GUID_START + 2
#define GIZMO_GUID_Z_AXIS		 EDITOR_ENTITY_GUID_START + 3
#define GIZMO_GUID_CENTER_AXIS	 EDITOR_ENTITY_GUID_START + 4
#define GIZMO_ORIENTATION_X		 EDITOR_ENTITY_GUID_START + 5
#define GIZMO_ORIENTATION_Y		 EDITOR_ENTITY_GUID_START + 6
#define GIZMO_ORIENTATION_Z		 EDITOR_ENTITY_GUID_START + 7

	enum class GizmoMode
	{
		Move,
		Rotate,
		Scale,
	};

	enum class GizmoLocality
	{
		World,
		Local,
	};

	enum class GizmoSnapping
	{
		Free,
		Step,
	};

	enum class WorldCameraType
	{
		Orbit,
		FreeMove,
	};

	enum class GizmoAxis
	{
		None,
		X,
		Y,
		Z,
		Center,
	};

	enum class GizmoMotion
	{
		None,
		Mouse,
		Key
	};

	struct SnappingOptions
	{
		Vector3 directionalSnap = Vector3::Zero;
		float	angularSnap		= 0.0f;
	};

	LINA_CLASS_BEGIN(SnappingOptions);
	LINA_FIELD(SnappingOptions, directionalSnap, "Directional Snap", FieldType::Vector3, 0)
	LINA_FIELD(SnappingOptions, angularSnap, "Angular Snap", FieldType::Float, 0)
	LINA_CLASS_END(SnappingOptions);

	struct CameraOptions
	{
		float movementBoost = 1.0f;
		float angularBoost	= 1.0f;
	};

	LINA_CLASS_BEGIN(CameraOptions);
	LINA_FIELD(CameraOptions, movementBoost, "Movement Boost", FieldType::Float, 0)
	LINA_FIELD(CameraOptions, angularBoost, "Angular Boost", FieldType::Float, 0)
	LINA_FIELD_LIMITS(CameraOptions, movementBoost, 0.1f, 3.0f, 0.1f)
	LINA_FIELD_LIMITS(CameraOptions, angularBoost, 0.1f, 3.0f, 0.1f)
	LINA_CLASS_END(CameraOptions);

} // namespace Lina::Editor
