/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Texture_OpenGL
Timestamp: 2/27/2019 1:44:21 PM

*/

#pragma once

#ifndef Texture_OpenGL_HPP
#define Texture_OpenGL_HPP



namespace LinaEngine
{
	class Texture_OpenGL
	{
	public:


		Texture_OpenGL(unsigned int textureUnit = 0, bool loadImmediate = false, const std::string& fileName = "")
		{
			m_TextureUnit = textureUnit;

			if (loadImmediate)
				Load(fileName);
		};

		bool Load(const std::string& fileName);
		void Use();

	private:

		bool isLoaded;
		bool isRGBA;
		unsigned int m_TextureUnit;
		unsigned int m_TextureObj;
	};
}


#endif