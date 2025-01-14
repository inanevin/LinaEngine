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
            stream << m_type << m_color << m_intensity << m_distance << m_cutOff;
        }
        
        virtual void LoadFromStream(IStream& stream)
        {
            stream>> m_type >> m_color >> m_intensity >> m_distance >> m_cutOff;
        }
        
    private:
        LINA_REFLECTION_ACCESS(CompLight);
        
        LightType m_type = LightType::Directional;
        Color m_color = Color::White;
        float m_intensity = 1.0f;
        float m_distance = 1.0f;
        float m_cutOff = 0.0f;
    };
    
LINA_CLASS_BEGIN(LightType)
LINA_PROPERTY_STRING(LightType, 0, "Directional")
LINA_PROPERTY_STRING(LightType, 1, "Point")
LINA_PROPERTY_STRING(LightType, 2, "Spot")
LINA_CLASS_END(LightType)

LINA_COMPONENT_BEGIN(CompLight, "Graphics")
LINA_FIELD(CompLight, m_type, "Type", FieldType::Enum, GetTypeID<LightType>())
LINA_FIELD(CompLight, m_distance, "Distance", FieldType::Float, 0)
LINA_FIELD(CompLight, m_cutOff, "CutOff", FieldType::Float, 0)
LINA_FIELD(CompLight, m_intensity, "Intensity", FieldType::Float, 0)
LINA_FIELD(CompLight, m_color, "Color", FieldType::Color, 0)
LINA_FIELD_DEPENDENCY_POS_OP(CompLight, m_distance, "m_type", 0, "neq"_hs)
LINA_FIELD_DEPENDENCY_POS(CompLight, m_cutOff, "m_type", 2)
LINA_CLASS_END(CompLight)


} // namespace Lina
