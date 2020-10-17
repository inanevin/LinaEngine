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
Class: Material

Representation of a material. Materials are used by mesh renderers and hold all the shader
related data and data structures.

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