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

#ifndef ResourceMetadata_HPP
#define ResourceMetadata_HPP

#include "Common/Data/String.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/SizeDefinitions.hpp"
#include "Common/StringID.hpp"

namespace Lina
{
	class ResourceMetadata
	{
	public:
		Vector2	 GetVector2(StringID sid, const Vector2& defaultValue = Vector2::Zero);
		Vector3	 GetVector3(StringID sid, const Vector3& defaultValue = Vector2::Zero);
		Vector4	 GetVector4(StringID sid, const Vector4& defaultValue = Vector2::Zero);
		String	 GetString(StringID sid, const String& defaultValue = "");
		StringID GetSID(StringID sid, StringID defValue);
		int		 GetInt(StringID sid, int defaultValue = 0);
		uint8	 GetUInt8(StringID sid, uint8 defaultValue = 0);
		float	 GetFloat(StringID sid, float defaultValue = 0.0f);
		bool	 GetBool(StringID sid, bool defaultValue = false);

		void SetVector2(StringID sid, const Vector2& value);
		void SetVector3(StringID sid, const Vector3& value);
		void SetVector4(StringID sid, const Vector4& value);
		void SetString(StringID sid, const String& value);
		void SetSID(StringID sid, StringID value);
		void SetInt(StringID sid, int value);
		void SetUInt8(StringID sid, uint8 value);
		void SetFloat(StringID sid, float value);
		void SetBool(StringID sid, bool value);

		void SaveToStream(OStream& stream);
		void LoadFromStream(IStream& stream);

		void ClearAll();

	private:
		HashMap<StringID, Vector2>	m_vec2s;
		HashMap<StringID, Vector3>	m_vec3s;
		HashMap<StringID, Vector4>	m_vec4s;
		HashMap<StringID, String>	m_strings;
		HashMap<StringID, int>		m_ints;
		HashMap<StringID, uint8>	m_uint8s;
		HashMap<StringID, float>	m_floats;
		HashMap<StringID, bool>		m_bools;
		HashMap<StringID, StringID> m_sids;
	};

} // namespace Lina

#endif
