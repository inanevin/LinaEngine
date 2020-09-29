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
#include "Rendering/RenderConstants.hpp"
#include "Rendering/RenderingCommon.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>

namespace LinaEngine::Graphics
{

	class Texture;

	struct MaterialTextureData
	{
		Texture* texture = nullptr;
		uint32 unit = 0;
		TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
	};

	struct MaterialSampler2D
	{
		uint32 unit = 0;
		Texture* boundTexture = nullptr;
		TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		bool isActive = false;
	};

	class Material
	{

	public:

		void SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D);
		void RemoveTexture(const std::string& textureName);
		Texture* GetTexture(const std::string& name);


		FORCEINLINE void SetFloat(const std::string& name, float value)
		{
			floats[name] = value;
		}


		FORCEINLINE void SetBool(const std::string& name, bool value)
		{
			booleans[name] = value;
		}

		FORCEINLINE void SetInt(const std::string& name, int value)
		{
			ints[name] = value;

			if (name == MAT_SURFACETYPE)
				m_SurfaceType = static_cast<MaterialSurfaceType>(value);
		}

		FORCEINLINE void SetColor(const std::string& name, const Color& color)
		{
			colors[name] = color;
		}

		FORCEINLINE void SetVector2(const std::string& name, const Vector2& vector)
		{
			vector2s[name] = vector;
		}

		FORCEINLINE void SetVector3(const std::string& name, const Vector3& vector)
		{
			vector3s[name] = vector;
		}

		FORCEINLINE void SetVector4(const std::string& name, const Vector4& vector)
		{
			vector4s[name] = vector;
		}

		FORCEINLINE void SetMatrix4(const std::string& name, const Matrix& matrix)
		{
			matrices[name] = matrix;
		}	

		FORCEINLINE float GetFloat(const std::string& name)
		{
			return floats[name];
		}

		FORCEINLINE float GetBool(const std::string& name)
		{
			return booleans[name];
		}

		FORCEINLINE int GetInt(const std::string& name)
		{
			return ints[name];
		}

		FORCEINLINE Color GetColor(const std::string& name)
		{
			return colors[name];
		}

		FORCEINLINE Vector2 GetVector2(const std::string& name)
		{
			return vector2s[name];
		}

		FORCEINLINE Vector3 GetVector3(const std::string& name)
		{
			return vector3s[name];
		}

		FORCEINLINE Vector4 GetVector4(const std::string& name)
		{
			return vector4s[name];
		}

		FORCEINLINE Matrix GetMatrix(const std::string& name)
		{
			return matrices[name];
		}
		
		FORCEINLINE Shaders GetShaderType() { return m_ShaderType; }
		FORCEINLINE uint32 GetShaderID() { return shaderID; }
		FORCEINLINE void SetSurfaceType(MaterialSurfaceType type) { m_SurfaceType = type; SetInt(MAT_SURFACETYPE, type); }
		FORCEINLINE MaterialSurfaceType GetSurfaceType() { return m_SurfaceType; }


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(usesHDRI, receivesLighting, isShadowMapped, m_ShaderType, m_SurfaceType, floats, ints, colors, vector2s, vector3s, vector4s, matrices, booleans);
		}

		int m_MaterialID = -1;

	private:

		friend class RenderEngine;
		friend class RenderContext;

		uint32 shaderID;
		bool usesHDRI = false;
		bool receivesLighting = false;
		bool isShadowMapped = false;
		Shaders m_ShaderType;
		MaterialSurfaceType m_SurfaceType = MaterialSurfaceType::Opaque;
		std::map<std::string, float> floats;
		std::map<std::string, int> ints;
		std::map<std::string, MaterialSampler2D> sampler2Ds;
		std::map<std::string, Color> colors;
		std::map<std::string, Vector2> vector2s;
		std::map<std::string, Vector3> vector3s;
		std::map<std::string, Vector4> vector4s;
		std::map<std::string, Matrix> matrices;;
		std::map<std::string, bool> booleans;

	
	};

	struct ModelMaterial
	{
		std::map<std::string, std::string> textureNames;
		std::map<std::string, float> floats;
		std::map<std::string, Vector3> vectors;
		std::map<std::string, Matrix> matrices;
	};

}


#endif