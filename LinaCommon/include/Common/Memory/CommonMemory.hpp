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

#include "Common/Data/Streams.hpp"

namespace Lina
{
	struct BucketIdentifier
	{
		uint16 allocationIndex = 0;
		uint8  bucketIndex	   = 0;
		uint8  isValid		   = 0;

		void SaveToStream(OStream& stream) const
		{
			stream << allocationIndex << bucketIndex;
		}

		void LoadFromStream(IStream& stream)
		{
			stream >> allocationIndex >> bucketIndex;
		}
	};

#define ALLOCATOR_BUCKET_MEM                                                                                                                                                                                                                                       \
	template <typename U, int N> friend class AllocatorBucket;                                                                                                                                                                                                     \
	BucketIdentifier m_bucketIdent = {};

#define ALLOCATOR_BUCKET_GET                                                                                                                                                                                                                                       \
	inline const BucketIdentifier& GetBucketIdent()                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                              \
		return m_bucketIdent;                                                                                                                                                                                                                                      \
	}

} // namespace Lina
