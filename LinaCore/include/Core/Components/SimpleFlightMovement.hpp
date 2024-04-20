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
	class SimpleFlightMovement : public Component
	{
	public:
		virtual void Begin() override;
		virtual void End() override;
		virtual void Tick(float delta) override;

		virtual TypeID GetComponentType() override
		{
			return GetTypeID<SimpleFlightMovement>();
		}

		virtual void SaveToStream(OStream& stream) const override
		{
			stream << m_movementSpeed << m_rotationSpeed;
		};

		virtual void LoadFromStream(IStream& stream) override
		{
			stream >> m_movementSpeed >> m_rotationSpeed;
		}

	private:
		LINA_REFLECTION_ACCESS(SimpleFlightMovement);
		float m_movementSpeed = 26.0f;
		float m_rotationSpeed = 12.0f;
	};

	LINA_REFLECTCOMPONENT_BEGIN(SimpleFlightMovement, "SimpleFlightMovement", "Transformation")
	LINA_REFLECTCOMPONENT_END(SimpleFlightMovement);
} // namespace Lina
