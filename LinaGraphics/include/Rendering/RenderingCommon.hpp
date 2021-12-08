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
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Matrix.hpp"
#include "Utility/Math/Color.hpp"
#include "map"

namespace LinaEngine::Graphics
{
#define INTERNAL_MAT_PATH "__internal"
#define MAX_POINT_LIGHTS 12
#define RENDERSETTINGS_FULLPATH "resources/engine/defaultSettings.rendersettings"
#define RENDERSETTINGS_FOLDERPATH "resources/engine"
#define RENDERSETTINGS_FILE "defaultSettings"
#define MAX_BONE_INFLUENCE 4

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
		FORMAT_DEPTH16 = 10
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

	struct ShaderSamplerData
	{
		uint32 m_unit = 0;
		TextureBindMode m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
	};

	struct ShaderUniformData
	{
		std::map<std::string, float> m_floats;
		std::map<std::string, int> m_ints;
		std::map<std::string, ShaderSamplerData> m_sampler2Ds;
		std::map<std::string, Color> m_colors;
		std::map<std::string, Vector2> m_vector2s;
		std::map<std::string, Vector3> m_vector3s;
		std::map<std::string, Vector4> m_vector4s;
		std::map<std::string, Matrix> m_matrices;
		std::map<std::string, bool> m_bools;
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
		bool skipParameters = false;
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

	struct ModelParameters
	{
		bool m_triangulate = true;
		bool m_smoothNormals = true;
		bool m_calculateTangentSpace = true;
		bool m_flipWinding = false;
		bool m_flipUVs = false;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_triangulate, m_smoothNormals, m_calculateTangentSpace, m_flipUVs, m_flipWinding);
		}
	};

	struct DebugLine
	{
		Vector3 m_from = Vector3::Zero;
		Vector3 m_to = Vector3::Zero;
		Color m_color = Color::White;
		float m_width = 0.0f;
	};

	struct VertexBoneData
	{
		int m_boneIDs[MAX_BONE_INFLUENCE];
		float m_boneWeights[MAX_BONE_INFLUENCE];
	};

	struct BoneInfo
	{
		int m_id;
		Matrix m_offset;
	};

	struct ModelSceneParameters
	{
		Vector3 m_worldPosition;
		Quaternion m_worldRotation;
		Vector3 m_worldScale;
		Matrix m_rootInverse;
	};

	struct TextureParameters
	{
		PixelFormat m_pixelFormat = PixelFormat::FORMAT_RGBA;
		PixelFormat m_internalPixelFormat = PixelFormat::FORMAT_RGBA;
		SamplerFilter m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		SamplerFilter m_magFilter = SamplerFilter::FILTER_LINEAR;
		SamplerWrapMode m_wrapS = SamplerWrapMode::WRAP_REPEAT;
		SamplerWrapMode m_wrapT = SamplerWrapMode::WRAP_REPEAT;
		SamplerWrapMode m_wrapR = SamplerWrapMode::WRAP_REPEAT;
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
		bool m_fullscreen = false;
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