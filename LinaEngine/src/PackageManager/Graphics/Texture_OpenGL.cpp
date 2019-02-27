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

#include "LinaPch.hpp"
#include "Texture_OpenGL.hpp"  
#include "glad/glad.h"
#include "Lina/Utility/stb_image.h"

namespace LinaEngine
{



	bool Texture_OpenGL::Load(const std::string& p)
	{
		std::string fullPath = ResourceConstants::TexturesPath + p;
		const char* filePath = fullPath.c_str();

		glGenTextures(1, &m_TextureObj);
		glBindTexture(GL_TEXTURE_2D, m_TextureObj);

		//First two funciton decides how the texture image will be wrapped on the objects surface.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Last two functions are for mipmap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);
		//These declared variables will be defined by stbi_load function with the texture files width, height and number of color
		//channel values.
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

		if (data)
		{
			GLenum format = GL_RGB;

			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			
			//This function genearates texture image at the currently bound texture.
			//2nd parameter is for setting mipmap level manually which is irrelevant right now.
			//6th paramter is 0 for legacy reasons.
			//8th paramter asks what format we store the data which is unsigned char
			//last parameter is for actual image data.
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
			isLoaded = true;
		}
		else
		{
			LINA_CORE_ERR("Texture failed to load at path: {0} {1}", filePath, p);
			stbi_image_free(data);
			isLoaded = false;
			return false;
		}

		return true;
	}

	void Texture_OpenGL::Use()
	{
		if (!isLoaded) return;

		glActiveTexture(m_TextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_TextureObj);
	}
}

