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

#include "Texture.hpp"
#include "Sampler.hpp"
#include "UniformBuffer.hpp"

namespace LinaEngine::Graphics
{
	class Shader
	{
	public:

		Shader() {};

		FORCEINLINE ~Shader() { m_EngineBoundID = renderDevice->ReleaseShaderProgram(m_EngineBoundID); }

		FORCEINLINE void Construct(PAMRenderDevice& renderDeviceIn, const LinaString& text)
		{
			renderDevice = &renderDeviceIn;
			m_EngineBoundID = renderDevice->CreateShaderProgram(text);
		}

		// Set uniform buffer through render engine.
		FORCEINLINE void SetUniformBuffer(const LinaString& name, UniformBuffer& buffer) 
		{ 
			renderDevice->SetShaderUniformBuffer(m_EngineBoundID, name, buffer.GetID()); 
		}

		// Set the texture sampler through render engine.
		FORCEINLINE void SetSampler(const LinaString& name, Texture& texture, Sampler& sampler, uint32 unit)
		{ 
			renderDevice->SetShaderSampler(m_EngineBoundID, name, texture.GetID(), sampler.GetID(), unit); 
		}

		// Get shader id, this gets matched w/ program id on render engine.
		FORCEINLINE uint32 GetID() { return m_EngineBoundID; }

	private:
		PAMRenderDevice* renderDevice;
		uint32 m_EngineBoundID;

	
		//NULL_COPY_AND_ASSIGN(Shader);
	
	};
}


#endif