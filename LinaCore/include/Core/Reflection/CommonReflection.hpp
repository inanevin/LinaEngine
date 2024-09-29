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

#include "Common/Reflection/ClassReflection.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	LINA_CLASS_BEGIN(Direction)
	LINA_PROPERTY_STRING(Direction, 0, "Top")
	LINA_PROPERTY_STRING(Direction, 1, "Bottom")
	LINA_PROPERTY_STRING(Direction, 2, "Left")
	LINA_PROPERTY_STRING(Direction, 3, "Right")
	LINA_PROPERTY_STRING(Direction, 4, "Center")
	LINA_CLASS_END(Direction)

	LINA_CLASS_BEGIN(DirectionOrientation)
	LINA_PROPERTY_STRING(DirectionOrientation, 0, "Horizontal")
	LINA_PROPERTY_STRING(DirectionOrientation, 1, "Vertical")
	LINA_CLASS_END(DirectionOrientation)

	LINA_CLASS_BEGIN(LinaVGAlignment)
	LINA_PROPERTY_STRING(LinaVG::TextAlignment, 0, "Left")
	LINA_PROPERTY_STRING(LinaVG::TextAlignment, 1, "Center")
	LINA_PROPERTY_STRING(LinaVG::TextAlignment, 2, "Right")
	LINA_CLASS_END(LinaVGAlignment)

	LINA_CLASS_BEGIN(LinaGXFormat)
	LINA_PROPERTY_STRING(LinaGX::Format, 0, "UNDEFINED")
	LINA_PROPERTY_STRING(LinaGX::Format, 1, "R8_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 2, "R8_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 3, "R8_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 4, "R8_SNORM")

	LINA_PROPERTY_STRING(LinaGX::Format, 5, "R8G8_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 6, "R8G8_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 7, "R8G8_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 8, "R8G8_SNORM")

	LINA_PROPERTY_STRING(LinaGX::Format, 9, "R8G8B8A8_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 10, "R8G8B8A8_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 11, "R8G8B8A8_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 12, "R8G8B8A8_SNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 13, "R8G8B8A8_SRGB")

	LINA_PROPERTY_STRING(LinaGX::Format, 14, "B8G8R8A8_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 15, "B8G8R8A8_SRGB")

	LINA_PROPERTY_STRING(LinaGX::Format, 16, "R16_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 17, "R16_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 18, "R16_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 19, "R16_SNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 20, "R16_SFLOAT")

	LINA_PROPERTY_STRING(LinaGX::Format, 21, "R16G16_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 22, "R16G16_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 23, "R16G16_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 24, "R16G16_SNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 25, "R16G16_SFLOAT")

	LINA_PROPERTY_STRING(LinaGX::Format, 26, "R16G16B16A16_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 27, "R16G16B16A16_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 28, "R16G16B16A16_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 29, "R16G16B16A16_SNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 30, "R16G16B16A16_SFLOAT")

	LINA_PROPERTY_STRING(LinaGX::Format, 31, "R32_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 32, "R32_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 33, "R32_SFLOAT")

	LINA_PROPERTY_STRING(LinaGX::Format, 34, "R32G32_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 35, "R32G32_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 36, "R32G32_SFLOAT")

	LINA_PROPERTY_STRING(LinaGX::Format, 37, "R32G32B32_SFLOAT")
	LINA_PROPERTY_STRING(LinaGX::Format, 38, "R32G32B32_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 39, "R32G32B32_UINT")

	LINA_PROPERTY_STRING(LinaGX::Format, 40, "R32G32B32A32_SINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 41, "R32G32B32A32_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 42, "R32G32B32A32_SFLOAT")

	LINA_PROPERTY_STRING(LinaGX::Format, 43, "D32_SFLOAT")
	LINA_PROPERTY_STRING(LinaGX::Format, 44, "D24_UNORM_S8_UINT")
	LINA_PROPERTY_STRING(LinaGX::Format, 45, "D16_UNORM")

	LINA_PROPERTY_STRING(LinaGX::Format, 46, "R11G11B10_SFLOAT")
	LINA_PROPERTY_STRING(LinaGX::Format, 47, "R10G0B10A2_INT")
	LINA_PROPERTY_STRING(LinaGX::Format, 48, "BC3_BLOCK_SRGB")
	LINA_PROPERTY_STRING(LinaGX::Format, 49, "BC3_BLOCK_UNORM")
	LINA_PROPERTY_STRING(LinaGX::Format, 50, "R11G11B10_SFLOAT")

	LINA_CLASS_END(LinaGXFormat)

	LINA_CLASS_BEGIN(LinaGXMipmapMode)
	LINA_PROPERTY_STRING(LinaGX::MipmapMode, 0, "Nearest")
	LINA_PROPERTY_STRING(LinaGX::MipmapMode, 1, "Linear")
	LINA_CLASS_END(LinaGXMipmapMode)

	LINA_CLASS_BEGIN(LinaGXMipmapFilter)
	LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 0, "Default")
	LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 1, "Box")
	LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 2, "Triangle")
	LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 3, "CubicSpline")
	LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 4, "CatmullRom")
	LINA_PROPERTY_STRING(LinaGX::MipmapFilter, 5, "Mitchell")
	LINA_CLASS_END(LinaGXMipmapFilter)

	LINA_CLASS_BEGIN(LinaGXFilter)
	LINA_PROPERTY_STRING(LinaGX::Filter, 0, "Anisotropic")
	LINA_PROPERTY_STRING(LinaGX::Filter, 1, "Nearest")
	LINA_PROPERTY_STRING(LinaGX::Filter, 2, "Linear")
	LINA_CLASS_END(LinaGXFilter)

} // namespace Lina
