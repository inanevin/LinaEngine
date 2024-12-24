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
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{
	RenderPassDescription EditorGfxHelpers::GetGUIPassDescription()
	{
		return {
			.buffers =
				{
					{
						.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
						.debugName	  = "RP: GUI - ViewData",
						.size		  = sizeof(GPUDataGUIView),
						.stagingOnly  = true,
						.bindingIndex = 0,
						.ident		  = "GUIViewData"_hs,
					},
					{
						.bufferType	  = LinaGX::ResourceTypeHint::TH_StorageBuffer,
						.debugName	  = "RP: GUI - Materials",
						.size		  = 10000,
						.stagingOnly  = false,
						.bindingIndex = 1,
						.ident		  = "GUIMaterials"_hs,
					},
				},
			.setDescription = GetSetDescriptionGUI(),
		};
	}

	RenderPassDescription EditorGfxHelpers::GetEntityBufferPassDescription()
	{
		LinaGX::DescriptorSetDesc setDesc = GetSetDescriptionEntityBufferPass();

		return {
			.buffers =
				{
					{
						.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
						.debugName	  = "EntityBuffer Pass - ViewData",
						.size		  = sizeof(GPUDataView),
						.stagingOnly  = true,
						.bindingIndex = 0,
						.ident		  = "ViewData"_hs,
					},
				},
			.setDescription = setDesc,
		};
	}

	RenderPassDescription EditorGfxHelpers::GetEditorWorldPassDescription()
	{
		LinaGX::DescriptorSetDesc setDesc = GetSetDescriptionEditorWorldPass();

		return {
			.buffers =
				{
					{
						.bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
						.debugName	  = "Editor World Pass - ViewData",
						.size		  = sizeof(EditorWorldPassViewData),
						.stagingOnly  = true,
						.bindingIndex = 0,
						.ident		  = "ViewData"_hs,
					},

				},
			.setDescription = setDesc,
		};
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

		return {.bindings = {guiBinding0, guiBinding1}};
	}

	LinaGX::DescriptorSetDesc EditorGfxHelpers::GetSetDescriptionEntityBufferPass()
	{
		LinaGX::DescriptorBinding binding0 = {
			.type	= LinaGX::DescriptorType::UBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding1 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding2 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding3 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {binding0, binding1, binding2, binding3}};
	}

	LinaGX::DescriptorSetDesc EditorGfxHelpers::GetSetDescriptionEditorWorldPass()
	{
		LinaGX::DescriptorBinding binding0 = {
			.type	= LinaGX::DescriptorType::UBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding1 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		LinaGX::DescriptorBinding binding2 = {
			.type	= LinaGX::DescriptorType::SSBO,
			.stages = {LinaGX::ShaderStage::Vertex, LinaGX::ShaderStage::Fragment},
		};

		return {.bindings = {binding0, binding1, binding2}};
	}

	LinaGX::PipelineLayoutDesc EditorGfxHelpers::GetPipelineLayoutDescriptionGlobal()
	{
		return {
			.descriptorSetDescriptions = {GfxHelpers::GetSetDescPersistentGlobal()},
			.constantRanges			   = {{
						   .stages = {LinaGX::ShaderStage::Fragment},
						   .size   = static_cast<uint32>(sizeof(GPUEditorGUIPushConstants)),
			   }},
			.debugName				   = "GUILayout",
		};
	}

	LinaGX::PipelineLayoutDesc EditorGfxHelpers::GetPipelineLayoutDescriptionGUI()
	{
		return {
			.descriptorSetDescriptions = {GfxHelpers::GetSetDescPersistentGlobal(), GetSetDescriptionGUI()},
			.constantRanges			   = {{
						   .stages = {LinaGX::ShaderStage::Fragment},
						   .size   = static_cast<uint32>(sizeof(GPUEditorGUIPushConstants)),
			   }},
			.debugName				   = "GUILayout",
		};
	}

	LinaGX::PipelineLayoutDesc EditorGfxHelpers::GetPipelineLayoutDescriptionEntityBufferPass()
	{
		LinaGX::PipelineLayoutDesc desc;
		desc.descriptorSetDescriptions = {GfxHelpers::GetSetDescPersistentGlobal(), GetSetDescriptionEntityBufferPass()};
		desc.debugName				   = "PL Editor EntityBuffer Pass";
		desc.constantRanges.push_back(LinaGX::PipelineLayoutPushConstantRange{.stages = {LinaGX::ShaderStage::Vertex}, .size = sizeof(uint32)});
		return desc;
	}

	LinaGX::PipelineLayoutDesc EditorGfxHelpers::GetPipelineLayoutDescriptionEditorWorldPass()
	{
		LinaGX::PipelineLayoutDesc desc;
		desc.descriptorSetDescriptions = {GfxHelpers::GetSetDescPersistentGlobal(), GetSetDescriptionEditorWorldPass()};
		desc.debugName				   = "PL Editor World Pass";
		desc.constantRanges.push_back(LinaGX::PipelineLayoutPushConstantRange{.stages = {LinaGX::ShaderStage::Vertex}, .size = sizeof(uint32)});
		return desc;
	}

} // namespace Lina::Editor
