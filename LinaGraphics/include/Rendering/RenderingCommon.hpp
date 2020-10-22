/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: RenderingCommon.hpp

Macros, structs and enums used in Graphics namespace are stored here.

Timestamp: 4/14/2019 11:59:32 AM
*/

#ifndef RenderingCommon_HPP
#define RenderingCommon_HPP

#include "Core/SizeDefinitions.hpp"
#include <string>

namespace LinaEngine::Graphics
{
#define INTERNAL_MAT_PATH "__internal"

	enum BufferUsage
	{
		USAGE_STATIC_DRAW = LINA_GRAPHICS_USAGE_STATIC_DRAW,
		USAGE_STREAM_DRAW = LINA_GRAPHICS_USAGE_STREAM_DRAW,
		USAGE_DYNAMIC_DRAW = LINA_GRAPHICS_USAGE_DYNAMIC_DRAW,
		USAGE_STATIC_COPY = LINA_GRAPHICS_USAGE_STATIC_COPY,
		USAGE_STREAM_COPY = LINA_GRAPHICS_USAGE_STREAM_COPY,
		USAGE_DYNAMIC_COPY = LINA_GRAPHICS_USAGE_DYNAMIC_COPY,
		USAGE_STATIC_READ = LINA_GRAPHICS_USAGE_STATIC_READ,
		USAGE_STREAM_READ = LINA_GRAPHICS_USAGE_STREAM_READ,
		USAGE_DYNAMIC_READ = LINA_GRAPHICS_USAGE_DYNAMIC_READ,
	};

	enum BufferBit
	{
		BIT_COLOR = LINA_GRAPHICS_BUFFERBIT_COLOR,
		BIT_DEPTH = LINA_GRAPHICS_BUFFERBIT_DEPTH,
		BIT_STENCIL = LINA_GRAPHICS_BUFFERBIT_STENCIL
	};

	enum SamplerFilter
	{
		FILTER_NEAREST = LINA_GRAPHICS_SAMPLER_FILTER_NEAREST,
		FILTER_LINEAR = LINA_GRAPHICS_SAMPLER_FILTER_LINEAR,
		FILTER_NEAREST_MIPMAP_NEAREST = LINA_GRAPHICS_SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST,
		FILTER_LINEAR_MIPMAP_NEAREST = LINA_GRAPHICS_SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST,
		FILTER_NEAREST_MIPMAP_LINEAR = LINA_GRAPHICS_SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MIPMAP_LINEAR = LINA_GRAPHICS_SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR,
	};

	enum SamplerWrapMode
	{
		WRAP_CLAMP_EDGE = LINA_GRAPHICS_SAMPLER_WRAP_CLAMP,
		WRAP_CLAMP_MIRROR = LINA_GRAPHICS_SAMPLER_WRAP_CLAMP_MIRROR,
		WRAP_CLAMP_BORDER = LINA_GRAPHICS_SAMPLER_WRAP_CLAMP_BORDER,
		WRAP_REPEAT = LINA_GRAPHICS_SAMPLER_WRAP_REPEAT,
		WRAP_REPEAT_MIRROR = LINA_GRAPHICS_SAMPLER_WRAP_REPEAT_MIRROR
	};

	enum TextureBindMode
	{
		BINDTEXTURE_NONE,
		BINDTEXTURE_TEXTURE,
		BINDTEXTURE_TEXTURE2D = LINA_GRAPHICS_BINDTEXTURE_TEXTURE2D,
		BINDTEXTURE_CUBEMAP = LINA_GRAPHICS_BINDTEXTURE_CUBEMAP,
		BINDTEXTURE_CUBEMAP_POSITIVE_X = LINA_GRAPHICS_BINDTEXTURE_CUBEMAP_POSITIVE_X,
		BINDTEXTURE_TEXTURE2D_MULTISAMPLE = LINA_GRAPHICS_BINDTEXTURE_TEXTURE2D_MULTISAMPLE
	};

	enum PixelFormat
	{
		FORMAT_R = 0,
		FORMAT_RG = 1,
		FORMAT_RGB = 2,
		FORMAT_RGBA = 3,
		FORMAT_RGB16F = 4,
		FORMAT_RGBA16F = 5,
		FORMAT_DEPTH = 6,
		FORMAT_DEPTH_AND_STENCIL = 7,
		FORMAT_SRGB = 8,
		FORMAT_SRGBA = 9,
		FORMAT_DEPTH16 = 61
	};



	enum PrimitiveType
	{
		PRIMITIVE_TRIANGLES = LINA_GRAPHICS_PRIMITIVE_TRIANGLES,
		PRIMITIVE_POINTS = LINA_GRAPHICS_PRIMITIVE_POINTS,
		PRIMITIVE_LINE_STRIP = LINA_GRAPHICS_PRIMITIVE_LINE_STRIP,
		PRIMITIVE_LINE_LOOP = LINA_GRAPHICS_PRIMITIVE_LINE_LOOP,
		PRIMITIVE_LINES = LINA_GRAPHICS_PRIMITIVE_LINES,
		PRIMITIVE_LINE_STRIP_ADJACENCY = LINA_GRAPHICS_PRIMITIVE_LINE_STRIP_ADJACENCY,
		PRIMITIVE_LINES_ADJACENCY = LINA_GRAPHICS_PRIMITIVE_LINES_ADJACENCY,
		PRIMITIVE_TRIANGLE_STRIP = LINA_GRAPHICS_PRIMITIVE_TRIANGLE_STRIP,
		PRIMITIVE_TRIANGLE_FAN = LINA_GRAPHICS_PRIMITIVE_TRIANGLE_FAN,
		PRIMITIVE_TRAINGLE_STRIP_ADJACENCY = LINA_GRAPHICS_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY,
		PRIMITIVE_TRIANGLES_ADJACENCY = LINA_GRAPHICS_PRIMITIVE_TRIANGLES_ADJACENCY,
		PRIMITIVE_PATCHES = LINA_GRAPHICS_PRIMITIVE_PATCHES,
	};

	enum FaceCulling
	{
		FACE_CULL_NONE,
		FACE_CULL_BACK = LINA_GRAPHICS_FACE_CULL_BACK,
		FACE_CULL_FRONT = LINA_GRAPHICS_FACE_CULL_FRONT,
		FACE_CULL_FRONT_AND_BACK = LINA_GRAPHICS_FACE_CULL_FRONT_AND_BACK,
	};

	enum DrawFunc
	{
		DRAW_FUNC_NEVER = LINA_GRAPHICS_DRAW_FUNC_NEVER,
		DRAW_FUNC_ALWAYS = LINA_GRAPHICS_DRAW_FUNC_ALWAYS,
		DRAW_FUNC_LESS = LINA_GRAPHICS_DRAW_FUNC_LESS,
		DRAW_FUNC_GREATER = LINA_GRAPHICS_DRAW_FUNC_GREATER,
		DRAW_FUNC_LEQUAL = LINA_GRAPHICS_DRAW_FUNC_LEQUAL,
		DRAW_FUNC_GEQUAL = LINA_GRAPHICS_DRAW_FUNC_GEQUAL,
		DRAW_FUNC_EQUAL = LINA_GRAPHICS_DRAW_FUNC_EQUAL,
		DRAW_FUNC_NOT_EQUAL = LINA_GRAPHICS_DRAW_FUNC_NOT_EQUAL,
	};

	enum FrameBufferAttachment
	{
		ATTACHMENT_COLOR = LINA_GRAPHICS_ATTACHMENT_COLOR,
		ATTACHMENT_DEPTH = LINA_GRAPHICS_ATTACHMENT_DEPTH,
		ATTACHMENT_STENCIL = LINA_GRAPHICS_ATTACHMENT_STENCIL,
		ATTACHMENT_DEPTH_AND_STENCIL = LINA_GRAPHICS_ATTACHMENT_DEPTHANDSTENCIL,
	};


	enum RenderBufferStorage
	{
		STORAGE_DEPTH = LINA_GRAPHICS_STORAGE_D,
		STORAGE_DEPTH_COMP16 = LINA_GRAPHICS_STORAGE_DC16,
		STORAGE_DEPTH_COMP24 = LINA_GRAPHICS_sTORAGE_DC24,
		STORAGE_DEPTH_COMP32F = LINA_GRAPHICS_STORAGE_DC32F,
		STORAGE_DEPTH24_STENCIL8 = LINA_GRAPHICS_STORAGE_D24S8,
		STORAGE_DEPTH32F_STENCIL8 = LINA_GRAPHICS_STORAGE_D32FS8,
		STORAGE_STENCIL_INDEX8 = LINA_GRAPHICS_STORAGE_SI8
	};

	enum BlendFunc
	{
		BLEND_FUNC_NONE,
		BLEND_FUNC_ONE = LINA_GRAPHICS_BLEND_FUNC_ONE,
		BLEND_FUNC_SRC_ALPHA = LINA_GRAPHICS_BLEND_FUNC_SRC_ALPHA,
		BLEND_FUNC_ONE_MINUS_SRC_ALPHA = LINA_GRAPHICS_BLEND_FUNC_ONE_MINUS_SRC_ALPHA,
		BLEND_FUNC_ONE_MINUS_DST_ALPHA = LINA_GRAPHICS_BLEND_FUNC_ONE_MINUS_DST_ALPHA,
		BLEND_FUNC_DST_ALPHA = LINA_GRAPHICS_BLEND_FUNC_DST_ALPHA,
	};

	enum StencilOp
	{
		STENCIL_KEEP = LINA_GRAPHICS_STENCIL_KEEP,
		STENCIL_ZERO = LINA_GRAPHICS_STENCIL_ZERO,
		STENCIL_REPLACE = LINA_GRAPHICS_STENCIL_REPLACE,
		STENICL_INCR = LINA_GRAPHICS_STENCIL_INCR,
		STENCIL_INCR_WRAP = LINA_GRAPHICS_STENCIL_INCR_WRAP,
		STENCIL_DECR_WRAP = LINA_GRAPHICS_STENCIL_DECR_WRAP,
		STENCIL_DECR = LINA_GRAPHICS_STENCIL_DECR,
		STENCIL_INVERT = LINA_GRAPHICS_STENCIL_INVERT,
	};

	enum UniformType
	{
		UT_Matrix4,
		UT_Vector3,
		UT_Vector4,
		UT_Vector2,
		UT_Float,
		UT_Int
	};

	struct SamplerData
	{
		SamplerFilter m_minFilter = SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR;
		SamplerFilter maxFilter = SamplerFilter::FILTER_LINEAR;
		SamplerWrapMode m_wrapS = SamplerWrapMode::WRAP_CLAMP_EDGE;
		SamplerWrapMode m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		float m_anisotropy = 0.0f;
	};

	struct DrawParams
	{
		PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;
		FaceCulling faceCulling = FACE_CULL_NONE;
		DrawFunc depthFunc = DRAW_FUNC_ALWAYS;
		DrawFunc stencilFunc = DRAW_FUNC_ALWAYS;
		StencilOp stencilFail = STENCIL_KEEP;
		StencilOp stencilPassButDepthFail = STENCIL_KEEP;
		StencilOp stencilPass = STENCIL_REPLACE;
		BlendFunc sourceBlend = BLEND_FUNC_NONE;
		BlendFunc destBlend = BLEND_FUNC_NONE;
		bool shouldWriteDepth = true;
		bool useDepthTest = true;
		bool useStencilTest = false;
		bool useScissorTest = false;
		uint32 scissorStartX = 0;
		uint32 scissorStartY = 0;
		uint32 scissorWidth = 0;
		uint32 scissorHeight = 0;
		uint32 stencilTestMask = 0;
		uint32 stencilWriteMask = 0;
		int32 stencilComparisonVal = 0;
	};

	enum Shaders
	{
		Standard_Unlit = 0,
		Skybox_SingleColor = 1,
		Skybox_Gradient = 2,
		Skybox_Cubemap = 3,
		Skybox_Procedural = 4,
		Skybox_HDRI = 5,
		PBR_Lit = 6,
		HDRI_Equirectangular = 7,
		HDRI_Irradiance = 8,
		HDRI_Prefilter = 9,
		HDRI_BRDF = 10,
		ScreenQuad_Final = 11,
		ScreenQuad_Blur = 12,
		ScreenQuad_Outline = 13,
		ScreenQuad_Shadowmap = 14,
		Debug_Line = 15,
		Standard_Sprite = 16
	};

	extern char* g_shadersStr[17];


	struct RenderingDebugData
	{
		bool visualizeDepth;
	};


	enum MaterialSurfaceType
	{
		Opaque = 0,
		Transparent = 1
	};

	extern char* g_materialSurfaceTypeStr[2];


	enum Primitives
	{
		Plane = 0,
		Cube = 1,
		Sphere = 2,
		Icosphere = 3,
		Cone = 4,
		Cylinder = 5
	};

	struct MeshParameters
	{
		bool m_triangulate = true;
		bool m_smoothNormals = true;
		bool m_calculateTangentSpace = true;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_triangulate, m_smoothNormals, m_calculateTangentSpace);
		}
	};

	struct TextureParameters
	{
		PixelFormat m_pixelFormat = PixelFormat::FORMAT_RGBA;
		PixelFormat m_internalPixelFormat = PixelFormat::FORMAT_RGBA;
		SamplerFilter m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		SamplerFilter m_magFilter = SamplerFilter::FILTER_LINEAR;
		SamplerWrapMode m_wrapS = SamplerWrapMode::WRAP_CLAMP_EDGE;
		SamplerWrapMode m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		SamplerWrapMode m_wrapR = SamplerWrapMode::WRAP_CLAMP_EDGE;
		bool m_generateMipMaps = false;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_pixelFormat, m_internalPixelFormat, m_minFilter, m_magFilter, m_wrapS, m_wrapT, m_wrapR, m_generateMipMaps);
		}
	};

	struct SamplerParameters
	{
		TextureParameters m_textureParams = TextureParameters();
		int m_anisotropy = 0.0f;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_anisotropy, m_textureParams);
		}
	};

	enum class WindowState
	{
		Normal = 0,
		Maximized = 1,
		Iconified = 2
	};

	/* Struct containing basic data about window properties. */
	struct WindowProperties
	{
		std::string m_title;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_xPos = 0;
		unsigned int m_yPos = 0;
		bool vSyncEnabled;
		bool m_decorated = true;
		bool m_resizable = true;
		WindowState m_windowState;

		WindowProperties()
		{
			m_title = "Lina Engine";
			m_width = 1440;
			m_height = 900;
		}

		WindowProperties(const std::string& title, unsigned int width, unsigned int height)
		{
			m_title = title;
			m_width = width;
			m_height = height;
		}
	};


}


#endif