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

#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{
	RenderPassDescription EditorGfxHelpers::GetGUIPassDescription()
	{
		return {
			.buffers		= {{
								   .bufferType	 = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
								   .debugName	 = "RP: GUI - ViewData",
								   .size		 = sizeof(GPUDataGUIView),
								   .stagingOnly	 = true,
								   .bindingIndex = 0,
								   .ident		 = "GUIViewData"_hs,
						   },
							   {
								   .bufferType	 = LinaGX::ResourceTypeHint::TH_StorageBuffer,
								   .debugName	 = "RP: GUI - Materials",
								   .size		 = 50000,
								   .stagingOnly	 = false,
								   .bindingIndex = 1,
								   .ident		 = "GUIMaterials"_hs,
						   },
							   {
								   .bufferType	 = LinaGX::ResourceTypeHint::TH_StorageBuffer,
								   .debugName	 = "RP: GUI - IndirectArguments",
								   .size		 = sizeof(GPUIndirectArgumentsGUI) * 5000,
								   .stagingOnly	 = false,
								   .bindingIndex = 2,
								   .ident		 = "IndirectArguments"_hs,
						   },
							   {
								   .bufferType	 = LinaGX::ResourceTypeHint::TH_IndirectBuffer,
								   .debugName	 = "RP: GUI - IndirectBuffer",
								   .size		 = Application::GetLGX()->GetIndexedIndirectCommandSize() * static_cast<size_t>(5000),
								   .stagingOnly	 = false,
								   .bindingIndex = -1,
								   .ident		 = "IndirectBuffer"_hs,
						   }},
			.setDescription = GetSetDescriptionGUI(),
		};
	}

	LinaGX::DescriptorSetDesc EditorGfxHelpers::GetSetDescriptionGlobal()
	{
		LinaGX::DescriptorBinding globalBinding0 = {
			.descriptorCount = 100,
			.type			 = LinaGX::DescriptorType::SeparateImage,
			.unbounded		 = true,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};
		LinaGX::DescriptorBinding globalBinding1 = {
			.descriptorCount = 100,
			.type			 = LinaGX::DescriptorType::SeparateSampler,
			.unbounded		 = true,
			.stages			 = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {globalBinding0, globalBinding1}};
	}

	LinaGX::DescriptorSetDesc EditorGfxHelpers::GetSetDescriptionGUI()
	{
		LinaGX::DescriptorBinding guiBinding0 = {
			.type	= LinaGX::DescriptorType::UBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding guiBinding1 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding guiBinding2 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {guiBinding0, guiBinding1, guiBinding2}};
	}

	LinaGX::PipelineLayoutDesc EditorGfxHelpers::GetPipelineLayoutDescriptionGUI()
	{
		return {
			.descriptorSetDescriptions = {GetSetDescriptionGlobal(), GetSetDescriptionGUI()},
			.debugName				   = "GUILayout",
		};
	}

	LinaGX::PipelineLayoutDesc EditorGfxHelpers::GetPipelineLayoutDescriptionGlobal()
	{
		return {
			.descriptorSetDescriptions = {GetSetDescriptionGlobal()},
			.debugName				   = "GlobalLayout",
		};
	}
} // namespace Lina::Editor
