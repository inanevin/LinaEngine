/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Lighting
Timestamp: 5/13/2019 12:52:02 AM

*/

#pragma once

#ifndef Lighting_HPP
#define Lighting_HPP

#include "Utility/Math/Color.hpp"

namespace LinaEngine::Graphics
{

	class BaseLight
	{
	public: 

		BaseLight(LinaEngine::Color color = LinaEngine::Color(1,1,1,1), float intensity = 1.0f) : m_Color(color), m_Intensity(intensity) {};

		FORCEINLINE LinaEngine::Color GetColor()
		{
			return m_Color;
		}

		FORCEINLINE Vector3F GetColorVector()
		{
			return Vector3F(m_Color.R(), m_Color.G(), m_Color.B());
		}

		FORCEINLINE float GetIntensity() const
		{
			return m_Intensity;
		}

		FORCEINLINE void SetColor(LinaEngine::Color newColor)
		{
			m_Color = newColor;
		}

		FORCEINLINE void SetIntensity(float newIntensity)
		{
			m_Intensity = newIntensity;
		}
	private:

		LinaEngine::Color m_Color = LinaEngine::Color(1, 1, 1, 1);
		float m_Intensity = 0.0f;

	};

	class AmbientLight : public BaseLight
	{
	public:
		AmbientLight(LinaEngine::Color color = LinaEngine::Color(1, 1, 1, 1), float intensity = 1.0f) : BaseLight(color, intensity) {};
	};

	class DirectionalLight : public BaseLight
	{

	};
}


#endif