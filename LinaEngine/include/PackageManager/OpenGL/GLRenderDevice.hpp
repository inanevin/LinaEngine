/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLRenderDevice
Timestamp: 4/27/2019 10:12:16 PM

*/

#pragma once

#ifndef GLRenderDevice_HPP
#define GLRenderDevice_HPP

#include "PackageManager/OpenGL/GLWindow.hpp"
#include "Core/DataStructures.hpp"
#include "Utility/Math/Matrix.hpp"
#include "Utility/Math/Color.hpp"


namespace LinaEngine
{
	namespace ECS
	{
		class LightingSystem;
	}
}

using namespace LinaEngine;


namespace LinaEngine::Graphics
{
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
		WRAP_CLAMP = LINA_GRAPHICS_SAMPLER_WRAP_CLAMP,
		WRAP_REPEAT = LINA_GRAPHICS_SAMPLER_WRAP_REPEAT,
		WRAP_CLAMP_MIRROR = LINA_GRAPHICS_SAMPLER_WRAP_CLAMP_MIRROR,
		WRAP_REPEAT_MIRROR = LINA_GRAPHICS_SAMPLER_WRAP_REPEAT_MIRROR,
	};

	enum BindTextureMode
	{
		BINDTEXTURE_TEXTURE2D = LINA_GRAPHICS_BINDTEXTURE_TEXTURE2D,
		BINDTEXTURE_CUBEMAP = LINA_GRAPHICS_BINDTEXTURE_CUBEMAP
	};

	enum PixelFormat
	{
		FORMAT_R,
		FORMAT_RG,
		FORMAT_RGB,
		FORMAT_RGBA,
		FORMAT_DEPTH,
		FORMAT_DEPTH_AND_STENCIL,
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

	enum FramebufferAttachment
	{
		ATTACHMENT_COLOR = LINA_GRAPHICS_ATTACHMENT_COLOR,
		ATTACHMENT_DEPTH = LINA_GRAPHICS_ATTACHMENT_DEPTH,
		ATTACHMENT_STENCIL = LINA_GRAPHICS_ATTACHMENT_STENCIL,
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
		Matrix4,
		Vector3,
		Vector4,
		Vector2,
		Float,
		Int
	};

	struct DrawParams
	{
		PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;
		FaceCulling faceCulling = FACE_CULL_NONE;
		DrawFunc depthFunc = DRAW_FUNC_ALWAYS;
		DrawFunc stencilFunc = DRAW_FUNC_ALWAYS;
		StencilOp stencilFail = STENCIL_KEEP;
		StencilOp stencilPassButDepthFail = STENCIL_KEEP;
		StencilOp stencilPass = STENCIL_KEEP;
		BlendFunc sourceBlend = BLEND_FUNC_NONE;
		BlendFunc destBlend = BLEND_FUNC_NONE;
		bool shouldWriteDepth = true;
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

	// Vertex array struct for storage & vertex array data transportation.
	struct VertexArrayData
	{
		uint32* buffers;
		uintptr* bufferSizes;
		uint32  numBuffers;
		uint32  numElements;
		uint32  instanceComponentsStartIndex;
		BufferUsage bufferUsage;
	};

	// Shader program struct for storage.
	struct ShaderProgram
	{
		LinaArray<uint32> shaders;
		LinaMap<LinaString, int32> uniformBlockMap;
		LinaMap<LinaString, int32> samplerMap;
		LinaMap<LinaString, int32> uniformMap;
	};

	// Frame buffer object struct for storage.
	struct FBOData
	{
		int32 width;
		int32 height;
	};


	class GLRenderDevice
	{
	public:

		GLRenderDevice();
		~GLRenderDevice();

		// Creates an GLFW window.
		FORCEINLINE bool CreateContextWindow()
		{
			return m_MainWindow->Initialize();
		}

		// Returns GLFW window instance.
		FORCEINLINE void* GetNativeWindow()
		{
			return m_MainWindow->GetNativeWindow();
		}

		// Set event callback for window.
		FORCEINLINE void SetMainWindowEventCallback(const std::function<void(Event&)>& callback)
		{
			m_MainWindow->SetEventCallback(callback);
		}

		// Swaps window buffer.
		FORCEINLINE void TickWindow()
		{
			m_MainWindow->Tick();
		}

		// Returns the window width & height
		FORCEINLINE Vector2F GetWindowSize()
		{
			return Vector2F(m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
		};

		// Returns the window center coordinates.
		FORCEINLINE Vector2F GetWindowCenter()
		{
			return Vector2F(m_MainWindow->GetWidth() / 2.0f, m_MainWindow->GetHeight() / 2.0f);
		}

		// Initializes the devices & params.
		void Initialize(LinaEngine::ECS::LightingSystem& lightingSystemIn);

		// Creates a texture on GL.
		uint32 CreateTexture2D(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress);

		// Creates a DDS texture on GL.
		uint32 CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount);

		// Creates a Cubemap texture on GL.
		uint32 CreateCubemapTexture(int32 width, int32 height, const LinaArray<int32*>& data, uint32 dataSize = 6);

		// Releases a previously created texture from GL.
		uint32 ReleaseTexture2D(uint32 texture2D);

		// Creates a vertex array on GL.
		uint32 CreateVertexArray(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage);

		// Creates a skybox vertex array.
		uint32 CreateSkyboxVertexArray();

		// Releases a previously created vertex array by id from GL.
		uint32 ReleaseVertexArray(uint32 vao);

		// Creates a texture sampler on GL.
		uint32 CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter, SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy);

		// Releases a previously created texture sampler from GL.
		uint32 ReleaseSampler(uint32 sampler);

		// Creates a uniform buffer on GL.
		uint32 CreateUniformBuffer(const void* data, uintptr dataSize, BufferUsage usage);

		// Releases a previously created buffer from GL.
		uint32 ReleaseUniformBuffer(uint32 buffer);

		// Creates a shader program based on shader text on GL.
		uint32 CreateShaderProgram(const LinaString& shaderText);

		// Releases a previously created shader program from GL.
		uint32 ReleaseShaderProgram(uint32 shader);

		// Creates a render target based on window props & attachments on GL.
		uint32 CreateRenderTarget(uint32 texture, int32 width, int32 height, FramebufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel);

		// Releases a previously created render target from GL.
		uint32 ReleaseRenderTarget(uint32 target);

		// Updates a vertex array object by id.
		void UpdateVertexArray(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);

		// Uses a shader by id.
		void SetShader(uint32 shader);

		// Uses a shader sampler by id.
		void SetShaderSampler(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit, BindTextureMode bindTextureMode = BindTextureMode::BINDTEXTURE_TEXTURE2D);

		// Creates uniform buffer of a shader by id.
		void SetShaderUniformBuffer(uint32 shader, const LinaString& uniformBufferName, uint32 buffer);

		// Binds a buffer object to a binding point on GL buffer, then binds the program uniform block to that points.
		void BindUniformBuffer(uint32 buffer, uint32 bindingPoint);

		// Binds a shader to unifor block binding point.
		void BindShaderBlockToBufferPoint(uint32 shader, uint32 blockPoint, LinaString& blockName);

		// Updates a vertex array buffer by id.
		void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);

		// Updates a uniform buffer for a shader by id.
		void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr offset, uintptr dataSize);

		// Actual drawing process for meshes.
		void Draw(uint32 fbo, uint32 shader, uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements);

		// Used for drawing a skybox.
		void DrawSkybox(uint32 fbo, uint32 shader, uint32 vao, uint32 texture, const DrawParams& drawParams);
		void DrawSkybox(uint32 fbo, uint32 shader, uint32 vao, uint32 texture, const DrawParams& drawParams, const Color& color);
		void DrawSkybox(uint32 fbo, uint32 shader, uint32 vao, uint32 texture, const DrawParams& drawParams, const Color& colorStart, const Color& colorEnd);
		void DrawSkybox(uint32 fbo, uint32 shader, uint32 vao, uint32 texture, const DrawParams& drawParams, const Color& colorStart, const Color& colorEnd, const Vector3F& sunvector);

		// Clears context.
		void Clear(uint32 fbo, bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const class Color& color, uint32 stencil);

		// Called when main window is resized.
		void OnWindowResized(float width, float height);



		// Updates a mat4 type uniform on a shader with given name.
		void UpdateShaderUniformMatrix(uint32 shader, const LinaString& uniform, const Matrix& m);

		// Updates a mat4 type uniform on a shader with given name.
		void UpdateShaderUniformMatrix(uint32 shader, const LinaString& uniform, void* data);

		// Updates a vec3 type uniform on a shader with given name.
		void UpdateShaderUniformVector3F(uint32 shader, const LinaString& uniform, const Vector3F& m);

		// Updates a vec3 type uniform on a shader with given name.
		void UpdateShaderUniformVector3F(uint32 shader, const LinaString& uniform, void* data);

		// Updates a vec3 type uniform on a shader with given name.
		void UpdateShaderUniformColor(uint32 shader, const LinaString& uniform, const Color& color);

		// Updates a float type uniform on a shader w/ given name.
		void UpdateShaderUniformFloat(uint32 shader, const LinaString& uniform, const float f);

		// Updates an integer type uniform on a shader w/ given name.
		void UpdateShaderUniformInt(uint32 shader, const LinaString& uniform, const int f);
	private:

		LinaString GetShaderVersion();
		uint32 GetVersion();
		void SetVAO(uint32 vao);
		void SetFBO(uint32 fbo);
		void SetViewport(uint32 fbo);
		void SetFaceCulling(FaceCulling faceCulling);
		void SetDepthTest(bool shouldWrite, DrawFunc depthFunc);
		void SetBlending(BlendFunc sourceBlend, BlendFunc destBlend);
		void SetStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass);
		void SetStencilWriteMask(uint32 mask);
		void SetScissorTest(bool enable, uint32 startX = 0, uint32 startY = 0, uint32 width = 0, uint32 height = 0);


	private:

		// Main window reference.
		std::unique_ptr<GLWindow> m_MainWindow;

		// Reference to the ECS Lighting system in Render Engine
		LinaEngine::ECS::LightingSystem* m_LightingSystem;

		// Currently active shader.
		uint32 m_BoundShader = 0;

		// Currently active vertex array object.
		uint32 m_BoundVAO;

		// Currently active frame buffer object.
		uint32 m_BoundFBO;

		// FBO rep. on viewport.
		uint32 m_ViewportFBO;

		// Map for bound vertex array objects.
		LinaMap<uint32, VertexArrayData> m_VAOMap;

		// Frame buffer object map w/ ids.
		LinaMap<uint32, FBOData> m_FBOMap;

		// Shader program map w/ ids.
		LinaMap<uint32, ShaderProgram> m_ShaderProgramMap;

		// Storage for shader version.
		LinaString m_ShaderVersion;

		// Storage for gl version data.
		uint32 m_GLVersion;

		// Current drawing parameters.
		FaceCulling m_UsedFaceCulling;
		DrawFunc m_UsedDepthFunction;
		BlendFunc m_UsedSourceBlending;
		BlendFunc m_UsedDestinationBlending;
		DrawFunc m_UsedStencilFunction;
		StencilOp m_usedStencilFail;
		StencilOp m_UsedStencilPassButDepthFail;
		StencilOp m_UsedStencilPass;

		// Current operation parameters.
		uint32 m_UsedStencilTestMask;
		uint32 m_UsedStencilWriteMask;
		int32 m_UsedStencilComparisonValue;
		bool m_IsBlendingEnabled;
		bool m_IsStencilTestEnabled;
		bool m_IsScissorsTestEnabled;
		bool m_ShouldWriteDepth;

	};
}


#endif