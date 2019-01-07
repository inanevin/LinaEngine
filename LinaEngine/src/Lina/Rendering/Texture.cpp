/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Texture
Timestamp: 1/7/2019 1:55:47 PM

*/

#include "LinaPch.hpp"
#include "Texture.hpp"  
#include "Lina/Utility/stb_image.h"

namespace LinaEngine
{
	Texture::Texture(GLenum TextureTarget)
	{
		m_TextureTarget = TextureTarget;
	}

	bool Texture::Load(const std::string& fullPath)
	{
		const char* filePath = fullPath.c_str();

		glGenTextures(1, &m_TextureObj);

		//These declared variables will be defined by stbi_load function with the texture files width, height and number of color
		//channel values.
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
		
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glBindTexture(m_TextureTarget, m_TextureObj);
			//This function genearates texture image at the currently bound texture.
			//2nd parameter is for setting mipmap level manually which is irrelevant right now.
			//6th paramter is 0 for legacy reasons.
			//8th paramter asks what format we store the data which is unsigned char
			//last parameter is for actual image data.
			glTexImage2D(m_TextureTarget, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//First two funciton decides how the texture image will be wrapped on the objects surface.
			glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//Last two functions are for mipmap
			glTexParameteri(m_TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(m_TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			LINA_CORE_ERR("Texture failed to load at path: {0}", filePath);
			stbi_image_free(data);
			return false;
		}

		return true;
	}


	void Texture::Bind(GLenum textureUnit)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_TextureObj);
	}
}

