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

#include "Graphics/Core/DrawPass.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "World/Core/Entity.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Core/View.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina
{
	DrawPass::DrawPass(GfxManager* gfxMan) : m_gfxManager(gfxMan)
	{
	}

	DrawPass::~DrawPass()
	{
	}

} // namespace Lina
