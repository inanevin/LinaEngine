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

	enum PixelFormat
	{
		FORMAT_R,
		FORMAT_RG,
		FORMAT_RGB,
		FORMAT_RGBA,
		FORMAT_DEPTH,
		FORMAT_DEPTH_AND_STENCIL,
	};


	struct ShaderProgram
	{
		LinaArray<uint32> m_Shaders;
		LinaMap<LinaString, uint32> m_UniformMap;
		LinaMap<LinaString, uint32> m_SamplerMap;
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

		// Initialize the render engine.
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
			return m_Derived->CreateDDSTexture2D_Impl(width, height,  buffer, fourCC, mipMapCount);
		}

		// Releases the texture from memory.
		FORCEINLINE uint32 ReleaseTexture2D(uint32 texture2D)
		{
			m_Derived->ReleaseTexture2D_Impl(texture2D);
		}

		// Creates a vertex array based on vertex data, usually by indexed models.
		FORCEINLINE uint32 CreateVertexArray(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, int bufferUsage)
		{
			return m_Derived->CreateVertexArray_Impl(vertexData, vertexElementSizes, numVertexComponents, numInstanceComponents, numVertices, indices, numIndices, bufferUsage);
		}

		// Releases a vertex array from mem
		FORCEINLINE uint32 ReleaseVertexArray(uint32 vao)
		{
			return m_Derived->ReleaseVertexArray_Impl(vao);
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

		// Updates the target VAO and makes ready for the next draw.
		FORCEINLINE void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize)
		{
			m_Derived->UpdateVertexArrayBuffer_Impl(vao, bufferIndex, data, dataSize);
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