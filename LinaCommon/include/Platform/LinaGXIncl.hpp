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

#ifndef LinaGXIncl_HPP
#define LinaGXIncl_HPP

#include "Data/Vector.hpp"
#include "Data/Queue.hpp"
#include "Data/String.hpp"
#include "Data/Deque.hpp"
#include "Data/HashMap.hpp"
#include "Memory/Memory.hpp"
#include "Core/StringID.hpp"
#include "Math/Vector.hpp"
#include "Math/Rect.hpp"

#ifndef LINAGX_VEC
#define LINAGX_VEC Lina::Vector
#endif

#ifndef INAGX_QUEUE
#define LINAGX_QUEUE Lina::Queue
#endif

#ifndef LINAGX_STRING
#define LINAGX_STRING Lina::String
#endif

#ifndef LINAGX_TOSTRING
#define LINAGX_TOSTRING Lina::TO_STRING
#endif

#ifndef LINAGX_WSTRING
#define LINAGX_WSTRING Lina::WString
#endif

#ifndef LINAGX_MAP
#define LINAGX_MAP Lina::HashMap
#endif

#ifndef LINAGX_DEQUE
#define LINAGX_DEQUE Lina::Deque
#endif

#ifndef LINAGX_MALLOC
#define LINAGX_MALLOC(...) MALLOC(__VA_ARGS__)
#endif

#ifndef LINAGX_MEMCPY
#define LINAGX_MEMCPY(...) MEMCPY(__VA_ARGS__)
#endif

#ifndef LINAGX_FREE
#define LINAGX_FREE(...) FREE(__VA_ARGS__)
#endif

#ifndef LINAGX_STRINGID
#define LINAGX_STRINGID Lina::StringID
#endif

#ifndef LINAGX_TYPEID
#define LINAGX_TYPEID Lina::TypeID
#endif

#ifndef LGXVector3
#define LGXVector3 Lina::Vector3
#endif

#ifndef LGXVector2
#define LGXVector2 Lina::Vector2
#endif

#ifndef LGXVector2ui
#define LGXVector2ui Lina::Vector2ui
#endif

#ifndef LGXVector2i
#define LGXVector2i Lina::Vector2i
#endif

#ifndef LGXVector4
#define LGXVector4 Lina::Vector4
#endif

#ifndef LGXVector4ui16
#define LGXVector4ui16 Lina::Vector4ui16
#endif

#ifndef LGXRectui
#define LGXRectui Lina::Rectui
#endif

#include "LinaGX/LinaGX.hpp"

#endif
