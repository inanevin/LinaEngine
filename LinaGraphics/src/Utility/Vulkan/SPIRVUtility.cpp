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

#include "Utility/Vulkan/SPIRVUtility.hpp"
#include "Data/DataCommon.hpp"

namespace Lina::Graphics
{
    void SPIRVUtility::Initialize()
    {
        glslang::InitializeProcess();
    }

    void SPIRVUtility::Shutdown()
    {
        glslang::FinalizeProcess();
    }

    void SPIRVUtility::InitResources(TBuiltInResource& resources)
    {
        resources.maxLights                                   = 32;
        resources.maxClipPlanes                               = 6;
        resources.maxTextureUnits                             = 32;
        resources.maxTextureCoords                            = 32;
        resources.maxVertexAttribs                            = 64;
        resources.maxVertexUniformComponents                  = 4096;
        resources.maxVaryingFloats                            = 64;
        resources.maxVertexTextureImageUnits                  = 32;
        resources.maxCombinedTextureImageUnits                = 80;
        resources.maxTextureImageUnits                        = 32;
        resources.maxFragmentUniformComponents                = 4096;
        resources.maxDrawBuffers                              = 32;
        resources.maxVertexUniformVectors                     = 128;
        resources.maxVaryingVectors                           = 8;
        resources.maxFragmentUniformVectors                   = 16;
        resources.maxVertexOutputVectors                      = 16;
        resources.maxFragmentInputVectors                     = 15;
        resources.minProgramTexelOffset                       = -8;
        resources.maxProgramTexelOffset                       = 7;
        resources.maxClipDistances                            = 8;
        resources.maxComputeWorkGroupCountX                   = 65535;
        resources.maxComputeWorkGroupCountY                   = 65535;
        resources.maxComputeWorkGroupCountZ                   = 65535;
        resources.maxComputeWorkGroupSizeX                    = 1024;
        resources.maxComputeWorkGroupSizeY                    = 1024;
        resources.maxComputeWorkGroupSizeZ                    = 64;
        resources.maxComputeUniformComponents                 = 1024;
        resources.maxComputeTextureImageUnits                 = 16;
        resources.maxComputeImageUniforms                     = 8;
        resources.maxComputeAtomicCounters                    = 8;
        resources.maxComputeAtomicCounterBuffers              = 1;
        resources.maxVaryingComponents                        = 60;
        resources.maxVertexOutputComponents                   = 64;
        resources.maxGeometryInputComponents                  = 64;
        resources.maxGeometryOutputComponents                 = 128;
        resources.maxFragmentInputComponents                  = 128;
        resources.maxImageUnits                               = 8;
        resources.maxCombinedImageUnitsAndFragmentOutputs     = 8;
        resources.maxCombinedShaderOutputResources            = 8;
        resources.maxImageSamples                             = 0;
        resources.maxVertexImageUniforms                      = 0;
        resources.maxTessControlImageUniforms                 = 0;
        resources.maxTessEvaluationImageUniforms              = 0;
        resources.maxGeometryImageUniforms                    = 0;
        resources.maxFragmentImageUniforms                    = 8;
        resources.maxCombinedImageUniforms                    = 8;
        resources.maxGeometryTextureImageUnits                = 16;
        resources.maxGeometryOutputVertices                   = 256;
        resources.maxGeometryTotalOutputComponents            = 1024;
        resources.maxGeometryUniformComponents                = 1024;
        resources.maxGeometryVaryingComponents                = 64;
        resources.maxTessControlInputComponents               = 128;
        resources.maxTessControlOutputComponents              = 128;
        resources.maxTessControlTextureImageUnits             = 16;
        resources.maxTessControlUniformComponents             = 1024;
        resources.maxTessControlTotalOutputComponents         = 4096;
        resources.maxTessEvaluationInputComponents            = 128;
        resources.maxTessEvaluationOutputComponents           = 128;
        resources.maxTessEvaluationTextureImageUnits          = 16;
        resources.maxTessEvaluationUniformComponents          = 1024;
        resources.maxTessPatchComponents                      = 120;
        resources.maxPatchVertices                            = 32;
        resources.maxTessGenLevel                             = 64;
        resources.maxViewports                                = 16;
        resources.maxVertexAtomicCounters                     = 0;
        resources.maxTessControlAtomicCounters                = 0;
        resources.maxTessEvaluationAtomicCounters             = 0;
        resources.maxGeometryAtomicCounters                   = 0;
        resources.maxFragmentAtomicCounters                   = 8;
        resources.maxCombinedAtomicCounters                   = 8;
        resources.maxAtomicCounterBindings                    = 1;
        resources.maxVertexAtomicCounterBuffers               = 0;
        resources.maxTessControlAtomicCounterBuffers          = 0;
        resources.maxTessEvaluationAtomicCounterBuffers       = 0;
        resources.maxGeometryAtomicCounterBuffers             = 0;
        resources.maxFragmentAtomicCounterBuffers             = 1;
        resources.maxCombinedAtomicCounterBuffers             = 1;
        resources.maxAtomicCounterBufferSize                  = 16384;
        resources.maxTransformFeedbackBuffers                 = 4;
        resources.maxTransformFeedbackInterleavedComponents   = 64;
        resources.maxCullDistances                            = 8;
        resources.maxCombinedClipAndCullDistances             = 8;
        resources.maxSamples                                  = 4;
        resources.maxMeshOutputVerticesNV                     = 256;
        resources.maxMeshOutputPrimitivesNV                   = 512;
        resources.maxMeshWorkGroupSizeX_NV                    = 32;
        resources.maxMeshWorkGroupSizeY_NV                    = 1;
        resources.maxMeshWorkGroupSizeZ_NV                    = 1;
        resources.maxTaskWorkGroupSizeX_NV                    = 32;
        resources.maxTaskWorkGroupSizeY_NV                    = 1;
        resources.maxTaskWorkGroupSizeZ_NV                    = 1;
        resources.maxMeshViewCountNV                          = 4;
        resources.limits.nonInductiveForLoops                 = 1;
        resources.limits.whileLoops                           = 1;
        resources.limits.doWhileLoops                         = 1;
        resources.limits.generalUniformIndexing               = 1;
        resources.limits.generalAttributeMatrixVectorIndexing = 1;
        resources.limits.generalVaryingIndexing               = 1;
        resources.limits.generalSamplerIndexing               = 1;
        resources.limits.generalVariableIndexing              = 1;
        resources.limits.generalConstantMatrixVectorIndexing  = 1;
    }

    EShLanguage SPIRVUtility::FindLanguage(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:
            return EShLangVertex;
        case ShaderStage::TesellationControl:
            return EShLangTessControl;
        case ShaderStage::TesellationEval:
            return EShLangTessEvaluation;
        case ShaderStage::Geometry:
            return EShLangGeometry;
        case ShaderStage::Fragment:
            return EShLangFragment;
        case ShaderStage::Compute:
            return EShLangCompute;
        default:
            return EShLangVertex;
        }
    }
    bool SPIRVUtility::GLSLToSPV(ShaderStage stg, const char* pShader, Vector<unsigned int>& spirv)
    {
        EShLanguage       stage = FindLanguage(stg);
        glslang::TShader  shader(stage);
        glslang::TProgram program;
        const char*       shaderStrings[1];
        TBuiltInResource  Resources = {};
        InitResources(Resources);
       
        // Enable SPIR-V and Vulkan rules when parsing GLSL
        EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

        shaderStrings[0] = pShader;
        shader.setStrings(shaderStrings, 1);

        if (!shader.parse(&Resources, 100, false, messages))
        {
            puts(shader.getInfoLog());
            puts(shader.getInfoDebugLog());
            LINA_ERR("[SPIRV Utility] -> Could not parse shader!");
            return false; // something didn't work
        }

        program.addShader(&shader);

        //
        // Program-level processing...
        //

        if (!program.link(messages))
        {
            puts(shader.getInfoLog());
            puts(shader.getInfoDebugLog());
            fflush(stdout);
            return false;
        }

        std::vector<unsigned int> arr;
        auto& inter = *program.getIntermediate(stage);
        glslang::GlslangToSpv(inter, arr);
        spirv.reserve(arr.size());

        for(auto i : arr)
            spirv.push_back(i);

        return true;
    }

} // namespace Lina::Graphics
