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

		Sampler(){};

		FORCEINLINE void Construct(PAMRenderDevice& deviceIn, SamplerFilter minFilter = SamplerFilter::FILTER_NEAREST_MIPMAP_LINEAR, SamplerFilter magFilter = SamplerFilter::FILTER_LINEAR,
			SamplerWrapMode wrapU = SamplerWrapMode::WRAP_CLAMP, SamplerWrapMode wrapV = SamplerWrapMode::WRAP_CLAMP, float anisotropy = 0.0f)
		{
			renderDevice = &deviceIn;
			m_EngineBoundID = renderDevice->CreateSampler(minFilter, magFilter, wrapU, wrapV, anisotropy);
		}

		// Destructor releases sampler data through render engine
		FORCEINLINE ~Sampler()
		{
			m_EngineBoundID = renderDevice->ReleaseSampler(m_EngineBoundID);
		}

		FORCEINLINE uint32 GetID() { return m_EngineBoundID; }

	private:

		PAMRenderDevice* renderDevice = nullptr;
		uint32 m_EngineBoundID = 0;

		NULL_COPY_AND_ASSIGN(Sampler);

	};
}


#endif