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
#include "Common/Data/Map.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class Material;
	class MeshDefault;
	class MeshComponent;
	class RenderableComponent;

	struct MaterialComparator
	{
		bool operator()(const Material* lhs, const Material* rhs) const;
	};

	struct InstanceData
	{
		Material* material	  = nullptr;
		uint32	  entityIndex = 0;
	};

	struct DrawDataMeshDefault
	{
		MeshDefault*		 mesh = nullptr;
		Vector<InstanceData> instances;
	};

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
		Vector4 cameraPositionAndNear;
		Vector4 cameraDirectionAndFar;
		Vector2 size;
		Vector2 padding;
		Vector4 padding2;
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
		Vector4 clip;
		Vector2 canvasSize;
		Vector2 padding;
	};

	struct GPUIndirectConstants0
	{
		uint32 entityID;
		uint32 materialByteIndex;
	};

	struct GPUPushConstantRPMain
	{
		uint32 batchStartIndex = 0;
	};

	struct GPUMaterialDefaultSky
	{
		Vector4 color;
	};

	struct GPUMaterialDefaultObject
	{
		uint32 txtAlbedo;
		uint32 defaultSampler;
	};

	struct GPUDataAtmosphere
	{
		Vector4 skyTopAndDiffusion;
		Vector4 skyHorizonAndBase;
		Vector4 skyGroundAndCurvature;
		Vector4 sunLightAndCoef;
		Vector4 sunPosition;
		Vector4 ambientTop;
		Vector4 ambientMiddle;
		Vector4 ambientBottom;
	};

	struct GPUDataDeferredLightingPass
	{
		uint32 gBufAlbedo			= 0;
		uint32 gBufPositionMetallic = 0;
		uint32 gBufNormalRoughness	= 0;
		uint32 gBufAO				= 0;
		uint32 gBufSampler			= 0;
		uint32 checkerTexture		= 0;
		uint32 pad;
		uint32 pad2;
		uint32 pad3;
	};

} // namespace Lina
