#include "Panels/TextEditorPanel.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/ImGuiCommon.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/InputEngine.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/GUILayer.hpp"
#include "Rendering/ShaderInclude.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
    static const char* ppnames[]  = {"LINA_PROJ", "LINA_VIEW", "LINA_LIGHTSPACE", "LINA_VP", "LINA_CAMPOS", "LINA_CAMNEAR", "LINA_CAMFAR", "LINA_AMBIENT",
                                    "LINA_PLIGHT_COUNT", "LINA_SLIGHT_COUNT", "LINA_VISUALIZEDEPTH", "LINA_SCREENSIZE", "LINA_MOUSEPOS", "LINA_DT", "LINA_ET"};
    static const char* ppvalues[] = {
        "Active camera projection matrix", "Active camera view matrix.", "Point light matrix.", "Active camera projection * view matrix.",
        "Active camera position.", "Active camera near plane.", "Active camera far plane.", "Level ambient color",
        "Total point light count.", "Total spotlight count.", "Whether depth visualization is enabled or not.", "Current screen resolution (whole editor if enabled).",
        "Mouse position (0,0 top-left, screenSize.x, screenSize.y bottom-right)", "Delta time.", "Elapsed time."};

    static const char* const keywords[] = {
        "const",
        "uniform",
        "buffer",
        "shared",
        "attribute",
        "varying",
        "coherent",
        "volatile",
        "restrict",
        "readonly",
        "writeonly",
        "atomic_uint",
        "layout",
        "centroid",
        "flat",
        "smooth",
        "noperspective",
        "patch",
        "sample",
        "invariant",
        "precise",
        "break",
        "continue",
        "do",
        "for",
        "while",
        "switch",
        "case",
        "default",
        "if",
        "else",
        "subroutine",
        "in",
        "out",
        "inout",
        "int",
        "void",
        "bool",
        "true",
        "false",
        "float",
        "double",
        "discard",
        "return",
        "vec2",
        "vec3",
        "vec4",
        "ivec2",
        "ivec3",
        "ivec4",
        "bvec2",
        "bvec3",
        "bvec4",
        "uint",
        "uvec2",
        "uvec3",
        "uvec4",
        "dvec2",
        "dvec3",
        "dvec4",
        "mat2",
        "mat3",
        "mat4",
        "mat2x2",
        "mat2x3",
        "mat2x4",
        "mat3x2",
        "mat3x3",
        "mat3x4",
        "mat4x2",
        "mat4x3",
        "mat4x4",
        "dmat2",
        "dmat3",
        "dmat4",
        "dmat2x2",
        "dmat2x3",
        "dmat2x4",
        "dmat3x2",
        "dmat3x3",
        "dmat3x4",
        "dmat4x2",
        "dmat4x3",
        "dmat4x4",
        "lowp",
        "mediump",
        "highp",
        "precision",
        "sampler1D",
        "sampler1DShadow",
        "sampler1DArray",
        "sampler1DArrayShadow",
        "isampler1D",
        "isampler1DArray",
        "usampler1D usampler1DArray",
        "sampler2D",
        "sampler2DShadow",
        "sampler2DArray",
        "sampler2DArrayShadow",
        "isampler2D",
        "isampler2DArray",
        "usampler2D",
        "usampler2DArray",
        "sampler2DRect",
        "sampler2DRectShadow",
        "isampler2DRect",
        "usampler2DRect",
        "sampler2DMS",
        "isampler2DMS",
        "usampler2DMS",
        "sampler2DMSArray",
        "isampler2DMSArray",
        "usampler2DMSArray",
        "sampler3D",
        "isampler3D",
        "usampler3D",
        "samplerCube",
        "samplerCubeShadow",
        "isamplerCube",
        "usamplerCube",
        "samplerCubeArray",
        "samplerCubeArrayShadow",
        "isamplerCubeArray",
        "usamplerCubeArray",
        "samplerBuffer",
        "isamplerBuffer",
        "usamplerBuffer",
        "image1D",
        "iimage1D",
        "uimage1D",
        "image1DArray",
        "iimage1DArray",
        "uimage1DArray",
        "image2D",
        "iimage2D",
        "uimage2D",
        "image2DArray",
        "iimage2DArray",
        "uimage2DArray",
        "image2DRect",
        "iimage2DRect",
        "uimage2DRect",
        "image2DMS",
        "iimage2DMS",
        "uimage2DMS",
        "image2DMSArray",
        "iimage2DMSArray",
        "uimage2DMSArray",
        "image3D",
        "iimage3D",
        "uimage3D",
        "imageCube",
        "iimageCube",
        "uimageCube",
        "imageCubeArray",
        "iimageCubeArray",
        "uimageCubeArray",
        "imageBuffer",
        "iimageBuffer",
        "uimageBuffer",
        "struct",
        "MaterialSampler2D",
        "MaterialSamplerCube",
    };

    static const char* const identifiers[] = {
        "radians", "degrees", "sin", "cos", "tan", "asin", "acos", "atan", "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
        "pow", "exp", "log", "exp2", "log2", "sqrt", "inversesqrt",
        "abs", "sign", "floor", "trunc", "round", "roundEven", "ceil", "fract", "mod", "modf", "min", "max", "clamp", "mix", "step", "smoothstep", "isnan", "isinf", "floatBitsToInt", "floatBitsToUint", "intBitsToFloat", "uintBitsToFloat", "fma", "frexp", "ldexp",
        "packUnorm2x16", "packSnorm2x16", "packUnorm4x8", "packSnorm4x8", "unpackUnorm2x16", "unpackSnorm2x16", "unpackUnorm4x8", "unpackSnorm4x8", "packHalf2x16", "unpackHalf2x16", "packDouble2x32", "unpackDouble2x32",
        "length", "distance", "dot", "cross", "normalize", "ftransform", "faceforward", "reflect", "refract",
        "matrixCompMult", "outerProduct", "transpose", "determinant", "inverse",
        "lessThan", "lessThanEqual", "greaterThan", "greaterThanEqual", "equal", "notEqual", "any", "all", "not",
        "uaddCarry", "usubBorrow", "umulExtended", "imulExtended", "bitfieldExtract", "bitfieldInsert", "bitfieldReverse", "bitCount", "findLSB", "findMSB",
        "textureSize", "textureQueryLod", "textureQueryLevels", "textureSamples",
        "texture", "textureProj", "textureLod", "textureOffset", "texelFetch", "texelFetchOffset", "textureProjOffset", "textureLodOffset", "textureProjLod", "textureProjLodOffset", "textureGrad", "textureGradOffset", "textureProjGrad", "textureProjGradOffset",
        "textureGather", "textureGatherOffset", "textureGatherOffsets",
        "texture1D", "texture1DProj", "texture1DLod", "texture1DProjLod", "texture2D", "texture2DProj", "texture2DLod", "texture2DProjLod", "texture3D", "texture3DProj", "texture3DLod", "texture3DProjLod", "textureCube", "textureCubeLod", "shadow1D", "shadow2D", "shadow1DProj", "shadow2DProj", "shadow1DLod", "shadow2DLod", "shadow1DProjLod", "shadow2DProjLod",
        "atomicCounterIncrement", "atomicCounterDecrement", "atomicCounter", "atomicCounterAdd", "atomicCounterSubtract", "atomicCounterMin", "atomicCounterMax", "atomicCounterAnd", "atomicCounterOr", "atomicCounterXor", "atomicCounterExchange", "atomicCounterCompSwap",
        "atomicAdd", "atomicMin", "atomicMax", "atomicAnd", "atomicOr", "atomicXor", "atomicExchange", "atomicCompSwap",
        "imageSize", "imageSamples", "imageLoad", "imageStore", "imageAtomicAdd", "imageAtomicMin", "imageAtomicMax", "imageAtomicAnd", "imageAtomicOr", "imageAtomicXor", "imageAtomicExchange", "imageAtomicCompSwap",
        "EmitStreamVertex", "EndStreamPrimitive", "EmitVertex", "EndPrimitive",
        "dFdx", "dFdy", "dFdxFine", "dFdyFine", "dFdxCoarse", "dFdyCoarse", "fwidth", "fwidthFine", "fwidthCoarse",
        "interpolateAtCentroid", "interpolateAtSample", "interpolateAtOffset",
        "noise1", "noise2", "noise3", "noise4",
        "barrier",
        "memoryBarrier", "memoryBarrierAtomicCounter", "memoryBarrierBuffer", "memoryBarrierShared", "memoryBarrierImage", "groupMemoryBarrier",
        "subpassLoad",
        "anyInvocation", "allInvocations", "allInvocationsEqual"};

    

    void TextEditorPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);

        Event::EventSystem::Get()->Connect<EShortcut, &TextEditorPanel::OnShortcut>(this);

    }

    void TextEditorPanel::Draw()
    {
    }

    void TextEditorPanel::Close()
    {
        EditorPanel::Close();
        m_currentFile = nullptr;
    }

    void TextEditorPanel::AddFile(Utility::File* file)
    {
       
    }

    void TextEditorPanel::OnShortcut(const EShortcut& ev)
    {
       
    }

    void TextEditorPanel::SaveCurrentFile(const String& text)
    {
        if (m_currentFile == nullptr)
            return;

        Utility::RewriteFileContents(m_currentFile, text);

        const String  path   = m_currentFile->m_fullPath;
        const StringIDType sid    = m_currentFile->sid;
        TypeID             tid    = 0;
        bool               reload = false;

        if (m_currentFile->m_extension.compare("linaglh") == 0)
        {
            reload = true;
            tid    = GetTypeID<Graphics::ShaderInclude>();
            Resources::ResourceStorage::Get()->Unload<Graphics::ShaderInclude>(sid);
        }
        else if (m_currentFile->m_extension.compare("linaglsl") == 0)
        {
            reload = true;
            tid    = GetTypeID<Graphics::Shader>();
            Resources::ResourceStorage::Get()->Unload<Graphics::Shader>(sid);
        }

        if (reload)
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{m_currentFile->m_fullPath, tid, sid});
    }

    void TextEditorPanel::ReloadCurrentFile()
    {
        if (m_currentFile != nullptr)
        {
            m_openFiles.erase(m_currentFile);
            AddFile(m_currentFile);
        }

    }

} // namespace Lina::Editor