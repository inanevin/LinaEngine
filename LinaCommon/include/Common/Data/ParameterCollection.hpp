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

#include "Common/StringID.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/String.hpp"

namespace Lina
{
	class OStream;
	class IStream;

	class ParameterCollection
	{
	private:
		static constexpr uint32 VERSION = 0;

		struct ColUint8
		{
			StringID sid = 0;
			uint8	 val = 0;

			void SaveToStream(OStream& stream) const
			{
				stream << sid << val;
			}
			void LoadFromStream(IStream& stream)
			{
				stream >> sid >> val;
			}
		};

		struct ColUint32
		{
			StringID sid = 0;
			uint32	 val = 0;

			void SaveToStream(OStream& stream) const
			{
				stream << sid << val;
			}
			void LoadFromStream(IStream& stream)
			{
				stream >> sid >> val;
			}
		};

		struct ColInt32
		{
			StringID sid = 0;
			int32	 val = 0;

			void SaveToStream(OStream& stream) const
			{
				stream << sid << val;
			}
			void LoadFromStream(IStream& stream)
			{
				stream >> sid >> val;
			}
		};

		struct ColFloat
		{
			StringID sid = 0;
			float	 val = 0.0f;
			void	 SaveToStream(OStream& stream) const
			{
				stream << sid << val;
			}
			void LoadFromStream(IStream& stream)
			{
				stream >> sid >> val;
			}
		};

		struct ColString
		{
			StringID sid = 0;
			String	 val = "";
			void	 SaveToStream(OStream& stream) const
			{
				stream << sid << val;
			}
			void LoadFromStream(IStream& stream)
			{
				stream >> sid >> val;
			}
		};

		struct ColResourceID
		{
			StringID   sid = 0;
			ResourceID val = 0;
			void	   SaveToStream(OStream& stream) const
			{
				stream << sid << val;
			}
			void LoadFromStream(IStream& stream)
			{
				stream >> sid >> val;
			}
		};

	public:
		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

		void  SetParamUint8(StringID sid, uint8 val);
		uint8 GetParamUint8(StringID sid, uint8 defaultVal = 0);

		void   SetParamUint32(StringID sid, uint32 val);
		uint32 GetParamUint32(StringID sid, uint32 defaultVal = 0);

		void  SetParamInt32(StringID sid, int32 val);
		int32 GetParamInt32(StringID sid, int32 defaultVal = 0);

		void  SetParamFloat(StringID sid, float val);
		float GetParamFloat(StringID sid, float defaultVal = 0.0f);

		void   SetParamString(StringID sid, const String& val);
		String GetParamString(StringID sid, const String& defaultVal = "");

		void	   SetParamResourceID(StringID sid, ResourceID val);
		ResourceID GetParamResourceID(StringID sid, ResourceID defaultVal = 0);

	public:
		Vector<ColUint8>	  m_uint8s;
		Vector<ColUint32>	  m_uint32s;
		Vector<ColInt32>	  m_int32s;
		Vector<ColFloat>	  m_floats;
		Vector<ColString>	  m_strings;
		Vector<ColResourceID> m_resIDs;
	};
} // namespace Lina
