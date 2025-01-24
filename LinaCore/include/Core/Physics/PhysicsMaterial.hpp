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

#include "Core/Resources/Resource.hpp"

namespace Lina
{
	class EntityWorld;

	class PhysicsMaterial : public Resource
	{
	public:
		static constexpr uint32 VERSION = 0;

		PhysicsMaterial(ResourceID id, const String& name) : Resource(id, GetTypeID<PhysicsMaterial>(), name){};
		~PhysicsMaterial() = default;

		virtual bool LoadFromFile(const String& path) override;
		virtual void SaveToStream(OStream& stream) const override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void GenerateHW() override;
		virtual void DestroyHW() override;

		inline float GetRestitution() const
		{
			return m_restitution;
		}

		inline float GetFriction() const
		{
			return m_friction;
		}

		inline float GetAngularDamping() const
		{
			return m_angularDamping;
		}

		inline float GetLinearDamping() const
		{
			return m_linearDamping;
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		LINA_REFLECTION_ACCESS(PhysicsMaterial)

		float m_restitution	   = 0.0f;
		float m_friction	   = 0.2f;
		float m_angularDamping = 0.05f;
		float m_linearDamping  = 0.05f;
	};

	LINA_RESOURCE_BEGIN(PhysicsMaterial)
	LINA_FIELD(PhysicsMaterial, m_restitution, "Restitution", FieldType::Float, 0)
	LINA_FIELD(PhysicsMaterial, m_friction, "Friction", FieldType::Float, 0)
	LINA_FIELD(PhysicsMaterial, m_linearDamping, "Linear Damping", FieldType::Float, 0)
	LINA_FIELD(PhysicsMaterial, m_angularDamping, "Angular Damping", FieldType::Float, 0)
	LINA_FIELD_LIMITS(PhysicsMaterial, m_restitution, 0.0f, 1.0f, 0.1f)
	LINA_FIELD_LIMITS(PhysicsMaterial, m_friction, 0.0f, 1.0f, 0.1f)
	LINA_FIELD_LIMITS(PhysicsMaterial, m_linearDamping, 0.0f, 1.0f, 0.1f)
	LINA_FIELD_LIMITS(PhysicsMaterial, m_angularDamping, 0.0f, 1.0f, 0.1f)
	LINA_CLASS_END(PhysicsMaterial)

} // namespace Lina
