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

#include "Common/Platform/LinaGXIncl.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"

namespace Lina::Editor
{
	struct GPUEditorGUIPushConstants
	{
		uint32 materialIndex;
	};

	struct GPUMaterialGUIColorWheel
	{
		float wheelRadius;
		float edgeSmoothness;
	};

	struct GPUMaterialGUIDefault
	{
		Vector4				 uvTilingAndOffset;
		uint32				 hasTexture;
		uint32				 displayChannels;
		uint32				 displayLod;
		uint32				 singleChannel;
		LinaTexture2DBinding diffuse;
	};

	struct GPUMaterialGUIGlitch
	{
		Vector4				 uvTilingAndOffset;
		LinaTexture2DBinding diffuse;
	};

	struct GPUMaterialGUIHueDisplay
	{
		Vector2 uvContribution;
	};

	struct GPUMaterialGUISDFText
	{
		LinaTexture2DBinding diffuse;
		Vector4				 outlineColor;
		float				 thickness;
		float				 softness;
		float				 outlineThickness;
		float				 outlineSoftness;
	};

	struct GPUMaterialGUIText
	{
		LinaTexture2DBinding diffuse;
	};

	enum class GUISpecialType
	{
		None,
		HorizontalHue,
		VerticalHue,
		ColorWheel,
		Glitch,
	};

	enum class DisplayChannels
	{
		RGBA,
		R,
		G,
		B,
		A
	};

	LINA_CLASS_BEGIN(DisplayChannels)
	LINA_PROPERTY_STRING(DisplayChannels, 0, "RGBA")
	LINA_PROPERTY_STRING(DisplayChannels, 1, "R")
	LINA_PROPERTY_STRING(DisplayChannels, 2, "G")
	LINA_PROPERTY_STRING(DisplayChannels, 3, "B")
	LINA_PROPERTY_STRING(DisplayChannels, 4, "A")
	LINA_CLASS_END(DisplayChannels)

	struct GUIUserData
	{
		GUISpecialType	specialType			= GUISpecialType::None;
		DisplayChannels displayChannels		= DisplayChannels::RGBA;
		uint32			mipLevel			= 0;
		ResourceID		sampler				= 0;
		float			sdfThickness		= 0.5f;
		float			sdfSoftness			= 0.02f;
		float			sdfOutlineThickness = 0.0f;
		float			sdfOutlineSoftness	= 0.0f;
		Color			sdfOutlineColor		= Color::Black;
		bool			isSingleChannel		= false;
	};

	class EditorGfxHelpers
	{
	public:
		static RenderPassDescription	  GetGUIPassDescription();
		static LinaGX::DescriptorSetDesc  GetSetDescriptionGUI();
		static LinaGX::PipelineLayoutDesc GetPipelineLayoutDescriptionGlobal();
		static LinaGX::PipelineLayoutDesc GetPipelineLayoutDescriptionGUI();
	};
} // namespace Lina::Editor
