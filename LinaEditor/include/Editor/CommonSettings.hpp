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

#include "Common/Reflection/ClassReflection.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{
	struct EditorInputSettings
	{
		float cameraMovePower = 0.2f;
		float cameraMoveSpeed = 5.0f;

		float cameraAngularPower = 10.0f;
		float cameraAngularSpeed = 30.0f;
		float mouseWheelBoost	 = 1.0f;

		void SaveToStream(OStream& stream) const
		{
			stream << cameraMovePower << cameraMoveSpeed << cameraAngularPower << cameraAngularSpeed;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> cameraMovePower >> cameraMoveSpeed >> cameraAngularPower >> cameraAngularSpeed;
		}
	};

	LINA_CLASS_BEGIN(EditorInputSettings);
	LINA_FIELD(EditorInputSettings, cameraMovePower, "Camera Move Power", FieldType::Float, 0)
	LINA_FIELD(EditorInputSettings, cameraMoveSpeed, "Camera Move Speed", FieldType::Float, 0)
	LINA_FIELD(EditorInputSettings, cameraAngularPower, "Camera Angular Power", FieldType::Float, 0)
	LINA_FIELD(EditorInputSettings, cameraAngularSpeed, "Camera Angular Speed", FieldType::Float, 0)
	LINA_FIELD(EditorInputSettings, mouseWheelBoost, "Mouse Wheel Boost", FieldType::Float, 0)
	LINA_FIELD_LIMITS(EditorInputSettings, mouseWheelBoost, 0.0f, 5.0f, 0.1f);
	LINA_CLASS_END(EditorInputSettings);
} // namespace Lina::Editor
