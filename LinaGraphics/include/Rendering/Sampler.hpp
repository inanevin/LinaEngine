/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Sampler
Timestamp: 4/26/2019 6:20:27 PM

*/

#pragma once

#ifndef Sampler_HPP
#define Sampler_HPP

#include "PackageManager/PAMRenderDevice.hpp"

namespace LinaEngine::Graphics
{
	class Sampler
	{
	public:

		Sampler() {};

		// Destructor releases sampler data through render engine
		FORCEINLINE ~Sampler()
		{
			m_EngineBoundID = renderDevice->ReleaseSampler(m_EngineBoundID);
		}

		FORCEINLINE void Construct(RenderDevice& deviceIn, SamplerParameters samplerParams, TextureBindMode bindMode)
		{
			renderDevice = &deviceIn;
			m_EngineBoundID = renderDevice->CreateSampler(samplerParams);
			m_TargetBindMode = bindMode;
			m_Params = samplerParams;
		}

		FORCEINLINE void UpdateSettings(SamplerParameters samplerParams) 
		{
			m_Params = samplerParams;
			renderDevice->UpdateSamplerParameters(m_EngineBoundID, samplerParams);
			renderDevice->UpdateTextureParameters(m_TargetBindMode, m_TargetTextureID, samplerParams);
		}

		FORCEINLINE SamplerParameters& GetSamplerParameters() { return m_Params; }

		FORCEINLINE uint32 GetID() const { return m_EngineBoundID; }
		FORCEINLINE void SetTargetTextureID(uint32 id) { m_TargetTextureID = id; }
	private:

		uint32 m_TargetTextureID;
		TextureBindMode m_TargetBindMode;
		SamplerParameters m_Params;
		RenderDevice* renderDevice;
		uint32 m_EngineBoundID;


	};
}


#endif