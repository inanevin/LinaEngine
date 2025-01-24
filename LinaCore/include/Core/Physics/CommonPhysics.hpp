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

#include "Common/SizeDefinitions.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/Reflection/CommonReflection.hpp"
#include "Common/Math/Vector.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Math/MathTypes.h>

namespace JPH
{
	struct Vec3;
	struct Quat;
	class Color;
	enum class EMotionType : uint8;
} // namespace JPH

namespace Lina
{
	class Entity;

	enum class PhysicsBroadPhaseLayers : uint16
	{
		NonMoving = 0,
		Moving,
	};

	enum class PhysicsObjectLayers : uint16
	{
		NonMoving = 0,
		Moving,
	};

	static constexpr uint16 PHYSICS_NUM_BP_LAYERS(2);
	static constexpr uint16 PHYSICS_NUM_OBJ_LAYERS(2);

	enum class PhysicsBodyType
	{
		None,
		Static,
		Kinematic,
		Dynamic
	};

	enum class PhysicsShapeType
	{
		Sphere,
		Box,
		Capsule,
		Cylinder,
		Plane,
	};

	struct EntityPhysicsSettings
	{
		PhysicsBodyType	 bodyType		   = PhysicsBodyType::None;
		PhysicsShapeType shapeType		   = PhysicsShapeType::Box;
		Vector3			 offset			   = Vector3::Zero;
		Vector3			 shapeExtents	   = Vector3(0.5f, 0.5f, 0.5f);
		float			 radius			   = 0.5f;
		float			 height			   = 1.0f;
		float			 mass			   = 1.0f;
		float			 gravityMultiplier = 1.0f;
		ResourceID		 material		   = ENGINE_PHY_MATERIAL_DEFAULT_ID;

		void SaveToStream(OStream& stream) const
		{
			stream << bodyType << shapeType << shapeExtents << radius << height << material << gravityMultiplier << mass << offset;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> bodyType >> shapeType >> shapeExtents >> radius >> height >> material >> gravityMultiplier >> mass >> offset;
		}
	};

	struct WorldPhysicsSettings
	{
		Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
	};

	struct RayResult
	{
		Vector<Entity*> hitEntities;
		Vector<Vector3> hitPoints;
		Vector<float>	hitDistances;
	};

	LINA_CLASS_BEGIN(WorldPhysicsSettings);
	LINA_FIELD(WorldPhysicsSettings, gravity, "Gravity", FieldType::Vector3, 0)
	LINA_CLASS_END(WorldPhysicsSettings);

	extern JPH::Vec3		ToJoltVec3(const Vector3& v);
	extern JPH::Quat		ToJoltQuat(const Quaternion& q);
	extern JPH::EMotionType ToJoltMotionType(const PhysicsBodyType& type);
	extern JPH::Color		ToJoltColor(const Color& c);
	extern Vector3			FromJoltVec3(const JPH::Vec3& v);
	extern Quaternion		FromJoltQuat(const JPH::Quat& q);
	extern Color			FromJoltColor(const JPH::Color& c);

	LINA_CLASS_BEGIN(PhysicsBodyType)
	LINA_PROPERTY_STRING(PhysicsBodyType, 0, "None")
	LINA_PROPERTY_STRING(PhysicsBodyType, 1, "Static")
	LINA_PROPERTY_STRING(PhysicsBodyType, 2, "Kinematic")
	LINA_PROPERTY_STRING(PhysicsBodyType, 3, "Dynamic")
	LINA_CLASS_END(PhysicsBodyType)

	LINA_CLASS_BEGIN(PhysicsShapeType)
	LINA_PROPERTY_STRING(PhysicsShapeType, 0, "Sphere")
	LINA_PROPERTY_STRING(PhysicsShapeType, 1, "Box")
	LINA_PROPERTY_STRING(PhysicsShapeType, 2, "Capsule")
	LINA_PROPERTY_STRING(PhysicsShapeType, 3, "Cylinder")
	LINA_PROPERTY_STRING(PhysicsShapeType, 4, "Infinite Plane")
	LINA_CLASS_END(PhysicsShapeType)

	LINA_CLASS_BEGIN(EntityPhysicsSettings)
	LINA_FIELD(EntityPhysicsSettings, bodyType, "Body Type", FieldType::Enum, GetTypeID<PhysicsBodyType>())
	LINA_FIELD(EntityPhysicsSettings, shapeType, "Shape Type", FieldType::Enum, GetTypeID<PhysicsShapeType>())
	LINA_FIELD(EntityPhysicsSettings, shapeExtents, "Shape Extents", FieldType::Vector3, 0)
	LINA_FIELD(EntityPhysicsSettings, radius, "Radius", FieldType::Float, 0)
	LINA_FIELD(EntityPhysicsSettings, height, "Height", FieldType::Float, 0)
	LINA_FIELD(EntityPhysicsSettings, offset, "Offset", FieldType::Vector3, 0)
	LINA_FIELD(EntityPhysicsSettings, mass, "Mass", FieldType::Float, 0)
	LINA_FIELD(EntityPhysicsSettings, gravityMultiplier, "Gravity Multiplier", FieldType::Float, 0)
	LINA_FIELD(EntityPhysicsSettings, material, "Material", FieldType::ResourceID, GetTypeID<PhysicsMaterial>())
	LINA_FIELD_DEPENDENCY(EntityPhysicsSettings, shapeExtents, "shapeType", "1")
	LINA_FIELD_DEPENDENCY(EntityPhysicsSettings, radius, "shapeType", "0, 2, 3")
	LINA_FIELD_DEPENDENCY(EntityPhysicsSettings, height, "shapeType", "0, 2, 3")
	LINA_CLASS_END(EntityPhysicsSettings)

} // namespace Lina
