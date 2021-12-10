/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: Sampler

Represents a data structure for texture samplers created in the GPU backend.

Timestamp: 4/26/2019 6:20:27 PM
*/

#pragma once

#ifndef Sampler_HPP
#define Sampler_HPP

#include "PackageManager/PAMRenderDevice.hpp"

namespace Lina::Graphics
{
	class Sampler
	{
	public:

		Sampler() {};

		// Destructor releases sampler data through render engine
		~Sampler()
		{
			m_engineBoundID = s_renderDevice->ReleaseSampler(m_engineBoundID);
		}

		void Construct(RenderDevice& deviceIn, SamplerParameters samplerParams, TextureBindMode bindMode, bool isCubemap = false)
		{
			s_renderDevice = &deviceIn;
			m_engineBoundID = s_renderDevice->CreateSampler(samplerParams, isCubemap);
			m_targetBindMode = bindMode;
			m_params = samplerParams;
		}

		void UpdateSettings(SamplerParameters samplerParams) 
		{
			m_params = samplerParams;
			s_renderDevice->UpdateSamplerParameters(m_engineBoundID, samplerParams);
			s_renderDevice->UpdateTextureParameters(m_targetBindMode, m_targetTextureID, samplerParams);
		}

		SamplerParameters& GetSamplerParameters() { return m_params; }

		uint32 GetID() const { return m_engineBoundID; }
		void SetTargetTextureID(uint32 id) { m_targetTextureID = id; }
	private:

		uint32 m_targetTextureID = 0;
		TextureBindMode m_targetBindMode;
		SamplerParameters m_params;
		RenderDevice* s_renderDevice = nullptr;
		uint32 m_engineBoundID = 0;


	};
}


#endif