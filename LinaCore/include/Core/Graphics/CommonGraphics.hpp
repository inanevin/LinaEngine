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
#include "Common/Reflection/ClassReflection.hpp"
#include <LinaGX/Common/Defines/Format.hpp>

namespace Lina
{

#define LINA_MAIN_SWAPCHAIN UINT32_MAX - 1
#define DEFAULT_CLEAR_CLR	Color(0.3f, 0.3f, 0.5f, 1.0f)
#define IDEAL_RT			16667
#define FRAMES_IN_FLIGHT	2
#define BACK_BUFFER_COUNT	3
#define MAX_BOUND_TEXTURES	512
#define MAX_BOUND_SAMPLERS	128

#define DEFAULT_FOV 90.0f

	constexpr uint32 GetBytesPerPixelFromFormat(LinaGX::Format format)
	{
		if (format == LinaGX::Format::R8_UNORM)
			return 1;
		if (format == LinaGX::Format::R16_UNORM)
			return 2;

		if (format == LinaGX::Format::R8G8_UNORM)
			return 2;

		if (format == LinaGX::Format::R16G16_UNORM)
			return 4;

		if (format == LinaGX::Format::R8G8B8A8_UNORM || format == LinaGX::Format::R8G8B8A8_SRGB)
			return 4;

		if (format == LinaGX::Format::R16G16B16A16_UNORM)
			return 8;

		LINA_ASSERT(false, "");
		return 4;
	}

	class SemaphoreData
	{
	public:
		SemaphoreData() {};
		SemaphoreData(uint16 semaphore) : m_semaphore(semaphore) {};

		inline uint64* GetValuePtr()
		{
			return &m_value;
		}

		inline uint64 GetValue() const
		{
			return m_value;
		}

		inline uint16* GetSemaphorePtr()
		{
			return &m_semaphore;
		}

		inline uint16 GetSemaphore() const
		{
			return m_semaphore;
		}

		inline bool IsModified() const
		{
			return m_modified;
		}

		inline void Increment()
		{
			m_value++;
			m_modified = true;
		}

		inline void ResetModified()
		{
			m_modified = false;
		}

	private:
		uint16 m_semaphore = 0;
		uint64 m_value	   = 0;
		bool   m_modified  = false;
	};

	struct LinaTexture2D
	{
		ResourceID texture = 0;
		ResourceID sampler = 0;

		void SaveToStream(OStream& out) const
		{
			out << texture << sampler;
		}
		void LoadFromStream(IStream& in)
		{
			in >> texture >> sampler;
		}
	};

	struct LinaTexture2DBinding
	{
		uint32 textureIndex = 0;
		uint32 samplerIndex = 0;
	};

	enum class ShaderType
	{
		DeferredSurface,
		ForwardSurface,
		Sky,
		GUI,
		Custom,
	};

	enum class ShaderPropertyType
	{
		Bool,
		Float,
		Int32,
		UInt32,
		Vec2,
		Vec3,
		Vec4,
		IVec2,
		IVec3,
		IVec4,
		Matrix4,
		Texture2D,
	};

	enum class BlendMode
	{
		Opaque,
		AlphaBlend,
		TransparentBlend,
	};

	enum class DepthTesting
	{
		None,
		Always,
		Less,
		Equal,
	};

	struct ShaderPropertyDefinition
	{
		String			   name = "";
		StringID		   sid	= 0;
		ShaderPropertyType type = ShaderPropertyType::Bool;

		bool operator==(const ShaderPropertyDefinition& other) const
		{
			return sid == other.sid && type == other.type;
		}

		static bool VerifySimilarity(const Vector<ShaderPropertyDefinition>& v1, const Vector<ShaderPropertyDefinition>& v2);

		void SaveToStream(OStream& stream) const
		{
			stream << name << sid << type;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> name >> sid >> type;
		}
	};

	class Texture;

} // namespace Lina
