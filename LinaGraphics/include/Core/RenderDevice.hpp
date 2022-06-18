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

/*
Class: RenderDevice

Responsible for handling all Open GL related drawing functionalities. All GL commands are only stored and used
within an instance of this class.

Timestamp: 4/27/2019 10:12:16 PM
*/

#pragma once

#ifndef GLRenderDevice_HPP
#define GLRenderDevice_HPP

#include "Math/Color.hpp"
#include "Math/Matrix.hpp"
#include "Rendering/RenderingCommon.hpp"

#include <map>

using namespace Lina;

namespace Lina::Graphics
{

    class RenderDevice
    {
    public:
        RenderDevice();
        ~RenderDevice();

        static RenderDevice* Get()
        {
            return s_renderDevice;
        }

        /// <summary>
        /// Inits open gl with current draw parameters.
        /// </summary>
        void Initialize(int width, int height, DrawParams& defaultParams, const WindowProperties& windowProps);

        /// <summary>
        /// Creates 2D texture in GL, use CreateTexture2DEmpty for framebuffer textures.
        /// </summary>
        uint32 CreateTexture2D(Vector2i size, const void* data, SamplerParameters samplerParams, bool compress, bool useBorder = false, Color borderColor = Color::White);

        /// <summary>
        /// Creates HDRI texture in GL.
        /// </summary>
        uint32 CreateTextureHDRI(Vector2i size, float* data, SamplerParameters samplerParams);

        /// <summary>
        /// Creates a cubemap texture, used for skyboxes & probes.
        /// </summary>
        uint32 CreateCubemapTexture(Vector2i size, SamplerParameters samplerParams, const std::vector<unsigned char*>& data, uint32 dataSize = 6);

        /// <summary>
        /// Creates an empty cubemap texture
        /// </summary>
        uint32 CreateCubemapTextureEmpty(Vector2i size, SamplerParameters samplerParams);

        /// <summary>
        /// Creates texture with MSAA flags.
        /// </summary>
        uint32 CreateTexture2DMSAA(Vector2i size, SamplerParameters samplerParams, int sampleCount);

        /// <summary>
        /// Creates xexture with null data, can be used for framebuffers.
        /// </summary>
        uint32 CreateTexture2DEmpty(Vector2i size, SamplerParameters samplerParams);

        /// <summary>
        /// Updates existing texture sampler parameters.
        /// </summary>
        void UpdateTextureParameters(uint32 bindMode, uint32 id, SamplerParameters samplerParmas);

        /// <summary>
        /// Deletes an existing GL texture.
        /// </summary>
        uint32 ReleaseTexture2D(uint32 texture2D);

        /// <summary>
        /// Creates a VAO for the given data. Supports multiple vertex and instanced components.
        /// </summary>
        /// <returns></returns>
        uint32 CreateVertexArray(const std::vector<BufferData>& bufferData, uint32 numVertexComponents, uint32 numInstanceComponents, uint32 numVertices, const uint32* indices, uint32 numIndices, BufferUsage bufferUsage);

        /// <summary>
        /// VAO helper for creating skybox cubes.
        /// </summary>
        uint32 CreateSkyboxVertexArray();

        /// <summary>
        /// VAO helper for creating fullscreen quads.
        /// </summary>
        uint32 CreateScreenQuadVertexArray();

        /// <summary>
        /// VAO helper for creating lines.
        /// </summary>
        uint32 CreateLineVertexArray();

        /// <summary>
        /// VAO helper for creating HDRI cubemaps.
        /// </summary>
        uint32 CreateHDRICubeVertexArray();

        /// <summary>
        /// Deletes the given VAO.
        /// </summary>
        uint32 ReleaseVertexArray(uint32 vao, bool checkMap = true);

        /// <summary>
        /// Creates a texture sampler.
        /// </summary>
        uint32 CreateSampler(SamplerParameters samplerParams, bool isCubemap = false);

        /// <summary>
        /// Deletes the given sampler.
        /// </summary>
        uint32 ReleaseSampler(uint32 sampler);

        /// <summary>
        /// Creates & initializes uniform buffer object (UBO).
        /// </summary>
        uint32 CreateUniformBuffer(const void* data, uintptr dataSize, BufferUsage usage);

        /// <summary>
        /// Deletes the given uniform buffer object (UBO).
        /// </summary>
        uint32 ReleaseUniformBuffer(uint32 buffer);

        /// <summary>
        /// Loads the given text as an OpenGL program.
        /// </summary>
        uint32 CreateShaderProgram(const std::string& shaderText, ShaderUniformData* data, bool usesGeometryShader);

        /// <summary>
        /// Checks the validity of the given loaded shader program.
        /// </summary>
        bool ValidateShaderProgram(uint32 shader);

        /// <summary>
        /// Unloads the given loaded shader program.
        /// </summary>
        uint32 ReleaseShaderProgram(uint32 shader);

        /// <summary>
        /// Creates a framebuffer object.
        /// </summary>
        /// <returns></returns>
        uint32 CreateRenderTarget(uint32                texture,
                                  TextureBindMode       bindTextureMode,
                                  FrameBufferAttachment attachment,
                                  uint32                attachmentNumber,
                                  uint32                mipLevel,
                                  bool                  noReadWrite,
                                  bool                  bindRBO    = false,
                                  FrameBufferAttachment rboAtt     = FrameBufferAttachment::ATTACHMENT_DEPTH_AND_STENCIL,
                                  uint32                rbo        = 0,
                                  bool                  errorCheck = true);

        /// <summary>
        /// Binds a texture to the given framebuffer.
        /// </summary>
        void BindTextureToRenderTarget(uint32 fbo, uint32 texture, TextureBindMode bindTextureMode, FrameBufferAttachment attachment, uint32 attachmentNumber, uint32 textureAttachmentNumber = 0, int mipLevel = 0, bool bindTexture = true, bool setDefaultFBO = true);

        /// <summary>
        /// Sets whether a framebuffer is supposed to draw to multiple buffers.
        /// </summary>
        void MultipleDrawBuffersCommand(uint32 fbo, uint32 bufferCount, uint32* attachments);

        /// <summary>
        /// Resizes the given framebuffer texture.
        /// </summary>
        void ResizeRTTexture(uint32 texture, Vector2i newSize, PixelFormat m_internalPixelFormat, PixelFormat m_pixelFormat, TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D, bool compress = false);

        /// <summary>
        /// Resizes the given render buffer.
        /// </summary>
        void ResizeRenderBuffer(uint32 fbo, uint32 rbo, Vector2i newSize, RenderBufferStorage storage);

        /// <summary>
        /// Deletes the given framebuffer.
        /// </summary>
        uint32 ReleaseRenderTarget(uint32 target);

        /// <summary>
        /// Creates a render buffer object (RBO).
        /// </summary>
        uint32 CreateRenderBufferObject(RenderBufferStorage storage, uint32 width, uint32 height, int sampleCount);

        /// <summary>
        /// Deletes the given render buffer object.
        /// </summary>
        uint32 ReleaseRenderBufferObject(uint32 target);

        /// <summary>
        /// Copies pixel data from one frame buffer to another.
        /// </summary>
        void BlitRenderTargets(uint32 readFBO, uint32 readWidth, uint32 readHeight, uint32 writeFBO, uint32 writeWidth, uint32 writeHeight, BufferBit mask, SamplerFilter filter, FrameBufferAttachment att, uint32 attCount);

        /// <summary>
        /// Update sthe vertex array buffer with the given data, uses glBufferData or glBufferSubdata according to the buffer size.
        /// </summary>
        void UpdateVertexArrayBuffer(uint32 vao, uint32 bufferIndex, const void* data, uintptr dataSize);

        /// <summary>
        /// Scans the given shader and returns a struct containing information about all the uniforms existing in the shader.
        /// </summary>
        ShaderUniformData ScanShaderUniforms(uint32 shader);

        /// <summary>
        /// Generate mipmap data for the given texture. Used internally for HDRI calculations.
        /// </summary>
        void GenerateTextureMipmaps(uint32 texture, TextureBindMode bindMode);

        /// <summary>
        /// Checks the validity of the given framebuffer.
        /// </summary>
        bool IsRenderTargetComplete(uint32 fbo);

        /// <summary>
        /// Reads the given pixel & fills in the existing data.
        /// </summary>
        void ReadPixels(uint32 x, uint32 y, uint32 w, uint32 h, FrameBufferAttachment att, uint32 attachmentNumder, void* data);

        /// <summary>
        /// Binds the given texture and reads it's data into the given pixel array.
        /// </summary>
        /// <param name="pixels"></param>
        void GetTextureImage(uint32 texture, PixelFormat format, TextureBindMode bind, void*& pixels);

        void BindUniformBuffer(uint32 buffer, uint32 bindingPoint);
        void BindShaderBlockToBufferPoint(uint32 shader, uint32 blockPoint, std::string& blockName);
        void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr offset, uintptr dataSize);
        void UpdateUniformBuffer(uint32 buffer, const void* data, uintptr dataSize);
        void UpdateSamplerParameters(uint32 sampler, SamplerParameters params);
        void SetShader(uint32 shader);
        void SetTexture(uint32 texture, uint32 sampler, uint32 unit, TextureBindMode bindTextureMode = TextureBindMode::BINDTEXTURE_TEXTURE2D, bool setSampler = false);
        void SetShaderUniformBuffer(uint32 shader, const std::string& uniformBufferName, uint32 buffer);
        void SetDrawParameters(const DrawParams& drawParams);
        void Draw(uint32 vao, const DrawParams& drawParams, uint32 numInstances, uint32 numElements, bool drawArrays = false);
        void DrawLine(float width);
        void DrawLine(uint32 shader, const Matrix& model, const Vector3& from, const Vector3& to, float width = 1.0f);
        void UpdateShaderUniformFloat(uint32 shader, const std::string& uniform, const float f);
        void UpdateShaderUniformInt(uint32 shader, const std::string& uniform, const int f);
        void UpdateShaderUniformColor(uint32 shader, const std::string& uniform, const Color& color);
        void UpdateShaderUniformVector2(uint32 shader, const std::string& uniform, const Vector2& m);
        void UpdateShaderUniformVector3(uint32 shader, const std::string& uniform, const Vector3& m);
        void UpdateShaderUniformVector4F(uint32 shader, const std::string& uniform, const Vector4& m);
        void UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, const Matrix& m);
        void UpdateShaderUniformMatrix(uint32 shader, const std::string& uniform, void* data);
        void SetFBO(uint32 fbo);
        void SetVAO(uint32 vao);
        void SetViewport(Vector2i pos, Vector2i size);
        void CaptureHDRILightingData(Matrix& view, Matrix& projection, Vector2i captureSize, uint32 cubeMapTexture, uint32 hdrTexture, uint32 fbo, uint32 rbo, uint32 shader);

        /// <summary>
        /// Calls glClearColor, pass in boolean arguments to determine which bits.
        /// </summary>
        void Clear(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const class Color& color, uint32 stencil);

    private:
        std::string GetShaderVersion();
        uint32      GetVersion();

        void SetRBO(uint32 rbo);
        void SetFaceCulling(FaceCulling faceCulling);
        void SetDepthTest(bool shouldWrite, DrawFunc depthFunc);
        void SetBlending(BlendFunc sourceBlend, BlendFunc destBlend);
        void SetStencilTest(bool enable, DrawFunc stencilFunc, uint32 stencilTestMask, uint32 stencilWriteMask, int32 stencilComparisonVal, StencilOp stencilFail, StencilOp stencilPassButDepthFail, StencilOp stencilPass);
        void SetScissorTest(bool enable, uint32 startX = 0, uint32 startY = 0, uint32 width = 0, uint32 height = 0);
        void SetupTextureParameters(uint32 textureTarget, SamplerParameters samplerParams, bool useBorder = false, float* borderColor = NULL);
        void SetStencilWriteMask(uint32 mask);
        void SetDepthTestEnable(bool enable);

    private:
        static RenderDevice* s_renderDevice;

        uint32                            m_boundShader   = 0;
        uint32                            m_boundVAO      = 0;
        uint32                            m_boundFBO      = 0;
        uint32                            m_boundReadFBO  = 0;
        uint32                            m_boundWriteFBO = 0;
        uint32                            m_viewportFBO   = 0;
        uint32                            m_boundRBO      = 0;
        uint32                            m_boundUBO;
        uint32                            m_boundTextureUnit;
        Vector2i                         m_boundViewportSize;
        Vector2i                         m_boundViewportPos;
        std::map<uint32, VertexArrayData> m_vaoMap;
        std::map<uint32, ShaderProgram>   m_shaderProgramMap;
        std::string                       m_shaderVersion;
        uint32                            m_GLVersion;

        // Current drawing parameters.
        FaceCulling m_usedFaceCulling;
        DrawFunc    m_usedDepthFunction;
        BlendFunc   m_usedSourceBlending;
        BlendFunc   m_usedDestinationBlending;
        DrawFunc    m_usedStencilFunction;
        StencilOp   m_usedStencilFail;
        StencilOp   m_usedStencilPassButDepthFail;
        StencilOp   m_usedStencilPass;

        // Current operation parameters.
        uint32 m_usedStencilTestMask        = 0;
        uint32 m_usedStencilWriteMask       = 0;
        int32  m_usedStencilComparisonValue = 0;
        bool   m_isBlendingEnabled          = true;
        bool   m_isStencilTestEnabled       = true;
        bool   m_isScissorsTestEnabled      = false;
        bool   m_shouldWriteDepth           = true;
        bool   m_isDepthTestEnabled         = true;
        Color  m_currentClearColor          = Color::Black;
    };
} // namespace Lina::Graphics

#endif
