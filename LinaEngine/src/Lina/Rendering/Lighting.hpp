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
Timestamp: 2/19/2019 12:35:56 PM

*/

#pragma once

#ifndef Lighting_HPP
#define Lighting_HPP

#include "Lina/Utility/Math/Color.hpp"


namespace LinaEngine
{

	class BaseLight
	{
	public:

		std::string Name;
		Color color;
		float AmbientIntensity;
		float DiffuseIntensity;

		BaseLight()
		{
			color = Color(0.0f, 0.0f, 0.0f);
			AmbientIntensity = 0.0f;
			DiffuseIntensity = 0.0f;
		}

		//virtual void AddToATB(TwBar *bar);
	};


	class DirectionalLight : public BaseLight
	{
	public:
		Vector3F Direction;

		DirectionalLight()
		{
			Direction = Vector3F(0.0f, 0.0f, 0.0f);
		}

		//virtual void AddToATB(TwBar *bar);
	};


	struct LightAttenuation
	{
		float Constant;
		float Linear;
		float Exp;

		LightAttenuation()
		{
			Constant = 1.0f;
			Linear = 0.0f;
			Exp = 0.0f;
		}
	};


	class PointLight : public BaseLight
	{
	public:
		Vector3F Position;
		LightAttenuation Attenuation;

		PointLight()
		{
			Position = Vector3F(0.0f, 0.0f, 0.0f);
		}

	//	virtual void AddToATB(TwBar *bar);
	};


	class SpotLight : public PointLight
	{
	public:
		Vector3F Direction;
		float Cutoff;

		SpotLight()
		{
			Direction = Vector3F(0.0f, 0.0f, 0.0f);
			Cutoff = 0.0f;
		}

		//virtual void AddToATB(TwBar *bar);
	};

}


#endif