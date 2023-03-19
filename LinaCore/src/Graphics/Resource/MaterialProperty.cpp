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

#include "Graphics/Resource/MaterialProperty.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina
{
	MaterialPropertyBase* MaterialPropertyBase::CreateProperty(MaterialPropertyType type, const String& name)
	{
		MaterialPropertyBase* p = nullptr;
		switch (type)
		{
		case MaterialPropertyType::Float: {
			p = new MaterialProperty<float>();
			break;
		}
		case MaterialPropertyType::Int: {
			p = new MaterialProperty<int>();
			break;
		}
		case MaterialPropertyType::Bool: {
			p = new MaterialProperty<bool>();
			break;
		}
		case MaterialPropertyType::Vector2: {
			p = new MaterialProperty<Vector2>();
			break;
		}
		case MaterialPropertyType::Vector2i: {
			p = new MaterialProperty<Vector2i>();
			break;
		}
		case MaterialPropertyType::Vector4: {
			p = new MaterialProperty<Vector4>();
			break;
		}
		case MaterialPropertyType::Vector4i: {
			p = new MaterialProperty<Vector4i>();
			break;
		}
		case MaterialPropertyType::Mat4: {
			p = new MaterialProperty<Matrix4>();
			break;
		}
		case MaterialPropertyType::Texture: {
			p = new MaterialProperty<StringID>();
			break;
		}
		default:
			LINA_ASSERT(false, "Type not found!");
		}

		p->m_name = name;
		p->m_type = type;
		return p;
	}
} // namespace Lina
