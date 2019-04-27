/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderEngine
Timestamp: 4/15/2019 12:26:31 PM

*/

#pragma once

#ifndef RenderEngine_HPP
#define RenderEngine_HPP

#include "Events/Event.hpp"
#include "Core/DataStructures.hpp"

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



	template<class Derived>
	class RenderEngine
	{
	public:

		virtual ~RenderEngine()
		{
			LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
		}

		// Get a void* reference to the native window. e.g GLFWwindow
		FORCEINLINE void* GetNativeWindow() { return m_Derived->GetNativeWindow_Impl(); }

		// Initialize the render renderEngine.
		FORCEINLINE void Initialize() { m_Derived->Initialize_Impl(); }

		// Called each frame.
		FORCEINLINE void Tick() { m_Derived->Tick_Impl(); }

		// Create a render context.
		FORCEINLINE bool CreateContextWindow() { return m_Derived->CreateContextWindow_Impl(); }

		// Set the target of the callbacks coming from the main window context.
		FORCEINLINE void SetMainWindowEventCallback(const std::function<void(Event&)>& callback) { m_Derived->SetMainWindowEventCallback_Impl(callback); }

		// Creates a 2D texture based on parameters. Data is sent as an arraybitmap data.
		FORCEINLINE uint32 CreateTexture2D(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
		{
			return m_Derived->CreateTexture2D_Impl(width, height, data, pixelDataFormat, internalPixelFormat, generateMipMaps, compress);
		}

		// Creates a 2D DDS texture based on parameters. Buffer is sent as an arraybitmap data.
		FORCEINLINE uint32 CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount)
		{
			return m_Derived->CreateDDSTexture2D_Impl(width, height, buffer, fourCC, mipMapCount);
		}

		// Releases the texture from memory.
		FORCEINLINE uint32 ReleaseTexture2D(uint32 texture2D)
		{
			m_Derived->ReleaseTexture2D_Impl(texture2D);
		}

		// Creates a vertex array based on vertex data, usually by indexed models.
		FORCEINLINE uint32 CreateVertexArray(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage)
		{
			return m_Derived->CreateVertexArray_Impl(vertexData, vertexElementSizes, numVertexComponents, numInstanceComponents, numVertices, indices, numIndices, bufferUsage);
		}

		// Releases a vertex array from mem
		FORCEINLINE uint32 ReleaseVertexArray(uint32 vao)
		{
			return m_Derived->ReleaseVertexArray_Impl(vao);
		}

		// Creates a texture sampler
		FORCEINLINE uint32 CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter, SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy)
		{
			return m_Derived->CreateSampler_Impl(minFilter, magFilter, wrapU, wrapV, anisotropy);
		}

		// Releases the target texture sampler
		FORCEINLINE uint32 ReleaseSampler(uint32 sampler)
		{
			return m_Derived->ReleaseSampler_Impl(sampler);
		}

		// Creates a uniform buffer based on buff. data.
		FORCEINLINE uint32 CreateUniformBuffer(const void* data, uintptr dataSize, enum BufferUsage usage)
		{
			return m_Derived->CreateUniformBuffer_Impl(data, dataSize, usage);
		}

		// Releases the target uniform buffer.
		FORCEINLINE uint32 ReleaseUniformBuffer(uint32 buffer)
		{
			return m_Derived->ReleaseUniformBuffer_Impl(buffer);
		}

		// Creates a shader program based on shader text.
		FORCEINLINE uint32 CreateShaderProgram(const LinaString& shaderText)
		{
			return m_Derived->CreateShaderProgram_Impl(shaderText);
		}

		// Releases the target shader program.
		FORCEINLINE uint32 ReleaseShaderProgram(uint32 shader)
		{
			return m_Derived->ReleaseShaderProgram_Impl(shader);
		}

		FORCEINLINE uint32 CreateRenderTarget(uint32 texture, int32 width, int32 height, FramebufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel)
		{
			return m_Derived->CreateRenderTarget_Impl(texture, width, height, attachment, attachmentNumber, mipLevel);
		}

		FORCEINLINE uint32 ReleaseRenderTarget(uint32 fbo)
		{
			return m_Derived->ReleaseRenderTarget_Impl(fbo);
		}

		// Sets the active shader.
		FORCEINLINE void SetShader(uint32 shader)
		{
			m_Derived->SetShader_Impl(shader);
		}

		// Sets the texture sampler for the target shader.
		FORCEINLINE void SetShaderSampler(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit)
		{
			m_Derived->SetShaderSampler_Impl(shader, samplerName, texture, sampler, unit);
		}

		FORCEINLINE void SetShaderUniformBuffer(uint32 shader, const LinaString& uniformBufferName, uint32 buffer)
		{
			m_Derived->SetShaderUniformBuffer_Impl(shader, uniformBufferName, buffer);
		}

		// Updates the target VAO and makes ready for the next draw.
		FORCEINLINE void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
		{
			m_Derived->UpdateVertexArrayBuffer_Impl(vao, bufferIndex, data, dataSize);
		}

		// Updates the target uniform buffer & makes it ready for next draw params.
		FORCEINLINE void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize)
		{
			m_Derived->UpdateUniformBuffer_Impl(buffer, data, dataSize);
		}

		// Draw instances.
		FORCEINLINE void Draw(uint32 fbo, uint32 shader, uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements)
		{
			m_Derived->Draw_Impl(fbo, shader, vao, drawParams, numInstances, numElements);
		}

		// Clear colors.
		FORCEINLINE void Clear(uint32 fbo, bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const class Color& color, uint32 stencil)
		{
			m_Derived->Clear_Impl(fbo, shouldClearColor, shouldClearDepth, shouldClearStencil, color, stencil);
		}

	protected:

		FORCEINLINE RenderEngine()
		{
			LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());
			m_Derived = static_cast<Derived*>(this);
		}

	private:

		// Derived class reference for static polymorphism.
		Derived* m_Derived;
	};

}


#endif