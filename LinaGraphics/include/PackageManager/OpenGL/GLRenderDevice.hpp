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
Class: GLRenderDevice

Responsible for handling all Open GL related drawing functionalities. All GL commands are only stored and used
within an instance of this class.

Timestamp: 4/27/2019 10:12:16 PM
*/

#pragma once

#ifndef GLRenderDevice_HPP
#define GLRenderDevice_HPP


#include "Math/Matrix.hpp"
#include "Math/Color.hpp"
#include "Rendering/RenderingCommon.hpp"
#include <map>

using namespace Lina;

namespace Lina::Graphics
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
		std::vector<uint32> shaders;
		std::map<std::string, int32> uniformBlockMap;
		std::map<std::string, int32> samplerMap;
		std::map<std::string, int32> uniformMap;
	};


	struct BufferData
	{
		BufferData() {};
		BufferData(uint32 size, uint32 attrib, bool isFloat, bool isInstanced) : m_isFloat(isFloat), m_attrib(attrib), m_elementSize(size), m_isInstanced(isInstanced) {};

		uint32 m_attrib;
		uint32 m_elementSize;
		bool m_isFloat;
		bool m_isInstanced;
		std::vector<float> m_floatElements;
		std::vector<int> m_intElements;
	};

	class GLRenderDevice
	{
	public:

		GLRenderDevice();
		~GLRenderDevice();

		void Initialize(int width, int height, DrawParams& defaultParams);
		uint32 CreateTexture2D(Vector2 size, const void* data,  SamplerParameters samplerParams ,bool compress, bool useBorder = false, Color borderColor = Color::White);
		uint32 CreateTextureHDRI(Vector2 size, float* data, SamplerParameters samplerParams);
		uint32 CreateCubemapTexture(Vector2 size, SamplerParameters samplerParams, const std::vector<unsigned char*>& data, uint32 dataSize = 6);
		uint32 CreateCubemapTextureEmpty(Vector2 size, SamplerParameters samplerParams);
		uint32 CreateTexture2DMSAA(Vector2 size, SamplerParameters samplerParams, int sampleCount);
		uint32 CreateTexture2DEmpty(Vector2 size, SamplerParameters samplerParams);
		
		void SetupTextureParameters(uint32 textureTarget, SamplerParameters samplerParams, bool useBorder = false, float* borderColor = NULL);
		void UpdateTextureParameters(uint32 bindMode, uint32 id, SamplerParameters samplerParmas);
	
		uint32 ReleaseTexture2D(uint32 texture2D);
		uint32 CreateVertexArray(const std::vector<BufferData>& bufferData, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage);
		uint32 CreateSkyboxVertexArray();
		uint32 CreateScreenQuadVertexArray();
		uint32 CreateLineVertexArray();
		uint32 CreateHDRICubeVertexArray();
		uint32 ReleaseVertexArray(uint32 vao, bool checkMap = true);
		uint32 CreateSampler(SamplerParameters samplerParams, bool isCubemap = false);
		uint32 ReleaseSampler(uint32 sampler);
		uint32 CreateUniformBuffer(const void* data, uintptr dataSize, BufferUsage usage);
		uint32 ReleaseUniformBuffer(uint32 buffer);
		uint32 CreateShaderProgram(const std::string& shaderText, ShaderUniformData* data, bool usesGeometryShader);
		bool ValidateShaderProgram(uint32 shader);
	
		uint32 ReleaseShaderProgram(uint32 shader);
		uint32 CreateRenderTarget(uint32 texture, int32 width, int32 height, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 mipLevel, bool noReadWrite, bool bindRBO = false, FrameBufferAttachment rboAtt = FrameBufferAttachment::ATTACHMENT_DEPTH_AND_STENCIL, uint32 rbo = 0, bool errorCheck = true);
		
		void BindTextureToRenderTarget(uint32 fbo, uint32 texture,TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 textureAttachmentNumber = 0, int mipLevel = 0, bool bindTexture = true, bool setDefaultFBO = true);
		void MultipleDrawBuffersCommand(uint32 fbo, uint32 bufferCount, uint32* attachments);
		void ResizeRTTexture(uint32 texture, Vector2 newSize, PixelFormat m_internalPixelFormat, PixelFormat m_pixelFormat, TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D, bool compress = false);
		void ResizeRenderBuffer(uint32 fbo, uint32 rbo, Vector2 newSize, RenderBufferStorage storage);
		
		uint32 ReleaseRenderTarget(uint32 target);
		uint32 CreateRenderBufferObject(RenderBufferStorage storage, uint32 width, uint32 height, int sampleCount);
		uint32 ReleaseRenderBufferObject(uint32 target);
		
		void UpdateSamplerParameters(uint32 sampler, SamplerParameters params);
		void GenerateTextureMipmaps(uint32 texture, TextureBindMode bindMode);
		void BlitFrameBuffers(uint32 readFBO, uint32 readWidth, uint32 readHeight, uint32 writeFBO, uint32 writeWidth, uint32 writeHeight, BufferBit mask, SamplerFilter filter, FrameBufferAttachment att, uint32 attCount);
		bool IsRenderTargetComplete(uint32 fbo);
		void UpdateVertexArray(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);
		void SetShader(uint32 shader);
		void SetTexture(uint32 texture, uint32 sampler, uint32 unit, TextureBindMode bindTextureMode = TextureBindMode::BINDTEXTURE_TEXTURE2D, bool setSampler = false);
		void SetShaderUniformBuffer(uint32 shader, const std::string& uniformBufferName, uint32 buffer);
		void BindUniformBuffer(uint32 buffer, uint32 bindingPoint);
		void BindShaderBlockToBufferPoint(uint32 shader, uint32 blockPoint, std::string& blockName);
		void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);
		void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr offset, uintptr dataSize);
		void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize);
		ShaderUniformData ScanShaderUniforms(uint32 shader);

		void SetDrawParameters(const DrawParams& drawParams);
		void Draw(uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements, bool drawArrays = false);
		void DrawLine(float width);
		void DrawLine(uint32 shader, const Matrix& model, const Vector3& from, const Vector3& to, float width = 1.0f);
		void Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const class Color& color, uint32 stencil);

		void UpdateShaderUniformFloat(uint32 shader, const std::string& uniform, const float f);
		void UpdateShaderUniformInt(uint32 shader, const std::string& uniform, const int f);
		void UpdateShaderUniformColor(uint32 shader, const std::string& uniform, const Color& color);
		void UpdateShaderUniformVector2(uint32 shader, const std::string& uniform, const Vector2& m);
		void UpdateShaderUniformVector3(uint32 shader, const std::string& uniform, const Vector3& m);
		void UpdateShaderUniformVector4F(uint32 shader, const std::string& uniform, const Vector4& m);
		void UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, const Matrix& m);
		void UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, void* data);

		void SetStencilWriteMask(uint32 mask);
		void SetDepthTestEnable(bool enable);
		void SetFBO(uint32 fbo);
		void SetVAO(uint32 vao);
		void CaptureHDRILightingData(Matrix& view, Matrix& projection, Vector2 captureSize, uint32 cubeMapTexture, uint32 hdrTexture, uint32 fbo, uint32 rbo, uint32 shader);
		void SetViewport(Vector2 pos, Vector2 size);


	private:

		std::string GetShaderVersion();
		uint32 GetVersion();
	
		void SetRBO(uint32 rbo);
		void SetFaceCulling(FaceCulling faceCulling);
		void SetDepthTest(bool shouldWrite, DrawFunc depthFunc);
		void SetBlending(BlendFunc sourceBlend, BlendFunc destBlend);
		void SetStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass);
		void SetScissorTest(bool enable, uint32 startX = 0, uint32 startY = 0, uint32 width = 0, uint32 height = 0);


	private:

		uint32 m_boundShader = 0;
		uint32 m_boundVAO = 0;
		uint32 m_boundFBO = 0;
		uint32 m_boundReadFBO = 0;
		uint32 m_boundWriteFBO = 0;
		uint32 m_viewportFBO = 0;
		uint32 m_boundRBO = 0;
		uint32 m_boundUBO;
		uint32 m_boundTextureUnit;
		Vector2 m_boundViewportSize;
		Vector2 m_boundViewportPos;
		std::map<uint32, VertexArrayData> m_vaoMap;
		std::map<uint32, ShaderProgram> m_shaderProgramMap;
		std::string m_ShaderVersion;
		uint32 m_GLVersion;

		// Current drawing parameters.
		FaceCulling m_usedFaceCulling;
		DrawFunc m_usedDepthFunction;
		BlendFunc m_usedSourceBlending;
		BlendFunc m_usedDestinationBlending;
		DrawFunc m_usedStencilFunction;
		StencilOp m_usedStencilFail;
		StencilOp m_usedStencilPassButDepthFail;
		StencilOp m_usedStencilPass;

		// Current operation parameters.
		uint32 m_usedStencilTestMask = 0;
		uint32 m_usedStencilWriteMask = 0;
		int32 m_usedStencilComparisonValue = 0;
		bool m_isBlendingEnabled = true;
		bool m_isStencilTestEnabled = true;
		bool m_isScissorsTestEnabled = false;
		bool m_shouldWriteDepth = true;
		bool m_isDepthTestEnabled = true;
		Color m_currentClearColor = Color::Black;
		
	};
}


#endif