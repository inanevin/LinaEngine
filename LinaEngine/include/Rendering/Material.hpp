/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Material
Timestamp: 4/26/2019 1:12:18 AM

*/

#pragma once

#ifndef Material_HPP
#define Material_HPP


#include "Utility/Math/Matrix.hpp"
#include "Utility/Math/Color.hpp"
#include <map>

namespace LinaEngine::Graphics
{
	class Texture;

	struct MaterialTextureData
	{
		Texture* texture = nullptr;
		uint32 unit = 0;
		BindTextureMode bindMode = BindTextureMode::BINDTEXTURE_TEXTURE2D;
	};

	class Material
	{

	public:
		
		FORCEINLINE void SetTexture(const std::string& textureName, Texture* texture, uint32 textureUnit, BindTextureMode bindMode = BindTextureMode::BINDTEXTURE_TEXTURE2D)
		{
			MaterialTextureData data;
			data.texture = texture;
			data.unit = textureUnit;
			data.bindMode = bindMode;
			textures[textureName] = data;
		}

		FORCEINLINE void SetFloat(const std::string& name, float value)
		{
			floats[name] = value;
		}

		FORCEINLINE void SetInt(const std::string& name, int value)
		{
			ints[name] = value;
		}

		FORCEINLINE void SetColor(const std::string& name, const Color& color)
		{
			colors[name] = color;
		}

		FORCEINLINE void SetVector2(const std::string& name, const Vector2F& vector)
		{
			vector2s[name] = vector;
		}

		FORCEINLINE void SetVector3(const std::string& name, const Vector3F& vector)
		{
			vector3s[name] = vector;
		}

		FORCEINLINE void SetVector4(const std::string& name, const Vector4F& vector)
		{
			vector4s[name] = vector;
		}

		FORCEINLINE void SetMatrix4(const std::string& name, const Matrix& matrix)
		{
			matrices[name] = matrix;
		}

		FORCEINLINE Texture* GetTexture(const std::string& name)
		{
			return textures[name].texture;
		}

		FORCEINLINE float GetFloat(const std::string& name)
		{
			return floats[name];
		}

		FORCEINLINE int GetInt(const std::string& name)
		{
			return ints[name];
		}

		FORCEINLINE Color GetColor(const std::string& name)
		{
			return colors[name];
		}

		FORCEINLINE Vector2F GetVector2(const std::string& name)
		{
			return vector2s[name];
		}

		FORCEINLINE Vector3F GetVector3(const std::string& name)
		{
			return vector3s[name];
		}

		FORCEINLINE Vector4F GetVector4(const std::string& name)
		{
			return vector4s[name];
		}

		FORCEINLINE Matrix GetMatrix(const std::string& name)
		{
			return matrices[name];
		}

		FORCEINLINE uint32 GetShaderID() { return shaderID; }
	private:

		friend class RenderEngine;
		friend class RenderContext;

		uint32 shaderID;
		std::map<std::string, MaterialTextureData> textures;
		std::map<std::string, float> floats;
		std::map<std::string, int> ints;
		std::map<std::string, Color> colors;
		std::map<std::string, Vector2F> vector2s;
		std::map<std::string, Vector3F> vector3s;
		std::map<std::string, Vector4F> vector4s;
		std::map<std::string, Matrix> matrices;;

	
	};

	struct ModelMaterial
	{
		std::map<std::string, std::string> textureNames;
		std::map<std::string, float> floats;
		std::map<std::string, Vector3F> vectors;
		std::map<std::string, Matrix> matrices;
	};

}


#endif