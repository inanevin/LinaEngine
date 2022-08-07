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

    VkCommandPoolCreateFlagBits GetCommandPoolCreateFlags(CommandPoolFlags f)
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

    VkQueueFlagBits GetQueueFamilyBit(QueueFamilies f)
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

    VkFenceCreateFlagBits GetFenceFlags(FenceFlags f)
    {
        switch (f)
        {
        case FenceFlags::Signaled:
            return VK_FENCE_CREATE_SIGNALED_BIT;
        default:
            return VK_FENCE_CREATE_SIGNALED_BIT;
        }
    }

    VkCommandBufferUsageFlagBits GetCommandBufferFlags(CommandBufferFlags f)
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

    VkShaderStageFlagBits GetShaderStage(ShaderStage s)
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
} // namespace Lina::Graphics
