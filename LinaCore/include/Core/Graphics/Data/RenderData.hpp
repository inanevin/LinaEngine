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

#ifndef RenderData_HPP
#define RenderData_HPP

#include "Common/Data/Vector.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Math/Matrix.hpp"

namespace Lina
{
	enum RenderableType
	{
		RenderableSprite	  = 1 << 0,
		RenderableDecal		  = 1 << 1,
		RenderableSky		  = 1 << 2,
		RenderableSkinnedMesh = 1 << 3,
		RenderableStaticMesh  = 1 << 4,
		RenderableParticle	  = 1 << 5,
	};

	struct GPUDataEngineGlobals
	{
		Vector4 mouseScreen;
		Vector4 deltaElapsed;
	};

	struct GPUDataView
	{
		Matrix4 view;
		Matrix4 proj;
		Matrix4 viewProj;
		Vector4 cameraPosition;
		Vector2 cameraNearFar;
		Vector2 screenSize;
	};

	struct GPUDataScene
	{
		Vector4 skyTop;
		Vector4 skyHor;
		Vector4 skyBot;
	};

	struct GPUDataObject
	{
		Matrix4 model;
	};

	struct GPUTexture2D
	{
		uint32 textureIndex;
		uint32 samplerIndex;
		uint32 padding[2];
	};

	struct GPUMaterialGUI
	{
		Vector4 color1;
		Vector4 color2;
		Vector4 floatPack1;
		Vector4 floatPack2;
	};

} // namespace Lina

#endif
