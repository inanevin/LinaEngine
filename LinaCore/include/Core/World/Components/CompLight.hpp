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

#include "Core/World/Component.hpp"

namespace Lina
{
	enum class LightType
	{
		Directional = 0,
		Point,
		Spot,
	};

	class CompLight : public Component
	{
	public:
		CompLight() : Component(GetTypeID<CompLight>(), 0){};

		inline void SetType(LightType type)
		{
			m_type = type;
		}

		virtual void SaveToStream(OStream& stream) const
		{
			stream << m_type << m_color << m_intensity << m_radius << m_cutoff << m_outerCutoff << m_falloff;
		}

		virtual void LoadFromStream(IStream& stream)
		{
			stream >> m_type >> m_color >> m_intensity >> m_radius >> m_cutoff >> m_outerCutoff >> m_falloff;
		}

		inline float GetRadius() const
		{
			return m_radius;
		}

		inline float GetCutoff() const
		{
			return m_cutoff;
		}

		inline float GetOuterCutoff() const
		{
			return m_outerCutoff;
		}

		inline float GetIntensity() const
		{
			return m_intensity;
		}

		inline const Color& GetColor() const
		{
			return m_color;
		}

		inline LightType GetType() const
		{
			return m_type;
		}

		inline float GetFalloff() const
		{
			return m_falloff;
		}

	private:
		LINA_REFLECTION_ACCESS(CompLight);

		LightType m_type		= LightType::Directional;
		Color	  m_color		= Color::White;
		float	  m_intensity	= 1.0f;
		float	  m_radius		= 2.0f;
		float	  m_falloff		= 1.0f;
		float	  m_cutoff		= 5.0f;
		float	  m_outerCutoff = 7.0f;
	};

	LINA_CLASS_BEGIN(LightType)
	LINA_PROPERTY_STRING(LightType, 0, "Directional")
	LINA_PROPERTY_STRING(LightType, 1, "Point")
	LINA_PROPERTY_STRING(LightType, 2, "Spot")
	LINA_CLASS_END(LightType)

	LINA_COMPONENT_BEGIN(CompLight, "Graphics")
	LINA_FIELD(CompLight, m_type, "Type", FieldType::Enum, GetTypeID<LightType>())
	LINA_FIELD(CompLight, m_radius, "Radius", FieldType::Float, 0)
	LINA_FIELD(CompLight, m_falloff, "Falloff", FieldType::Float, 0)
	LINA_FIELD(CompLight, m_cutoff, "Cutoff", FieldType::Float, 0)
	LINA_FIELD(CompLight, m_outerCutoff, "Outer Cutoff", FieldType::Float, 0)
	LINA_FIELD(CompLight, m_intensity, "Intensity", FieldType::Float, 0)
	LINA_FIELD(CompLight, m_color, "Color", FieldType::Color, 0)
	LINA_FIELD_DEPENDENCY_OP(CompLight, m_radius, "m_type", "0", "neq")
	LINA_FIELD_DEPENDENCY_OP(CompLight, m_falloff, "m_type", "0", "neq")
	LINA_FIELD_DEPENDENCY(CompLight, m_cutoff, "m_type", "2")
	LINA_FIELD_DEPENDENCY(CompLight, m_outerCutoff, "m_type", "2")
	LINA_FIELD_LIMITS(CompLight, m_cutoff, 0.0f, 89.0f, 1.0f)
	LINA_FIELD_LIMITS(CompLight, m_outerCutoff, 0.0f, 89.0f, 1.0f)
	LINA_CLASS_END(CompLight)

} // namespace Lina
