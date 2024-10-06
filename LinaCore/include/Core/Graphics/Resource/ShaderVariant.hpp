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
#include "Common/Data/Streams.hpp"
#include "Common/Data/String.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Reflection/ClassReflection.hpp"

namespace Lina
{
	struct VariantColorTarget
	{
		LinaGX::Format format;

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	struct ShaderVariant
	{
		uint32 _gpuHandle		= 0;
		bool   _gpuHandleExists = false;

		String					   name				   = "";
		bool					   blendDisable		   = false;
		LinaGX::BlendFactor		   blendSrcFactor	   = LinaGX::BlendFactor::SrcAlpha;
		LinaGX::BlendFactor		   blendDstFactor	   = LinaGX::BlendFactor::OneMinusSrcAlpha;
		LinaGX::BlendOp			   blendColorOp		   = LinaGX::BlendOp::Add;
		LinaGX::BlendFactor		   blendSrcAlphaFactor = LinaGX::BlendFactor::One;
		LinaGX::BlendFactor		   blendDstAlphaFactor = LinaGX::BlendFactor::Zero;
		LinaGX::BlendOp			   blendAlphaOp		   = LinaGX::BlendOp::Add;
		bool					   depthTest		   = true;
		bool					   depthWrite		   = true;
		LinaGX::Format			   depthFormat		   = LinaGX::Format::D32_SFLOAT;
		Vector<VariantColorTarget> targets;
		LinaGX::CompareOp		   depthOp			 = LinaGX::CompareOp::Less;
		LinaGX::CullMode		   cullMode			 = LinaGX::CullMode::Back;
		LinaGX::FrontFace		   frontFace		 = LinaGX::FrontFace::CCW;
		LinaGX::Topology		   topology			 = LinaGX::Topology::TriangleList;
		bool					   depthBiasEnable	 = false;
		float					   depthBiasConstant = 0.0f;
		float					   depthBiasClamp	 = 0.0f;
		float					   depthBiasSlope	 = 0.0f;

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	LINA_CLASS_BEGIN(ShaderVariant)
	LINA_FIELD(ShaderVariant, name, "Name", FieldType::String, 0)
	LINA_FIELD(ShaderVariant, blendDisable, "Blend Disable", FieldType::Boolean, 0)
	LINA_FIELD(ShaderVariant, blendSrcFactor, "Src Factor", FieldType::Enum, GetTypeID<LinaGX::BlendFactor>())
	LINA_FIELD(ShaderVariant, blendDstFactor, "Dst Factor", FieldType::Enum, GetTypeID<LinaGX::BlendFactor>())
	LINA_FIELD(ShaderVariant, blendColorOp, "Color Op", FieldType::Enum, GetTypeID<LinaGX::BlendOp>())
	LINA_FIELD(ShaderVariant, blendSrcAlphaFactor, "Src Alpha Factor", FieldType::Enum, GetTypeID<LinaGX::BlendFactor>())
	LINA_FIELD(ShaderVariant, blendDstAlphaFactor, "Dst Alpha Factor", FieldType::Enum, GetTypeID<LinaGX::BlendFactor>())
	LINA_FIELD(ShaderVariant, blendAlphaOp, "Alpha Op", FieldType::Enum, GetTypeID<LinaGX::BlendOp>())
	LINA_FIELD(ShaderVariant, depthTest, "Depth Test", FieldType::Boolean, 0)
	LINA_FIELD(ShaderVariant, depthWrite, "Depth Write", FieldType::Boolean, 0)
	LINA_FIELD(ShaderVariant, depthOp, "Depth Op", FieldType::Enum, GetTypeID<LinaGX::CompareOp>())
	LINA_FIELD(ShaderVariant, depthBiasEnable, "Depth Bias", FieldType::Boolean, 0)
	LINA_FIELD(ShaderVariant, depthBiasConstant, "Bias Constant", FieldType::Float, 0)
	LINA_FIELD(ShaderVariant, depthBiasClamp, "Bias Constant", FieldType::Float, 0)
	LINA_FIELD(ShaderVariant, depthBiasSlope, "Bias Constant", FieldType::Float, 0)

	LINA_FIELD(ShaderVariant, cullMode, "Cull Mode", FieldType::Enum, GetTypeID<LinaGX::CullMode>())
	LINA_FIELD(ShaderVariant, frontFace, "Front Face", FieldType::Enum, GetTypeID<LinaGX::FrontFace>())
	LINA_FIELD(ShaderVariant, topology, "Topology", FieldType::Enum, GetTypeID<LinaGX::Topology>())

	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, blendSrcFactor, "blendDisable", 0)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, blendDstFactor, "blendDisable", 0)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, blendColorOp, "blendDisable", 0)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, blendSrcAlphaFactor, "blendDisable", 0)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, blendDstAlphaFactor, "blendDisable", 0)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, blendAlphaOp, "blendDisable", 0)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, depthBiasConstant, "depthBiasEnable", 1)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, depthBiasClamp, "depthBiasEnable", 1)
	LINA_FIELD_DEPENDENCY_POS(ShaderVariant, depthBiasSlope, "depthBiasEnable", 1)
	LINA_CLASS_END(ShaderVariant)

} // namespace Lina
