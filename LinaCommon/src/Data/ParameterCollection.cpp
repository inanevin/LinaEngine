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

#include "Common/Data/ParameterCollection.hpp"

namespace Lina
{
	void ParameterCollection::SaveToStream(OStream& stream) const
	{
		stream << VERSION;
		stream << m_uint8s;
		stream << m_uint32s;
		stream << m_floats;
		stream << m_strings;
		stream << m_int32s;
		stream << m_resIDs;
	}

	void ParameterCollection::LoadFromStream(IStream& stream)
	{
		uint32 version = 0;
		stream >> version;
		stream >> m_uint8s;
		stream >> m_uint32s;
		stream >> m_floats;
		stream >> m_strings;
		stream >> m_int32s;
		stream >> m_resIDs;
	}

	void ParameterCollection::SetParamUint8(StringID sid, uint8 val)
	{
		auto it = linatl::find_if(m_uint8s.begin(), m_uint8s.end(), [sid](const ColUint8& col) -> bool { return col.sid == sid; });
		if (it != m_uint8s.end())
		{
			it->val = val;
			return;
		}

		m_uint8s.push_back({.sid = sid, .val = val});
	}

	uint8 ParameterCollection::GetParamUint8(StringID sid, uint8 defaultVal)
	{
		auto it = linatl::find_if(m_uint8s.begin(), m_uint8s.end(), [sid](const ColUint8& col) -> bool { return col.sid == sid; });
		if (it != m_uint8s.end())
			return it->val;

		m_uint8s.push_back({.sid = sid, .val = defaultVal});
		return defaultVal;
	}

	void ParameterCollection::SetParamUint32(StringID sid, uint32 val)
	{
		auto it = linatl::find_if(m_uint32s.begin(), m_uint32s.end(), [sid](const ColUint32& col) -> bool { return col.sid == sid; });
		if (it != m_uint32s.end())
		{
			it->val = val;
			return;
		}

		m_uint32s.push_back({.sid = sid, .val = val});
	}

	uint32 ParameterCollection::GetParamUint32(StringID sid, uint32 defaultVal)
	{
		auto it = linatl::find_if(m_uint32s.begin(), m_uint32s.end(), [sid](const ColUint32& col) -> bool { return col.sid == sid; });
		if (it != m_uint32s.end())
			return it->val;

		m_uint32s.push_back({.sid = sid, .val = defaultVal});
		return defaultVal;
	}

	void ParameterCollection::SetParamInt32(StringID sid, int32 val)
	{
		auto it = linatl::find_if(m_int32s.begin(), m_int32s.end(), [sid](const ColInt32& col) -> bool { return col.sid == sid; });
		if (it != m_int32s.end())
		{
			it->val = val;
			return;
		}

		m_int32s.push_back({.sid = sid, .val = val});
	}

	int32 ParameterCollection::GetParamInt32(StringID sid, int32 defaultVal)
	{
		auto it = linatl::find_if(m_int32s.begin(), m_int32s.end(), [sid](const ColInt32& col) -> bool { return col.sid == sid; });
		if (it != m_int32s.end())
			return it->val;

		m_int32s.push_back({.sid = sid, .val = defaultVal});
		return defaultVal;
	}

	void ParameterCollection::SetParamFloat(StringID sid, float val)
	{
		auto it = linatl::find_if(m_floats.begin(), m_floats.end(), [sid](const ColFloat& col) -> bool { return col.sid == sid; });
		if (it != m_floats.end())
		{
			it->val = val;
			return;
		}

		m_floats.push_back({.sid = sid, .val = val});
	}

	float ParameterCollection::GetParamFloat(StringID sid, float defaultVal)
	{
		auto it = linatl::find_if(m_floats.begin(), m_floats.end(), [sid](const ColFloat& col) -> bool { return col.sid == sid; });
		if (it != m_floats.end())
			return it->val;

		m_floats.push_back({.sid = sid, .val = defaultVal});
		return defaultVal;
	}

	void ParameterCollection::SetParamString(StringID sid, const String& val)
	{
		auto it = linatl::find_if(m_strings.begin(), m_strings.end(), [sid](const ColString& col) -> bool { return col.sid == sid; });
		if (it != m_strings.end())
		{
			it->val = val;
			return;
		}

		m_strings.push_back({.sid = sid, .val = val});
	}

	String ParameterCollection::GetParamString(StringID sid, const String& defaultVal)
	{
		auto it = linatl::find_if(m_strings.begin(), m_strings.end(), [sid](const ColString& col) -> bool { return col.sid == sid; });
		if (it != m_strings.end())
			return it->val;

		m_strings.push_back({.sid = sid, .val = defaultVal});
		return defaultVal;
	}

	void ParameterCollection::SetParamResourceID(StringID sid, ResourceID val)
	{
		auto it = linatl::find_if(m_resIDs.begin(), m_resIDs.end(), [sid](const ColResourceID& col) -> bool { return col.sid == sid; });
		if (it != m_resIDs.end())
		{
			it->val = val;
			return;
		}

		m_resIDs.push_back({.sid = sid, .val = val});
	}

	ResourceID ParameterCollection::GetParamResourceID(StringID sid, ResourceID defaultVal)
	{
		auto it = linatl::find_if(m_resIDs.begin(), m_resIDs.end(), [sid](const ColResourceID& col) -> bool { return col.sid == sid; });
		if (it != m_resIDs.end())
			return it->val;

		m_resIDs.push_back({.sid = sid, .val = defaultVal});
		return defaultVal;
	}

} // namespace Lina
