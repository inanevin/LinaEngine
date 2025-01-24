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

#include "Core/Resources/Data/ResourceMetadata.hpp"

namespace Lina
{
	Vector2 ResourceMetadata::GetVector2(StringID sid, const Vector2& defaultValue)
	{
		if (m_vec2s.find(sid) == m_vec2s.end())
			m_vec2s[sid] = defaultValue;

		return m_vec2s[sid];
	}

	Vector3 ResourceMetadata::GetVector3(StringID sid, const Vector3& defaultValue)
	{
		if (m_vec3s.find(sid) == m_vec3s.end())
			m_vec3s[sid] = defaultValue;

		return m_vec3s[sid];
	}

	Vector4 ResourceMetadata::GetVector4(StringID sid, const Vector4& defaultValue)
	{
		if (m_vec4s.find(sid) == m_vec4s.end())
			m_vec4s[sid] = defaultValue;

		return m_vec4s[sid];
	}

	String ResourceMetadata::GetString(StringID sid, const String& defaultValue)
	{
		if (m_strings.find(sid) == m_strings.end())
			m_strings[sid] = defaultValue;

		return m_strings[sid];
	}

	StringID ResourceMetadata::GetSID(StringID sid, StringID defValue)
	{
		if (m_sids.find(sid) == m_sids.end())
			m_sids[sid] = defValue;

		return m_sids[sid];
	}

	int ResourceMetadata::GetInt(StringID sid, int defaultValue)
	{
		if (m_ints.find(sid) == m_ints.end())
			m_ints[sid] = defaultValue;

		return m_ints[sid];
	}

	uint8 ResourceMetadata::GetUInt8(StringID sid, uint8 defaultValue)
	{
		if (m_uint8s.find(sid) == m_uint8s.end())
			m_uint8s[sid] = defaultValue;

		return m_uint8s[sid];
	}

	float ResourceMetadata::GetFloat(StringID sid, float defaultValue)
	{
		if (m_floats.find(sid) == m_floats.end())
			m_floats[sid] = defaultValue;

		return m_floats[sid];
	}

	bool ResourceMetadata::GetBool(StringID sid, bool defaultValue)
	{
		if (m_bools.find(sid) == m_bools.end())
			m_bools[sid] = defaultValue;

		return m_bools[sid];
	}

	void ResourceMetadata::SetVector2(StringID sid, const Vector2& value)
	{
		m_vec2s[sid] = value;
	}

	void ResourceMetadata::SetVector3(StringID sid, const Vector3& value)
	{
		m_vec3s[sid] = value;
	}

	void ResourceMetadata::SetVector4(StringID sid, const Vector4& value)
	{
		m_vec4s[sid] = value;
	}

	void ResourceMetadata::SetString(StringID sid, const String& value)
	{
		m_strings[sid] = value;
	}

	void ResourceMetadata::SetSID(StringID sid, StringID value)
	{
		m_sids[sid] = value;
	}

	void ResourceMetadata::SetInt(StringID sid, int value)
	{
		m_ints[sid] = value;
	}

	void ResourceMetadata::SetUInt8(StringID sid, uint8 value)
	{
		m_uint8s[sid] = value;
	}

	void ResourceMetadata::SetFloat(StringID sid, float value)
	{
		m_floats[sid] = value;
	}

	void ResourceMetadata::SetBool(StringID sid, bool value)
	{
		m_bools[sid] = value;
	}

	void ResourceMetadata::SaveToStream(OStream& stream)
	{
		stream << m_ints;
		stream << m_uint8s;
		stream << m_bools;
		stream << m_floats;
		stream << m_vec2s;
		stream << m_vec3s;
		stream << m_vec4s;
		stream << m_strings;
	}

	void ResourceMetadata::LoadFromStream(IStream& stream)
	{
		stream >> m_ints;
		stream >> m_uint8s;
		stream >> m_bools;
		stream >> m_floats;
		stream >> m_vec2s;
		stream >> m_vec3s;
		stream >> m_vec4s;
		stream >> m_strings;
	}

	void ResourceMetadata::ClearAll()
	{
		m_vec2s.clear();
		m_vec3s.clear();
		m_vec4s.clear();
		m_ints.clear();
		m_floats.clear();
		m_strings.clear();
	}
} // namespace Lina
