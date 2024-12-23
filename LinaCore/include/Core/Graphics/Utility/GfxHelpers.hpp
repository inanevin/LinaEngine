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

#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Math/Matrix.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"

namespace Lina
{
	class GfxHelpers
	{
	public:
		static LinaGX::Instance* InitializeLinaGX();
		static void				 InitializeLinaVG();
		static void				 ShutdownLinaVG();

		static LinaGX::DescriptorSetDesc GetSetDescPersistentGlobal();
		static LinaGX::DescriptorSetDesc GetSetDescPersistentRenderPass(RenderPassType renderpassType);

		static LinaGX::TextureBarrier GetTextureBarrierPresent2Color(uint32 texture, bool isSwapchain);
		static LinaGX::TextureBarrier GetTextureBarrierColor2Present(uint32 texture, bool isSwapchain);

		static LinaGX::TextureBarrier GetTextureBarrierColorAtt2Read(uint32 texture);
		static LinaGX::TextureBarrier GetTextureBarrierColorRead2Att(uint32 texture);

		static LinaGX::TextureBarrier GetTextureBarrierDepthAtt2Read(uint32 texture);
		static LinaGX::TextureBarrier GetTextureBarrierDepthRead2Att(uint32 texture);

		static LinaGX::TextureBarrier GetTextureBarrierUndef2TransferDest(uint32 texture);
		static LinaGX::TextureBarrier GetTextureBarrierTransferDest2Sampled(uint32 texture);

		static LinaGX::PipelineLayoutDesc GetPLDescPersistentGlobal();
		static LinaGX::PipelineLayoutDesc GetPLDescPersistentRenderPass(RenderPassType renderpassType);

		static LinaGX::DescriptorBinding GetBindingFromShaderBinding(const LinaGX::ShaderDescriptorSetBinding& b);

		static RenderPassDescription GetRenderPassDescription(RenderPassType type);

		static Matrix4 GetProjectionFromSize(const Vector2ui& size);
	};
} // namespace Lina
