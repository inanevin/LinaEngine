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

#include "Common/Data/Vector.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Math/Matrix.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class Material;
	class Buffer;

	struct MaterialComparator
	{
		bool operator()(const Material* lhs, const Material* rhs) const;
	};

	enum RenderPassType
	{
		RENDER_PASS_DEFERRED = 0,
		RENDER_PASS_FORWARD,
		RENDER_PASS_SWAPCHAIN,
		RENDER_PASS_UNKNOWN,
	};

	enum MeshFormat
	{
		MESH_FORMAT_STATIC = 0,
		MESH_FORMAT_SKINNED,
		MESH_FORMAT_MAX,
	};

	constexpr const char* RPTypeToString(RenderPassType type)
	{
		switch (type)
		{
		case RENDER_PASS_DEFERRED:
			return "Main";
		case RENDER_PASS_FORWARD:
			return "ForwardTransparency";
		default:
			return "Unknown";
		}
	}

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
		Vector4 deltaElapsed;
	};

	struct GPUDataDeferredPass
	{
		Matrix4 view;
		Matrix4 proj;
		Matrix4 viewProj;
		Vector4 padding;
		Vector4 padding1;
		Vector4 padding2;
		Vector4 padding3;
	};

	struct GPUDataForwardPass
	{
		Matrix4 view;
		Matrix4 proj;
		Matrix4 viewProj;
		Matrix4 orthoProj;
		Vector4 ambientTop;
		Vector4 ambientMid;
		Vector4 ambientBot;
		Vector4 cameraPositionAndNear;
		Vector4 cameraDirectionAndFar;
		Vector4 sizeAndMouse;
		uint32	gBufAlbedo;
		uint32	gBufPositionMetallic;
		uint32	gBufNormalRoughness;
		uint32	gBufSampler;
		uint32	lightCount;
		float	padding0;
		float	padding1;
		float	padding2;
	};

	struct GPUDataSwapchainPass
	{
		uint32 textureIndex;
		uint32 samplerIndex;
		uint32 padding0;
		uint32 padding1;
	};

	struct GPUMaterialGUI
	{
		Vector4 color1;
		Vector4 color2;
		Vector4 floatPack1;
		Vector4 floatPack2;
		Vector4 clip;
		Vector2 canvasSize;
		Vector2 padding;
	};

	struct GPUEntity
	{
		Matrix4 model;
		Matrix4 normal;
		Vector4 position;
		Vector4 forward;
		float	padding[24];
	};

	struct GPUDrawArguments
	{
		uint32 constant0;
		uint32 constant1;
		uint32 constant2;
		uint32 constant3;
	};

	struct Line3DVertex
	{
		Vector3 position;
		Vector3 nextPosition;
		Vector4 color;
		float	direction;
	};

	struct EntityIdent
	{
		EntityID entityGUID = 0;
		uint32	 uniqueID2	= 0;
		uint32	 uniqueID3	= 0;
		uint32	 uniqueID4	= 0;

		bool operator==(const EntityIdent& other) const
		{
			return entityGUID == other.entityGUID && uniqueID2 == other.uniqueID2 && uniqueID3 == other.uniqueID3 && uniqueID4 == other.uniqueID4;
		}
	};

	struct DrawEntity
	{
		GPUEntity	entity;
		EntityIdent ident;
	};

	struct GPULight
	{
		Vector4 colorAndIntensity;
		uint32	type;
		uint32	entityIndex;
		float	radius;
		float	falloff;
		float	cutoff;
		float	outerCutoff;
		float	padding[22];
	};

	struct PhysicsDebugVertex
	{
		Vector3 position;
		Vector4 color;
	};

	template <typename T, int N> struct BufferedGroup
	{
		T* items[N];
	};

} // namespace Lina
