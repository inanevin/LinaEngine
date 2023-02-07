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

#ifndef CameraComponent_HPP
#define CameraComponent_HPP

// Headers here.
#include "World/Core/Component.hpp"
#include "Reflection/ClassReflection.hpp"
#include "Data/Streams.hpp"

namespace Lina
{
	class CameraSystem;

	class CameraComponent : public Component
	{
	public:
		float fieldOfView = 90.0f;
		float zNear		  = 0.01f;
		float zFar		  = 1000.0f;

		virtual void SaveToStream(OStream& stream)
		{
			stream << fieldOfView << zNear << zFar;
		};

		virtual void LoadFromStream(IStream& stream)
		{
			stream >> fieldOfView >> zNear >> zFar;
		}

		inline const Matrix& GetProjection()
		{
			return m_projection;
		}

		inline const Matrix& GetView()
		{
			return m_view;
		}

	private:
		friend class CameraSystem;

		Matrix m_projection = Matrix::Identity();
		Matrix m_view		= Matrix::Identity();
	};
} // namespace Lina

#endif
