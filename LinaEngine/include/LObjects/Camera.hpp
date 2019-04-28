/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Camera
Timestamp: 1/5/2019 9:51:42 PM

*/

#pragma once

#ifndef Camera_HPP
#define Camera_HPP

#include "Core/APIExport.hpp"
#include "Core/Transform.hpp"


namespace LinaEngine
{
	struct PerspectiveInformation
	{
		float FOV;
		float width;
		float height;
		float zNear;
		float zFar;

		PerspectiveInformation()
		{
			FOV = 90.0f;
			width = 1920;
			height = 1080;
			zNear = 0.1f;
			zFar = 1000.0f;
		}

		PerspectiveInformation(float fieldOfView, float w, float h, float zn, float zf)
		{
			FOV = fieldOfView;
			width = w;
			height = h;
			zNear = zn;
			zFar = zf;
		}

		PerspectiveInformation(const PerspectiveInformation& p)
		{
			this->FOV = p.FOV;
			this->width = p.width;
			this->height = p.height;
			this->zNear = p.zNear;
			this->zFar = p.zFar;
		}
	};

	
	class Camera
	{
	public:

		

		Camera(PerspectiveInformation p = PerspectiveInformation());
		
		//Matrix4F GetViewProjection();
		//inline Matrix4F GetPerspectiveProjection() { return m_PerspectiveProjection; };
		//void SetPerspectiveInformation(PerspectiveInformation p);

		//void OnInput(InputEngine& i);
		inline PerspectiveInformation GetPerspectiveInformation() { return m_PersInfo; }
		
		Transform m_Transform;

	private:

		
		//Matrix4F m_PerspectiveProjection;
		//Matrix4F m_ViewProjection;
		PerspectiveInformation m_PersInfo;
		Vector2F m_windowCenter;
		bool     m_mouseLocked;
	
	};
}


#endif