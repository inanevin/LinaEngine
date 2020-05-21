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


#include "Utility/Math/Matrix.hpp"
#include "Utility/Math/Color.hpp"
#include "Core/LinaArray.hpp"
#include "Rendering/RenderingCommon.hpp"
#include <map>

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
		std::map<std::string, int32> uniformBlockMap;
		std::map<std::string, int32> samplerMap;
		std::map<std::string, int32> uniformMap;
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

		// Initializes the devices & params.
		void Initialize(LinaEngine::ECS::LightingSystem& lightingSystemIn, int width, int height);

		// Creates a texture on GL.
		uint32 CreateTexture2D(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress
		, SamplerFilter minFilter = SamplerFilter::FILTER_NEAREST, SamplerFilter magFilter = SamplerFilter::FILTER_NEAREST, SamplerWrapMode wrapS = SamplerWrapMode::WRAP_REPEAT,
			SamplerWrapMode wrapT = SamplerWrapMode::WRAP_REPEAT, int sampleCount = 0, bool emptyTexture = false);

		// Creates a DDS texture on GL.
		uint32 CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount);

		// Creates a Cubemap texture on GL.
		uint32 CreateCubemapTexture(int32 width, int32 height, const LinaArray<int32*>& data, uint32 dataSize = 6, PixelFormat pixelDataFormat = PixelFormat::FORMAT_RGB, PixelFormat internalPixelFormat = PixelFormat::FORMAT_RGB, bool generateMipMaps = true);

		// Releases a previously created texture from GL.
		uint32 ReleaseTexture2D(uint32 texture2D);

		// Creates a vertex array on GL for mesh & model data.
		uint32 CreateVertexArray(const float** vertexData, const uint32* vertexElementSizes, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage);

		// Creates a skybox vertex array.
		uint32 CreateSkyboxVertexArray();

		// Creates a vertex array for a screen quad
		uint32 CreateScreenQuadVertexArray();

		// Releases a previously created vertex array by id from GL.
		uint32 ReleaseVertexArray(uint32 vao, bool checkMap = true);

		// Creates a texture sampler on GL.
		uint32 CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter, SamplerWrapMode wrapU, SamplerWrapMode wrapV, float anisotropy);

		// Releases a previously created texture sampler from GL.
		uint32 ReleaseSampler(uint32 sampler);

		// Creates a uniform buffer on GL.
		uint32 CreateUniformBuffer(const void* data, uintptr dataSize, BufferUsage usage);

		// Releases a previously created buffer from GL.
		uint32 ReleaseUniformBuffer(uint32 buffer);

		// Creates a shader program based on shader text on GL.
		uint32 CreateShaderProgram(const std::string& shaderText, bool usesGeometryShader);

		// Releases a previously created shader program from GL.
		uint32 ReleaseShaderProgram(uint32 shader);

		// Creates a render target based on window props & attachments on GL.
		uint32 CreateRenderTarget(uint32 texture, int32 width, int32 height, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel, bool noReadWrite, bool bindRBO = false, FrameBufferAttachment rboAtt = FrameBufferAttachment::ATTACHMENT_DEPTH_AND_STENCIL, uint32 rbo = 0);

		// Releases a previously created render target from GL.
		uint32 ReleaseRenderTarget(uint32 target);

		// Creates a render buffer object
		uint32 CreateRenderBufferObject(RenderBufferStorage storage, uint32 width, uint32 height, int sampleCount);

		// Deletes a render buffer object.
		uint32 ReleaseRenderBufferObject(uint32 target);

		// Binds a rbo to an existing fbo
		void BlitFrameBuffers(uint32 readFBO, uint32 readWidth, uint32 readHeight, uint32 writeFBO, uint32 writeWidth, uint32 writeHeight, BufferBit mask, SamplerFilter filter);

		// Updates a vertex array object by id.
		void UpdateVertexArray(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);

		// Uses a shader by id.
		void SetShader(uint32 shader);

		// Uses a shader sampler by id.
		void SetTexture(uint32 texture, uint32 sampler, uint32 unit, TextureBindMode bindTextureMode = TextureBindMode::BINDTEXTURE_TEXTURE2D, bool setSampler = false);

		// Creates uniform buffer of a shader by id.
		void SetShaderUniformBuffer(uint32 shader, const std::string& uniformBufferName, uint32 buffer);

		// Binds a buffer object to a binding point on GL buffer, then binds the program uniform block to that points.
		void BindUniformBuffer(uint32 buffer, uint32 bindingPoint);

		// Binds a shader to unifor block binding point.
		void BindShaderBlockToBufferPoint(uint32 shader, uint32 blockPoint, std::string& blockName);

		// Updates a vertex array buffer by id.
		void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);

		// Updates a uniform buffer for a shader by id with offset.
		void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr offset, uintptr dataSize);

		// Updates a uniform buffer for a shader by id without offset.
		void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize);

		// Actual drawing process for meshes.
		void Draw(uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements, bool drawArrays = false);

		// Clears context.
		void Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const class Color& color, uint32 stencil);

		// Called when main window is resized.
		void OnWindowResized(float width, float height);

		// Updates a float type uniform on a shader w/ given name.
		void UpdateShaderUniformFloat(uint32 shader, const std::string& uniform, const float f);

		// Updates an integer type uniform on a shader w/ given name.
		void UpdateShaderUniformInt(uint32 shader, const std::string& uniform, const int f);

		// Updates a vec3 type uniform on a shader with given name.
		void UpdateShaderUniformColor(uint32 shader, const std::string& uniform, const Color& color);

		// Updates a vec2 type uniform on a shader with given name.
		void UpdateShaderUniformVector2(uint32 shader, const std::string& uniform, const Vector2& m);

		// Updates a vec3 type uniform on a shader with given name.
		void UpdateShaderUniformVector3(uint32 shader, const std::string& uniform, const Vector3& m);

		// Updates a vec4 type uniform on a shader with given name.
		void UpdateShaderUniformVector4F(uint32 shader, const std::string& uniform, const Vector4& m);

		// Updates a mat4 type uniform on a shader with given name.
		void UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, const Matrix& m);

		// Updates a mat4 type uniform on a shader with given name.
		void UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, void* data);

		// Sets stencil mask to specific value
		void SetStencilWriteMask(uint32 mask);

		// Enable/disable depth testing.
		void SetDepthTestEnable(bool enable);

		// Set frame buffer object.
		void SetFBO(uint32 fbo);

		// Set vertex array object.
		void SetVAO(uint32 vao);

	private:

		std::string GetShaderVersion();
		uint32 GetVersion();
	
		void SetRBO(uint32 rbo);
		void SetViewport(uint32 fbo);
		void SetFaceCulling(FaceCulling faceCulling);
		void SetDepthTest(bool shouldWrite, DrawFunc depthFunc);
		void SetBlending(BlendFunc sourceBlend, BlendFunc destBlend);
		void SetStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass);
		void SetScissorTest(bool enable, uint32 startX = 0, uint32 startY = 0, uint32 width = 0, uint32 height = 0);


	private:

		// Reference to the ECS Lighting system in Render Engine
		LinaEngine::ECS::LightingSystem* m_LightingSystem;

		// Currently active shader.
		uint32 m_BoundShader = 0;

		// Currently active vertex array object.
		uint32 m_BoundVAO = 0;

		// Currently active frame buffer object.
		uint32 m_BoundFBO = 0;

		// FBO rep. on viewport.
		uint32 m_ViewportFBO = 0;

		// Currently bound render buffer object
		uint32 m_BoundRBO = 0;

		// Currently bound uniform buffer
		uint32 m_BoundUBO;

		// Currently bound texture unit
		uint32 m_BoundTextureUnit;

		// Map for bound vertex array objects.
		std::map<uint32, VertexArrayData> m_VAOMap;

		// Frame buffer object map w/ ids.
		std::map<uint32, FBOData> m_FBOMap;

		// Shader program map w/ ids.
		std::map<uint32, ShaderProgram> m_ShaderProgramMap;

		// Bound texture map
		std::map<uint32, uint32> m_BoundTextures;

		// Bound sampler map.
		std::map<uint32, uint32> m_BoundSamplers;

		// Storage for shader version.
		std::string m_ShaderVersion;

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
		bool m_IsDepthTestEnabled;
		Color m_CurrentClearColor;
		
	};
}


#endif