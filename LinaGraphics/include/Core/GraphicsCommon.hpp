/*
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

#pragma once

#ifndef GraphicsCommon_HPP
#define GraphicsCommon_HPP

#include "Math/Color.hpp"
#include "Data/Vector.hpp"
#include "Math/Matrix.hpp"
#include "Core/SizeDefinitions.hpp"

enum VkFormat;
enum VkColorSpaceKHR;
enum VkPresentModeKHR;
enum VkCommandBufferLevel;
enum VkCommandPoolCreateFlagBits;
enum VkQueueFlagBits;
enum VkImageLayout;
enum VkPipelineBindPoint;
enum VkAttachmentLoadOp;
enum VkAttachmentStoreOp;
enum VkCompareOp;
enum VkFenceCreateFlagBits;
enum VkCommandBufferUsageFlagBits;
enum VkShaderStageFlagBits;
enum VkPrimitiveTopology;
enum VkPolygonMode;
enum VkCullModeFlagBits;
enum VkVertexInputRate;
enum VkImageUsageFlagBits;
enum VkImageTiling;
enum VkImageAspectFlagBits;
enum VmaMemoryUsage;
enum VkDescriptorType;
enum VkIndexType;
enum VkFilter;
enum VkSamplerAddressMode;
enum VkFrontFace;
enum VkBorderColor;
enum VkBlendFactor;
enum VkBlendOp;
enum VkLogicOp;
enum VkResult;
enum VkSharingMode;

struct VmaAllocation_T;
struct VkBuffer_T;
struct VkImage_T;
struct VkImageView_T;
struct VkSampler_T;
struct VkDescriptorSet_T;

namespace Lina::Graphics
{
    enum class Format
    {
        B8G8R8A8_SRGB,
        B8G8R8A8_UNORM,
        R32G32B32_SFLOAT,
        R32G32B32A32_SFLOAT,
        R32G32_SFLOAT,
        D32_SFLOAT,
        R8G8B8A8_UNORM,
        R8G8B8A8_SRGB,
        R8_UNORM,
        R8_UINT,
        R16_SFLOAT,
        R16_SINT,
        R32_SFLOAT,
        R32_SINT,
    };

    extern VkFormat GetFormat(Format f);

    enum class Filter
    {
        Nearest,
        Linear,
        CubicImg,
    };

    extern VkFilter GetFilter(Filter f);

    enum class SamplerAddressMode
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    extern VkSamplerAddressMode GetSamplerAddressMode(SamplerAddressMode m);

    enum class ColorSpace
    {
        SRGB_NONLINEAR,
    };

    extern VkColorSpaceKHR GetColorSpace(ColorSpace f);

    enum class PresentMode
    {
        Immediate,
        Mailbox,
        FIFO,
        FIFORelaxed,
    };

    extern VkPresentModeKHR GetPresentMode(PresentMode m);

    enum class CommandBufferLevel
    {
        Primary,
        Secondary,
    };

    extern VkCommandBufferLevel GetCommandBufferLevel(CommandBufferLevel lvl);

    enum class CommandPoolFlags
    {
        Transient,
        Reset,
        Protected,
    };

    extern uint32 GetCommandPoolCreateFlags(CommandPoolFlags f);

    enum class QueueFamilies
    {
        Graphics,
        Compute,
        Transfer,
        SparseBinding,
        Protected,
    };

    extern uint32 GetQueueFamilyBit(QueueFamilies f);

    enum class ImageLayout
    {
        Undefined,
        General,
        ColorOptimal,
        DepthStencilOptimal,
        DepthStencilReadOnlyOptimal,
        PresentSurface,
        TransferSrcOptimal,
        TransferDstOptimal,
        ShaderReadOnlyOptimal,
    };

    extern VkImageLayout GetImageLayout(ImageLayout l);

    enum class PipelineBindPoint
    {
        Graphics,
        Computer,
    };

    extern VkPipelineBindPoint GetPipelineBindPoint(PipelineBindPoint p);

    enum class LoadOp
    {
        Load,
        Clear,
        DontCare,
        None,
    };

    extern VkAttachmentLoadOp GetLoadOp(LoadOp p);

    enum class StoreOp
    {
        Store,
        DontCare,
        None,
    };

    extern VkAttachmentStoreOp GetStoreOp(StoreOp op);

    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LEqual,
        Greater,
        NotEqual,
        GEqual,
        Always
    };

    extern VkCompareOp GetCompareOp(CompareOp op);

    enum class FenceFlags
    {
        None,
        Signaled,
    };

    extern uint32 GetFenceFlags(FenceFlags f);

    enum class CommandBufferFlags
    {
        OneTimeSubmit,
        RenderPassContinue,
        SimultaneousUse,
    };

    extern uint32 GetCommandBufferFlags(CommandBufferFlags f);

    enum class ShaderStage
    {
        Vertex,
        TesellationControl,
        TesellationEval,
        Geometry,
        Fragment,
        Compute,
    };

    extern uint32 GetShaderStage(ShaderStage s);

    enum class Topology
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        TriangleListAdjacency,
        TriangleStripAdjacency,
    };

    extern VkPrimitiveTopology GetTopology(Topology t);

    enum class PolygonMode
    {
        Fill,
        Line,
        Point,
        FillRect,
    };

    extern VkPolygonMode GetPolygonMode(PolygonMode m);

    enum class CullMode
    {
        None,
        Front,
        Back,
        FrontAndBack,
    };

    extern uint32 GetCullMode(CullMode cm);

    enum class FrontFace
    {
        ClockWise,
        AntiClockWise
    };

    extern VkFrontFace GetFrontFace(FrontFace face);

    enum class VertexInputRate
    {
        Vertex,
        Instance
    };

    extern VkVertexInputRate GetVertexInputRate(VertexInputRate rate);

    enum class ImageUsageFlags
    {
        TransferSrc,
        TransferDest,
        Sampled,
        Storage,
        ColorAttachment,
        DepthStencilAttachment,
        TransientAttachment,
        InputAttachment,
        FragmentDensity,
    };

    extern uint32 GetImageUsage(ImageUsageFlags usage);

    enum class ImageTiling
    {
        Optimal,
        Linear
    };

    extern VkImageTiling GetImageTiling(ImageTiling tiling);

    enum class ImageAspectFlags
    {
        AspectColor,
        AspectDepth,
        AspectStencil,
        AspectMetadata
    };

    extern uint32 GetImageAspectFlags(ImageAspectFlags aspectFlags);

    enum class AccessFlags
    {
        ColorAttachmentRead,
        ColorAttachmentWrite,
        DepthStencilAttachmentRead,
        DepthStencilAttachmentWrite,
        TransferRead,
        TransferWrite,
        ShaderRead,
        MemoryRead,
    };

    extern uint32 GetAccessFlags(AccessFlags flags);

    enum class PipelineStageFlags
    {
        TopOfPipe,
        ColorAttachmentOutput,
        EarlyFragmentTests,
        LateFragmentTests,
        Transfer,
        BottomOfPipe,
        FragmentShader,
    };

    extern uint32 GetPipelineStageFlags(PipelineStageFlags flags);

    enum class BufferUsageFlags
    {
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        StorageBuffer,
        TransferSrc,
        TransferDst,
        IndirectBuffer,
    };

    extern uint32 GetBufferUsageFlags(BufferUsageFlags flags);

    enum class MemoryUsageFlags
    {
        CpuOnly,
        GpuOnly,
        CpuToGpu,
        GpuToCpu,
    };

    extern VmaMemoryUsage GetMemoryUsageFlags(MemoryUsageFlags flags);

    enum class MemoryPropertyFlags
    {
        None,
        DeviceLocal,
        HostVisible,
        HostCoherent,
        HostCached,
    };

    extern uint32 GetMemoryPropertyFlags(MemoryPropertyFlags flags);

    enum class DescriptorSetCreateFlags
    {
        None,
        UpdateAfterBind,
        Push
    };

    extern uint32 GetDescriptorSetCreateFlags(DescriptorSetCreateFlags flags);

    enum class DescriptorPoolCreateFlags
    {
        None,
        FreeDescriptorSet,
        UpdateAfterBind,
        HostOnly,
    };

    extern uint32 GetDescriptorPoolCreateFlags(DescriptorPoolCreateFlags flags);

    enum class DescriptorType
    {
        UniformBuffer,
        UniformBufferDynamic,
        CombinedImageSampler,
        StorageBuffer,
    };

    extern VkDescriptorType GetDescriptorType(DescriptorType type);

    enum class IndexType
    {
        Uint16,
        Uint32,
        Uint8_Ext,
        None,
    };

    extern VkIndexType GetIndexType(IndexType type);

    enum class BorderColor
    {
        FloatTransparentBlack,
        FloatOpaqueWhite,
        FloatOpaqueBlack,
    };

    extern VkBorderColor GetBorderColor(BorderColor col);

    enum class DependencyFlag
    {
        ByRegion,
        DeviceGroup,
    };

    uint32 GetDependencyFlags(DependencyFlag flag);

    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
    };

    VkBlendFactor GetBlendFactor(BlendFactor factor);

    enum class BlendOp
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    VkBlendOp GetBlendOp(BlendOp blendOp);

    enum class ColorComponentFlags
    {
        R,
        G,
        B,
        A,
        RG,
        RGB,
        RGBA
    };

    uint32 GetColorComponentFlags(ColorComponentFlags flags);

    enum class LogicOp
    {
        Clear,
        And,
        AndReverse,
        Copy,
        AndInverted,
        NoOp,
        XOR,
        OR,
        NOR,
        Equivalent
    };

    VkLogicOp GetLogicOp(LogicOp op);

    enum class VulkanResult
    {
        OutOfDateKHR,
        SuboptimalKHR,
        Success,
        Error
    };

    VulkanResult GetResult(int32 result);

    enum class DescriptorSetLayoutBindingFlags
    {
        None,
        UpdateAfterBind,
        UpdateUnusedWhilePending,
        PartiallyBound
    };

    uint32 GetDescriptorLayoutBindingFlags(DescriptorSetLayoutBindingFlags flags);

    enum class SharingMode
    {
        Exclusive,
        Concurrent,
    };

    VkSharingMode GetSharingMode(SharingMode m);

    enum class SurfaceType
    {
        Opaque,
        Transparent,
        Glass
    };

    enum class EngineShaderType
    {
        LitStandard = 0,
        UnlitStandard,
        GUIStandard,
        GUIText,
        SQFinal,
        SQPostProcess,
    };

    enum class EnginePrimitiveType
    {
        Cube = 0,
        Capsule,
        Cylinder,
        Sphere,
        Plane,
        Quad,
        Suzanne,
        LinaLogo,
    };

    enum DrawPassMask
    {
        Opaque      = 1 << 0,
        Transparent = 1 << 1,
        Shadow      = 1 << 2,
    };

    enum class EngineTextureType
    {
        DefaultLina,
        LinaLogoTransparent,
        Grid512,
    };

    enum RenderableType
    {
        RenderableSprite      = 1 << 0,
        RenderableDecal       = 1 << 1,
        RenderableSky         = 1 << 2,
        RenderableSkinnedMesh = 1 << 3,
        RenderableStaticMesh  = 1 << 4,
        RenderableParticle    = 1 << 5,
    };

    enum class DescriptorSetType
    {
        GlobalSet = 0,
        PassSet   = 1,
    };

    enum class MaterialPropertyType
    {
        Float   = 0,
        Int     = 1,
        Bool    = 2,
        Vector2 = 3,
        Vector4 = 4,
        Mat4    = 5,
        Texture = 6,
    };

    enum class RenderPassType
    {
        Main        = 0,
        PostProcess = 1,
        Final       = 2,
    };

    enum class PipelineType
    {
        Default  = 0,
        NoVertex = 1,
        GUI      = 2,
    };

    struct Offset3D
    {
        int32 x = 0;
        int32 y = 0;
        int32 z = 0;
    };

    struct Extent3D
    {
        uint32 width  = 0;
        uint32 height = 0;
        uint32 depth  = 0;
    };

    struct BufferCopy
    {
        uint64 destinationOffset = 0;
        uint64 sourceOffset      = 0;
        uint64 size              = 0;
    };

    struct ImageSubresourceRange
    {
        uint32 aspectFlags    = 0;
        uint32 baseMipLevel   = 0;
        uint32 levelCount     = 1;
        uint32 baseArrayLayer = 0;
        uint32 layerCount     = 1;
    };

    struct BufferImageCopy
    {
        uint64                bufferOffset      = 0;
        uint32                bufferRowLength   = 0;
        uint32                bufferImageHeight = 0;
        ImageSubresourceRange imageSubresource;
        Offset3D              imageOffset;
        Extent3D              imageExtent;
    };

    struct DefaultMemoryBarrier
    {
        uint32 srcAccessMask = 0;
        uint32 dstAccessMask = 0;
    };

    struct BufferMemoryBarrier
    {
        uint32      srcAccessMask       = 0;
        uint32      dstAccessMask       = 0;
        uint32      srcQueueFamilyIndex = 0;
        uint32      dstQueueFamilyIndex = 0;
        VkBuffer_T* buffer              = nullptr;
        uint64      offset              = 0;
        uint64      size                = 0;
    };

    struct ImageMemoryBarrier
    {
        uint32                srcAccessMask       = 0;
        uint32                dstAccessMask       = 0;
        ImageLayout           oldLayout           = ImageLayout::Undefined;
        ImageLayout           newLayout           = ImageLayout::Undefined;
        uint32                srcQueueFamilyIndex = 0;
        uint32                dstQueueFamilyIndex = 0;
        VkImage_T*            img                 = nullptr;
        ImageSubresourceRange subresourceRange;
    };

    struct DescriptorSetLayoutBinding
    {
        uint32         binding         = 0;
        uint32         descriptorCount = 0;
        uint32         stageFlags      = 0;
        DescriptorType type            = DescriptorType::UniformBuffer;
    };

    struct ClearValue
    {
        Color  clearColor = Color::White;
        bool   isColor    = false;
        float  depth      = 1.0f;
        uint32 stencil    = 0;
    };

    struct Attachment
    {
    public:
        uint32      flags          = 0;
        Format      format         = Format::B8G8R8A8_SRGB;
        LoadOp      loadOp         = LoadOp::Load;
        StoreOp     storeOp        = StoreOp::Store;
        LoadOp      stencilLoadOp  = LoadOp::DontCare;
        StoreOp     stencilStoreOp = StoreOp::DontCare;
        ImageLayout initialLayout  = ImageLayout::Undefined;
        ImageLayout finalLayout    = ImageLayout::PresentSurface;
    };

    struct AttachmentReference
    {
        uint32      attachment;
        ImageLayout layout;
    };

    struct Viewport
    {
        float x        = 0.0f;
        float y        = 0.0f;
        float width    = 0.0f;
        float height   = 0.0f;
        float minDepth = 0.0f;
        float maxDepth = 0.0f;
    };

    struct AllocatedBuffer
    {
        VkBuffer_T*      buffer     = nullptr;
        VmaAllocation_T* allocation = nullptr;
    };

    struct AllocatedImage
    {
        VkImage_T*       image      = nullptr;
        VmaAllocation_T* allocation = nullptr;
    };

    struct VertexInputBinding
    {
        uint32          binding   = 0;
        uint32          stride    = 0;
        VertexInputRate inputRate = VertexInputRate::Vertex;
    };

    struct VertexInputAttribute
    {
        uint32 binding  = 0;
        uint32 location = 0;
        Format format   = Format::R32G32B32_SFLOAT;
        uint32 offset   = 0;
    };

    struct VertexInputDescription
    {
        Vector<VertexInputBinding>   bindings;
        Vector<VertexInputAttribute> attributes;
        uint32                       flags = 0;
    };

    struct PushConstantRange
    {
        uint32 offset     = 0;
        uint32 size       = 0;
        uint32 stageFlags = 0;
    };

    struct MeshPushConstants
    {
        Vector4 data         = Vector4::Zero;
        Matrix  renderMatrix = Matrix::Identity();
    };

    struct SubPassDependency
    {
        uint32 srcSubpass      = 0;
        uint32 dstSubpass      = 0;
        uint32 srcStageMask    = 0;
        uint32 dstStageMask    = 0;
        uint32 srcAccessMask   = 0;
        uint32 dstAccessMask   = 0;
        uint32 dependencyFlags = 0;
    };

    struct WriteDescriptorSet
    {
        VkBuffer_T*        buffer          = nullptr;
        uint64             offset          = 0;
        uint64             range           = 0;
        VkDescriptorSet_T* dstSet          = nullptr;
        uint32             dstBinding      = 0;
        uint32             descriptorCount = 1;
        DescriptorType     descriptorType  = DescriptorType::UniformBuffer;
        VkImageView_T*     imageView       = nullptr;
        ImageLayout        imageLayout     = ImageLayout::ColorOptimal;
        VkSampler_T*       sampler         = nullptr;
    };

    struct ShaderDescriptorSetInfo
    {
        uint32         setIndex        = 0;
        uint32         bindingIndex    = 0;
        uint32         stageFlags      = 0;
        DescriptorType type            = DescriptorType::UniformBuffer;
        uint32         descriptorCount = 1;
    };

    struct ShaderReflectedProperty
    {
        uint32               descriptorBinding = 0;
        DescriptorType       descriptorType    = DescriptorType::UniformBuffer;
        String               name              = "";
        MaterialPropertyType propertyType      = MaterialPropertyType::Float;
    };

    struct ColorBlendAttachmentState
    {
        bool                blendEnable         = false;
        BlendFactor         srcColorBlendFactor = BlendFactor::Zero;
        BlendFactor         dstColorBlendFactor = BlendFactor::Zero;
        BlendOp             colorBlendOp        = BlendOp::Add;
        BlendFactor         srcAlphaBlendFactor = BlendFactor::Zero;
        BlendFactor         dstAlphaBlendFactor = BlendFactor::Zero;
        BlendOp             alphaBlendOp        = BlendOp::Add;
        ColorComponentFlags componentFlags      = ColorComponentFlags::RGBA;
    };

#define TO_FLAGS(X) static_cast<uint32>(X)
#define ICONPACK_SID "LINA_ENGINE_ICONPACK"

} // namespace Lina::Graphics

#endif
