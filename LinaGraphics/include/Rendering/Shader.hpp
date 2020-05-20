/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Shader
Timestamp: 2/16/2019 1:47:28 AM

*/

#pragma once

#ifndef Shader_HPP
#define Shader_HPP

#include "Core/Common.hpp"
#include "PackageManager/PAMRenderDevice.hpp"
#include "UniformBuffer.hpp"
#include <string>

namespace LinaEngine::Graphics
{

	class Shader
	{
	public:

		Shader() {};

		FORCEINLINE ~Shader() { m_EngineBoundID = renderDevice->ReleaseShaderProgram(m_EngineBoundID); }

		FORCEINLINE Shader& Construct(RenderDevice& renderDeviceIn, const std::string& text, bool usesGeometryShader)
		{
			renderDevice = &renderDeviceIn;
			m_EngineBoundID = renderDevice->CreateShaderProgram(text, usesGeometryShader);
			return *this;
		}

		// Set uniform buffer through render engine.
		FORCEINLINE void SetUniformBuffer(const std::string& name, UniformBuffer& buffer) 
		{ 
			renderDevice->SetShaderUniformBuffer(m_EngineBoundID, name, buffer.GetID()); 
		}

		FORCEINLINE void BindBlockToBuffer(uint32 bindingPoint, std::string blockName)
		{
			renderDevice->BindShaderBlockToBufferPoint(m_EngineBoundID, bindingPoint, blockName);
		}

		// Get shader id, this gets matched w/ program id on render engine.
		FORCEINLINE uint32 GetID() { return m_EngineBoundID; }


	private:

		RenderDevice* renderDevice = nullptr;
		uint32 m_EngineBoundID;

		//NULL_COPY_AND_ASSIGN(Shader);
	
	};
}


#endif