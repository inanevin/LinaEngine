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
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina
{
	/*
	class CompCamera : public Component
	{
	public:
		CompCamera() : Component(GetTypeID<CompCamera>()){};

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_fieldOfView << m_zNear << m_zFar;
		};

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_fieldOfView >> m_zNear >> m_zFar;
		}

		inline float GetNear() const
		{
			return m_zNear;
		}

		inline float GetFar() const
		{
			return m_zFar;
		}

		inline float GetFOV() const
		{
			return m_fieldOfView;
		}

	private:
		LINA_REFLECTION_ACCESS(CompCamera);

		float m_fieldOfView = DEFAULT_FOV;
		float m_zNear		= 0.01f;
		float m_zFar		= 500.0f;
	};

	LINA_COMPONENT_BEGIN(CompCamera, "Graphics");
	LINA_CLASS_END(CompCamera);
	*/
} // namespace Lina
