/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: GLRenderEngine
Timestamp: 4/15/2019 12:37:37 PM

*/

#pragma once

#ifndef GLRenderEngine_HPP
#define GLRenderEngine_HPP


#include "Rendering/RenderEngine.hpp"
#include "PackageManager/PAMWindow.hpp"
#include "Core/DataStructures.hpp"

namespace LinaEngine::Graphics
{

#define MAKEFOURCC(a, b, c, d)                              \
                ((uint32)(uint8)(a) | ((uint32)(uint8)(b) << 8) |       \
				((uint32)(uint8)(c) << 16) | ((uint32)(uint8)(d) << 24 ))

#define MAKEFOURCCDXT(a) MAKEFOURCC('D', 'X', 'T', a)
#define FOURCC_DXT1 MAKEFOURCCDXT('1')
#define FOURCC_DXT2 MAKEFOURCCDXT('2')
#define FOURCC_DXT3 MAKEFOURCCDXT('3')
#define FOURCC_DXT4 MAKEFOURCCDXT('4')
#define FOURCC_DXT5 MAKEFOURCCDXT('5')

	struct VertexArray
	{
		uint32* buffers;
		uintptr* bufferSizes;
		uint32  numBuffers;
		uint32  numElements;
		uint32  instanceComponentsStartIndex;
		BufferUsage bufferUsage;
	};

	struct ShaderProgram
	{
		LinaArray<uint32>      shaders;
		LinaMap<LinaString, int32> uniformMap;
		LinaMap<LinaString, int32> samplerMap;
	};

	struct FBOData
	{
		int32 width;
		int32 height;
	};


	// A derived class of RenderEngine. Check RenderEngine.hpp for method details.
	class GLRenderEngine : public RenderEngine<GLRenderEngine>
	{
	public:

		GLRenderEngine();
		virtual ~GLRenderEngine();

		void Initialize_Impl() STATIC_OVERRIDE;
		void Tick_Impl() STATIC_OVERRIDE;

		FORCEINLINE bool CreateContextWindow_Impl() STATIC_OVERRIDE 
		{ 
			return m_MainWindow->Initialize(); 
		}
		FORCEINLINE void* GetNativeWindow_Impl() STATIC_OVERRIDE 
		{ 
			return m_MainWindow->GetNativeWindow();
		}

		FORCEINLINE void SetMainWindowEventCallback_Impl(const std::function<void(Event&)>& callback) STATIC_OVERRIDE 
		{ 
			m_MainWindow->SetEventCallback(callback);
		}

		uint32 CreateTexture2D_Impl(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress) STATIC_OVERRIDE;
		uint32 CreateDDSTexture2D_Impl(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount) STATIC_OVERRIDE;
		uint32 ReleaseTexture2D_Impl(uint32 texture2D) STATIC_OVERRIDE;
		uint32 CreateVertexArray_Impl(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage) STATIC_OVERRIDE;
		uint32 ReleaseVertexArray_Impl(uint32 vao);
		uint32 CreateSampler_Impl(SamplerFilter minFilter, SamplerFilter magFilter, SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy);
		uint32 ReleaseSampler_Impl(uint32 sampler);
		uint32 CreateUniformBuffer_Impl(const void* data, uintptr dataSize, enum BufferUsage usage);
		uint32 ReleaseUniformBuffer_Impl(uint32 buffer);
		uint32 CreateShaderProgram_Impl(const LinaString& shaderText);
		uint32 ReleaseShaderProgram_Impl(uint32 shader);
		uint32 CreateRenderTarget_Impl(uint32 texture, int32 width, int32 height, FramebufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel);
		uint32 ReleaseRenderTarget_Impl(uint32 target);
		void UpdateVertexArray_Impl(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);
		void SetShader_Impl(uint32 shader) STATIC_OVERRIDE;
		void SetShaderSampler_Impl(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit) STATIC_OVERRIDE;
		void SetShaderUniformBuffer_Impl(uint32 shader, const LinaString& uniformBufferName, uint32 buffer);
		void UpdateVertexArrayBuffer_Impl(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);
		void UpdateUniformBuffer_Impl(uint32 buffer, const void* data, uintptr dataSize);
		void Draw_Impl(uint32 fbo, uint32 shader, uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements);
		void Clear_Impl(uint32 fbo, bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const Color& color, uint32 stencil);


	private:

		void setVAO(uint32 vao);
		LinaString getShaderVersion();
		uint32 getVersion();
		void setFBO(uint32 fbo);
		void setViewport(uint32 fbo);
		void setFaceCulling(FaceCulling faceCulling);
		void setDepthTest(bool shouldWrite, DrawFunc depthFunc);
		void setBlending(BlendFunc sourceBlend, BlendFunc destBlend);
		void setStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass);
		void setStencilWriteMask(uint32 mask);
		void setScissorTest(bool enable, uint32 startX = 0, uint32 startY = 0, uint32 width = 0, uint32 height = 0);

	private:

		// Main window reference.
		std::unique_ptr<Window<PAMWindow>> m_MainWindow;

		// Currently active shader.
		uint32 m_BoundShader = 0;

		// Currently active vertex array object.
		uint32 m_BoundVAO;

		// Currently active frame buffer object.
		uint32 boundFBO;

		// FBO rep. on viewport.
		uint32 viewportFBO;

		// Map for bound vertex array objects.
		LinaMap<uint32, VertexArray> vaoMap;

		// Shader program map w/ ids.
		LinaMap<uint32, ShaderProgram> shaderProgramMap;

		// Frame buffer object map w/ ids.
		LinaMap<uint32, FBOData> fboMap;

		// Storage for shader version.
		LinaString shaderVersion;

		// Storage for gl version data.
		uint32 version;

		FaceCulling currentFaceCulling;
		DrawFunc currentDepthFunc;
		BlendFunc currentSourceBlend;
		BlendFunc currentDestBlend;
		DrawFunc currentStencilFunc;
		StencilOp currentStencilFail;
		StencilOp currentStencilPassButDepthFail;
		StencilOp currentStencilPass;
		bool blendingEnabled;
		bool shouldWriteDepth;
		bool stencilTestEnabled;
		bool scissorTestEnabled;
		uint32 currentStencilTestMask;
		uint32 currentStencilWriteMask;
		int32 currentStencilComparisonVal;


	};
}



#endif