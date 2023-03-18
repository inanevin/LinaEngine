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

#ifndef CommonGraphics_HPP
#define CommonGraphics_HPP

#include "Data/Vector.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Core/StringID.hpp"
#include "Data/Functional.hpp"
#include "Math/Color.hpp"

namespace Lina
{
	class Texture;
	class IGfxBufferResource;

	enum class Format
	{
		UNDEFINED = 0,
		B8G8R8A8_SRGB,
		B8G8R8A8_UNORM,
		R32G32B32_SFLOAT,
		R32G32B32A32_SFLOAT,
		R32G32_SFLOAT,
		D32_SFLOAT,
		R8G8B8A8_UNORM,
		R8G8B8A8_SRGB,
		R8G8_UNORM,
		R8_UNORM,
		R8_UINT,
		R16_SFLOAT,
		R16_SINT,
		R32_SFLOAT,
		R32_SINT,
		FORMAT_MAX,
	};

	enum class Filter
	{
		Anisotropic,
		Nearest,
		Linear,
	};

	enum class SamplerAddressMode
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge
	};

	enum class ColorSpace
	{
		SRGB_NONLINEAR,
	};

	enum class PresentMode
	{
		Immediate,
		Mailbox,
		FIFO,
		FIFORelaxed,
	};

	enum class QueueFamilies
	{
		Graphics,
		Compute,
		Transfer,
		SparseBinding,
		Protected,
	};

	enum class LoadOp
	{
		Load,
		Clear,
		DontCare,
		None,
	};

	enum class StoreOp
	{
		Store,
		DontCare,
		None,
	};

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

	enum class ShaderStage
	{
		Vertex,
		TesellationControl,
		TesellationEval,
		Geometry,
		Fragment,
		Compute,
	};

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

	enum class PolygonMode
	{
		Fill,
		Line,
		Point,
		FillRect,
	};

	enum class CullMode
	{
		None,
		Front,
		Back,
		FrontAndBack,
	};

	enum class FrontFace
	{
		ClockWise,
		AntiClockWise
	};

	enum class VertexInputRate
	{
		Vertex,
		Instance
	};

	enum class IndexType
	{
		Uint16,
		Uint32,
		Uint8_Ext,
		None,
	};

	enum class BorderColor
	{
		FloatTransparentBlack,
		FloatOpaqueWhite,
		FloatOpaqueBlack,
	};

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

	enum class BlendOp
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

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

	enum class ResolveMode
	{
		None,
		SampleZero,
		Average,
		Min,
		Max,
	};

	enum class MipmapMode
	{
		Nearest,
		Linear
	};

	enum class MipmapFilter
	{
		Default = 0,
		Box,
		Triangle,
		CubicSpline,
		CatmullRom,
		Mitchell
	};

	enum class SharingMode
	{
		Exclusive,
		Concurrent,
	};

	enum class MaterialPropertyType
	{
		Float = 0,
		Int,
		Bool,
		Vector2,
		Vector2i,
		Vector4,
		Mat4,
		Texture,
	};

	enum class PipelineType
	{
		Standard = 0,
		NoVertex,
		GUI,
	};

	enum class DescriptorType
	{
		UniformBuffer,
		UniformBufferDynamic,
		CombinedImageSampler,
		StorageBuffer,
	};

	enum class ImageTiling
	{
		Optimal,
		Linear
	};

	enum class CommandType
	{
		Graphics,
		Compute,
		Transfer
	};

	enum class ResourceState
	{
		CopyDestination,
		VertexBuffer,
		IndexBuffer,
		UniformBuffer,
		GenericRead,
		NonPixelShaderResource,
		PixelShaderResource,
	};

	enum class BufferResourceType
	{
		UniformBuffer,
		IndirectBuffer,
		Staging,
		GPUDest,
	};

	enum class TextureResourceType
	{
		Texture2DSwapchain,
		Texture2DDefault,
		Texture2DRenderTargetColor,
		Texture2DRenderTargetDepthStencil,
	};

	enum MaterialBindFlag
	{
		MBF_BindShader			   = 1 << 0,
		MBF_BindMaterialProperties = 1 << 1,
	};

	enum class ResourceTransitionType
	{
		SRV2RT,
		RT2SRV,
		RT2Present,
		Present2RT,
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

	struct Viewport
	{
		float x		   = 0.0f;
		float y		   = 0.0f;
		float width	   = 0.0f;
		float height   = 0.0f;
		float minDepth = 0.0f;
		float maxDepth = 0.0f;
	};

	struct VertexInputAttribute
	{
		uint32 binding	= 0;
		uint32 location = 0;
		Format format	= Format::R32G32B32_SFLOAT;
		uint32 offset	= 0;
	};

	struct SamplerData
	{
		Filter			   minFilter   = Filter::Linear;
		Filter			   magFilter   = Filter::Linear;
		SamplerAddressMode mode		   = SamplerAddressMode::ClampToEdge;
		uint32			   anisotropy  = 4;
		float			   minLod	   = 0.0f;
		float			   maxLod	   = 1.0f;
		float			   mipLodBias  = 0.0f;
		Color			   borderColor = Color::Black;
	};

	struct UserBinding
	{
		uint32				binding			= 0;
		uint32				descriptorCount = 0;
		DescriptorType		type			= DescriptorType::UniformBuffer;
		Vector<ShaderStage> stages;
	};

	struct ShaderByteCode
	{
		uint8* data		= nullptr;
		uint32 dataSize = 0;
	};

	struct ImageGenerateRequest
	{
		TextureResourceType type;
		Delegate<void()>	onGenerated;
	};

	struct ResourceTransition
	{
		ResourceTransitionType type	   = ResourceTransitionType::Present2RT;
		Texture*			   texture = nullptr;
		IGfxBufferResource*	   buffer  = nullptr;
	};

	class GfxCommand
	{
	public:
		Delegate<void(uint32 cmdList)> Record;
		Delegate<void()>			   OnRecorded;
		Delegate<void()>			   OnSubmitted;
	};

#define DEFAULT_DEPTH_FORMAT	 Format::D32_SFLOAT
#define DEFAULT_SWAPCHAIN_FORMAT Format::R8G8B8A8_UNORM
#define DEFAULT_COLOR_FORMAT	 Format::R32G32B32A32_SFLOAT
#define LINA_MAIN_SWAPCHAIN		 "LinaMainSwapchain"_hs
#define DEFAULT_SAMPLER_SID		 "Resource/Core/Samplers/DefaultSampler.linasampler"_hs
#define DEFAULT_TEXTURE_SID		 "Resources/Core/Textures/LogoWithText.png"_hs
#define DEFAULT_CLEAR_CLR		 Color::Black
} // namespace Lina

#endif