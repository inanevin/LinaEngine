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

#include "Math/AABB.hpp"
#include "Data/Vector.hpp"
#include "Data/Bitmask.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

namespace Lina
{
	class RenderableComponent;
	class Mesh;
	class Material;

#define OBJ_BUFFER_MAX 15
#define MAX_LIGHTS	   10

	enum RenderableType
	{
		RenderableSprite	  = 1 << 0,
		RenderableDecal		  = 1 << 1,
		RenderableSky		  = 1 << 2,
		RenderableSkinnedMesh = 1 << 3,
		RenderableStaticMesh  = 1 << 4,
		RenderableParticle	  = 1 << 5,
	};

	enum DrawPassMask
	{
		Opaque		= 1 << 0,
		Transparent = 1 << 1,
		Shadow		= 1 << 2,
	};

	struct GPUGlobalData
	{
		Vector2 mousePosition = Vector2::Zero;
		float	deltaTime	  = 0.0f;
		float	elapsedTime	  = 0.0f;
	};

	struct GPUSceneData
	{
		Matrix4 view;
		Matrix4 proj;
		Matrix4 viewProj;
		Vector4 cameraPosition;
		Vector2 cameraNearFar;
		Vector2 screenSize;
	};

	struct GPULinaTexture2D
	{
		uint32 textureIndex;
		uint32 samplerIndex;
	};

} // namespace Lina

#endif
