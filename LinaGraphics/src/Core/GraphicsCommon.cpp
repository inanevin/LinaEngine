#include "Core/GraphicsCommon.hpp"
#include <vulkan/vulkan.h>

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
        case Format::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case Format::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        default:
            return VK_FORMAT_B8G8R8A8_SRGB;
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
        default:
            return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        }
    }

    uint32 GetPipelineStageFlags(PipelineStageFlags flags)
    {
        switch (flags)
        {
        case PipelineStageFlags::ColorAttachmentOutput:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case PipelineStageFlags::EarlyFragmentTests:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case PipelineStageFlags::LateFragmentTests:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        default:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
    }

} // namespace Lina::Graphics
