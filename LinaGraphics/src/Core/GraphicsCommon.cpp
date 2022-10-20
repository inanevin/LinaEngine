#include "Core/GraphicsCommon.hpp"
#include <vulkan/vulkan.h>
#include "Utility/Vulkan/vk_mem_alloc.h"

namespace Lina::Graphics
{
    VkFormat GetFormat(Format f)
    {
        switch (f)
        {
        case Format::B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case Format::R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case Format::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case Format::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        default:
            return VK_FORMAT_B8G8R8A8_SRGB;
        }
    }

    VkFilter GetFilter(Filter f)
    {
        switch (f)
        {
        case Filter::Linear:
            return VK_FILTER_LINEAR;
        case Filter::Nearest:
            return VK_FILTER_NEAREST;
        case Filter::CubicImg:
            return VK_FILTER_CUBIC_IMG;
        default:
            return VK_FILTER_LINEAR;
        }
    }

    VkSamplerAddressMode GetSamplerAddressMode(SamplerAddressMode m)
    {
        switch (m)
        {
        case SamplerAddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    VkColorSpaceKHR GetColorSpace(ColorSpace f)
    {
        switch (f)
        {
        case ColorSpace::SRGB_NONLINEAR:
            return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        default:
            return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
    }

    VkPresentModeKHR GetPresentMode(PresentMode m)
    {
        switch (m)
        {
        case PresentMode::Immediate:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode::Mailbox:
            return VK_PRESENT_MODE_MAILBOX_KHR;
        case PresentMode::FIFO:
            return VK_PRESENT_MODE_FIFO_KHR;
        case PresentMode::FIFORelaxed:
            return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        default:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    VkCommandBufferLevel GetCommandBufferLevel(CommandBufferLevel lvl)
    {
        switch (lvl)
        {
        case CommandBufferLevel::Primary:
            return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        case CommandBufferLevel::Secondary:
            return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        default:
            return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        }
    }

    uint32 GetCommandPoolCreateFlags(CommandPoolFlags f)
    {
        switch (f)
        {
        case CommandPoolFlags::Transient:
            return VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        case CommandPoolFlags::Reset:
            return VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        case CommandPoolFlags::Protected:
            return VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        default:
            return VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        }
    }

    uint32 GetQueueFamilyBit(QueueFamilies f)
    {
        switch (f)
        {
        case QueueFamilies::Graphics:
            return VK_QUEUE_GRAPHICS_BIT;
        case QueueFamilies::Compute:
            return VK_QUEUE_COMPUTE_BIT;
        case QueueFamilies::Transfer:
            return VK_QUEUE_TRANSFER_BIT;
        case QueueFamilies::SparseBinding:
            return VK_QUEUE_SPARSE_BINDING_BIT;
        case QueueFamilies::Protected:
            return VK_QUEUE_PROTECTED_BIT;
        default:
            return VK_QUEUE_GRAPHICS_BIT;
        }
    }

    VkImageLayout GetImageLayout(ImageLayout l)
    {
        switch (l)
        {
        case ImageLayout::Undefined:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageLayout::General:
            return VK_IMAGE_LAYOUT_GENERAL;
        case ImageLayout::ColorOptimal:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageLayout::DepthStencilOptimal:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ImageLayout::DepthStencilReadOnlyOptimal:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case ImageLayout::PresentSurface:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        case ImageLayout::TransferSrcOptimal:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageLayout::TransferDstOptimal:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case ImageLayout::ShaderReadOnlyOptimal:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    VkPipelineBindPoint GetPipelineBindPoint(PipelineBindPoint p)
    {
        switch (p)
        {
        case PipelineBindPoint::Graphics:
            return VK_PIPELINE_BIND_POINT_GRAPHICS;
        case PipelineBindPoint::Computer:
            return VK_PIPELINE_BIND_POINT_COMPUTE;
        default:
            return VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
    }

    VkAttachmentLoadOp GetLoadOp(LoadOp p)
    {
        switch (p)
        {
        case LoadOp::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOp::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOp::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        case LoadOp::None:
            return VK_ATTACHMENT_LOAD_OP_NONE_EXT;
        default:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    VkAttachmentStoreOp GetStoreOp(StoreOp op)
    {
        switch (op)
        {
        case StoreOp::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOp::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case StoreOp::None:
            return VK_ATTACHMENT_STORE_OP_NONE;
        default:
            return VK_ATTACHMENT_STORE_OP_STORE;
        }
    }

    uint32 GetFenceFlags(FenceFlags f)
    {
        switch (f)
        {
        case FenceFlags::None:
            return 0;
        case FenceFlags::Signaled:
            return VK_FENCE_CREATE_SIGNALED_BIT;
        default:
            return VK_FENCE_CREATE_SIGNALED_BIT;
        }
    }

    uint32 GetCommandBufferFlags(CommandBufferFlags f)
    {
        switch (f)
        {
        case CommandBufferFlags::OneTimeSubmit:
            return VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        case CommandBufferFlags::RenderPassContinue:
            return VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        case CommandBufferFlags::SimultaneousUse:
            return VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        default:
            return VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
    }

    VkCompareOp GetCompareOp(CompareOp op)
    {
        switch (op)
        {
        case CompareOp::Never:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::Less:
            return VK_COMPARE_OP_LESS;
        case CompareOp::Equal:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::LEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::Greater:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::Always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_ALWAYS;
        }
    }

    uint32 GetShaderStage(ShaderStage s)
    {
        switch (s)
        {
        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::TesellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::TesellationEval:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case ShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_VERTEX_BIT;
        }
    }

    VkPrimitiveTopology GetTopology(Topology t)
    {
        switch (t)
        {
        case Topology::PointList:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case Topology::LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case Topology::LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case Topology::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case Topology::TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case Topology::TriangleFan:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case Topology::TriangleListAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case Topology::TriangleStripAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        default:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    VkPolygonMode GetPolygonMode(PolygonMode m)
    {
        switch (m)
        {
        case PolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line:
            return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point:
            return VK_POLYGON_MODE_POINT;
        case PolygonMode::FillRect:
            return VK_POLYGON_MODE_FILL_RECTANGLE_NV;
        default:
            return VK_POLYGON_MODE_FILL;
        }
    }

    uint32 GetCullMode(CullMode m)
    {
        switch (m)
        {
        case CullMode::None:
            return VK_CULL_MODE_NONE;
        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;
        default:
            return VK_CULL_MODE_NONE;
        }
    }

    VkFrontFace GetFrontFace(FrontFace face)
    {
        switch (face)
        {
        case FrontFace::ClockWise:
            return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::AntiClockWise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
    }
    VkVertexInputRate GetVertexInputRate(VertexInputRate rate)
    {
        switch (rate)
        {
        case VertexInputRate::Vertex:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case VertexInputRate::Instance:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
        default:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    uint32 GetImageUsage(ImageUsageFlags usage)
    {
        switch (usage)
        {
        case ImageUsageFlags::TransferSrc:
            return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        case ImageUsageFlags::TransferDest:
            return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        case ImageUsageFlags::Sampled:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
        case ImageUsageFlags::Storage:
            return VK_IMAGE_USAGE_STORAGE_BIT;
        case ImageUsageFlags::ColorAttachment:
            return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        case ImageUsageFlags::DepthStencilAttachment:
            return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        case ImageUsageFlags::TransientAttachment:
            return VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        case ImageUsageFlags::InputAttachment:
            return VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        case ImageUsageFlags::FragmentDensity:
            return VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT;
        default:
            return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
    }

    VkImageTiling GetImageTiling(ImageTiling tiling)
    {
        switch (tiling)
        {
        case ImageTiling::Optimal:
            return VK_IMAGE_TILING_OPTIMAL;
        case ImageTiling::Linear:
            return VK_IMAGE_TILING_LINEAR;
        default:
            return VK_IMAGE_TILING_OPTIMAL;
        }
    }

    uint32 GetImageAspectFlags(ImageAspectFlags aspectFlags)
    {
        switch (aspectFlags)
        {
        case ImageAspectFlags::AspectColor:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case ImageAspectFlags::AspectDepth:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case ImageAspectFlags::AspectStencil:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        case ImageAspectFlags::AspectMetadata:
            return VK_IMAGE_ASPECT_METADATA_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    uint32 GetAccessFlags(AccessFlags flags)
    {
        switch (flags)
        {
        case AccessFlags::ColorAttachmentRead:
            return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        case AccessFlags::ColorAttachmentWrite:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case AccessFlags::DepthStencilAttachmentRead:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        case AccessFlags::DepthStencilAttachmentWrite:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case AccessFlags::TransferWrite:
            return VK_ACCESS_TRANSFER_WRITE_BIT;
        case AccessFlags::TransferRead:
            return VK_ACCESS_TRANSFER_READ_BIT;
        case AccessFlags::ShaderRead:
            return VK_ACCESS_SHADER_READ_BIT;
        case AccessFlags::MemoryRead:
            return VK_ACCESS_MEMORY_READ_BIT;
        default:
            return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        }
    }

    uint32 GetPipelineStageFlags(PipelineStageFlags flags)
    {
        switch (flags)
        {
        case PipelineStageFlags::TopOfPipe:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        case PipelineStageFlags::ColorAttachmentOutput:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case PipelineStageFlags::EarlyFragmentTests:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case PipelineStageFlags::LateFragmentTests:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case PipelineStageFlags::Transfer:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case PipelineStageFlags::BottomOfPipe:
            return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        case PipelineStageFlags::FragmentShader:
            return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        default:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
    }

    uint32 GetBufferUsageFlags(BufferUsageFlags flags)
    {
        switch (flags)
        {
        case BufferUsageFlags::VertexBuffer:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case BufferUsageFlags::IndexBuffer:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case BufferUsageFlags::UniformBuffer:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case BufferUsageFlags::StorageBuffer:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        case BufferUsageFlags::TransferSrc:
            return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        case BufferUsageFlags::TransferDst:
            return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case BufferUsageFlags::IndirectBuffer:
            return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        default:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
    }

    VmaMemoryUsage GetMemoryUsageFlags(MemoryUsageFlags flags)
    {
        switch (flags)
        {
        case MemoryUsageFlags::CpuOnly:
            return VMA_MEMORY_USAGE_CPU_ONLY;
        case MemoryUsageFlags::GpuOnly:
            return VMA_MEMORY_USAGE_GPU_ONLY;
        case MemoryUsageFlags::CpuToGpu:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case MemoryUsageFlags::GpuToCpu:
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        default:
            return VMA_MEMORY_USAGE_CPU_ONLY;
        }
    }

    uint32 GetMemoryPropertyFlags(MemoryPropertyFlags flags)
    {
        switch (flags)
        {
        case MemoryPropertyFlags::None:
            return 0;
        case MemoryPropertyFlags::DeviceLocal:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        case MemoryPropertyFlags::HostVisible:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        case MemoryPropertyFlags::HostCoherent:
            return VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        case MemoryPropertyFlags::HostCached:
            return VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        default:
            return 0;
        }
    }

    uint32 GetDescriptorSetCreateFlags(DescriptorSetCreateFlags flags)
    {
        switch (flags)
        {
        case DescriptorSetCreateFlags::None:
            return 0;
        case DescriptorSetCreateFlags::UpdateAfterBind:
            return VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        case DescriptorSetCreateFlags::Push:
            return VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
        default:
            return 0;
        }
    }

    uint32 GetDescriptorPoolCreateFlags(DescriptorPoolCreateFlags flags)
    {
        switch (flags)
        {
        case DescriptorPoolCreateFlags::None:
            return 0;
        case DescriptorPoolCreateFlags::FreeDescriptorSet:
            return VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        case DescriptorPoolCreateFlags::UpdateAfterBind:
            return VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
        case DescriptorPoolCreateFlags::HostOnly:
            return VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_VALVE;
        default:
            return 0;
        }
    }

    VkDescriptorType GetDescriptorType(DescriptorType type)
    {
        switch (type)
        {
        case DescriptorType::UniformBuffer:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::UniformBufferDynamic:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case DescriptorType::StorageBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case DescriptorType::CombinedImageSampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    VkIndexType GetIndexType(IndexType type)
    {
        switch (type)
        {
        case IndexType::Uint16:
            return VK_INDEX_TYPE_UINT16;
        case IndexType::Uint32:
            return VK_INDEX_TYPE_UINT32;
        case IndexType::None:
            return VK_INDEX_TYPE_NONE_KHR;
        case IndexType::Uint8_Ext:
            return VK_INDEX_TYPE_UINT8_EXT;
        default:
            return VK_INDEX_TYPE_UINT32;
        }
    }

    VkBorderColor GetBorderColor(BorderColor col)
    {
        switch (col)
        {
        case BorderColor::FloatTransparentBlack:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case BorderColor::FloatOpaqueWhite:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        case BorderColor::FloatOpaqueBlack:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        default:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        }
    }

    uint32 GetDependencyFlags(DependencyFlag flag)
    {
        switch (flag)
        {
        case DependencyFlag::ByRegion:
            return VK_DEPENDENCY_BY_REGION_BIT;
        case DependencyFlag::DeviceGroup:
            return VK_DEPENDENCY_DEVICE_GROUP_BIT;
        default:
            return VK_DEPENDENCY_BY_REGION_BIT;
        }
    }

    VkBlendFactor GetBlendFactor(BlendFactor factor)
    {
        VK_BLEND_FACTOR_CONSTANT_ALPHA;
        switch (factor)
        {

        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:
            return VK_BLEND_FACTOR_ZERO;
        }
    }

    VkBlendOp GetBlendOp(BlendOp op)
    {
        switch (op)
        {
        case BlendOp::Add:
            return VK_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::Min:
            return VK_BLEND_OP_MIN;
        case BlendOp::Max:
            return VK_BLEND_OP_MAX;
        default:
            return VK_BLEND_OP_ADD;
        }
    }

    uint32 GetColorComponentFlags(ColorComponentFlags flags)
    {
        switch (flags)
        {
        case ColorComponentFlags::R:
            return VK_COLOR_COMPONENT_R_BIT;
        case ColorComponentFlags::G:
            return VK_COLOR_COMPONENT_G_BIT;
        case ColorComponentFlags::B:
            return VK_COLOR_COMPONENT_B_BIT;
        case ColorComponentFlags::A:
            return VK_COLOR_COMPONENT_A_BIT;
        case ColorComponentFlags::RG:
            return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT;
        case ColorComponentFlags::RGB:
            return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
        case ColorComponentFlags::RGBA:
            return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        default:
            return VK_COLOR_COMPONENT_R_BIT;
        }
    }

    VkLogicOp GetLogicOp(LogicOp op)
    {
        switch (op)
        {
        case LogicOp::Clear:
            return VK_LOGIC_OP_CLEAR;
        case LogicOp::And:
            return VK_LOGIC_OP_AND;
        case LogicOp::AndReverse:
            return VK_LOGIC_OP_AND_REVERSE;
        case LogicOp::Copy:
            return VK_LOGIC_OP_COPY;
        case LogicOp::AndInverted:
            return VK_LOGIC_OP_AND_INVERTED;
        case LogicOp::NoOp:
            return VK_LOGIC_OP_NO_OP;
        case LogicOp::XOR:
            return VK_LOGIC_OP_XOR;
        case LogicOp::OR:
            return VK_LOGIC_OP_OR;
        case LogicOp::NOR:
            return VK_LOGIC_OP_NOR;
        case LogicOp::Equivalent:
            return VK_LOGIC_OP_EQUIVALENT;
        default:
            return VK_LOGIC_OP_CLEAR;
        }
    }

    VulkanResult GetResult(int32 result)
    {
        VkResult vkRes = static_cast<VkResult>(result);

        switch (vkRes)
        {
        case VK_SUCCESS:
            return VulkanResult::Success;
        case VK_ERROR_OUT_OF_DATE_KHR:
            return VulkanResult::OutOfDateKHR;
        case VK_SUBOPTIMAL_KHR:
            return VulkanResult::SuboptimalKHR;
        default:
            return VulkanResult::Error;
        }
    }
} // namespace Lina::Graphics
