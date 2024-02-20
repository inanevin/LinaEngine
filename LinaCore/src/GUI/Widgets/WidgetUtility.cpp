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

#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Rect.hpp"

namespace Lina
{
	void WidgetUtility::DrawRectBackground(int32 threadIndex, const RectBackground& options, const Rect& rect, int32 drawOrder, bool positionIsCenter)
	{
		if (!options.enabled)
			return;

		LinaVG::StyleOptions opts;
		opts.color.start		 = options.startColor.AsLVG4();
		opts.color.end			 = options.endColor.AsLVG4();
		opts.rounding			 = options.rounding;
		const LinaVG::Vec2 start = positionIsCenter ? (rect.pos - rect.size * 0.5f).AsLVG() : (rect.pos).AsLVG();
		const LinaVG::Vec2 end	 = positionIsCenter ? (rect.pos + rect.size * 0.5f).AsLVG() : (rect.pos + rect.size).AsLVG();
		LinaVG::DrawRect(threadIndex, start, end, opts, 0.0f, drawOrder);
	}
} // namespace Lina