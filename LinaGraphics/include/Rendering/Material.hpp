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
#include <set>

namespace Lina::Graphics
{
	class RenderEngine;
	class Texture;
	class Shader;

	struct MaterialSampler2D
	{
		uint32 m_unit = 0;
		Texture* m_boundTexture = nullptr;
		std::string m_path = "";
		std::string m_paramsPath = "";
		TextureBindMode m_bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D;
		bool m_isActive = false;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_unit, m_path, m_paramsPath, m_bindMode, m_isActive);
		}


	};

	class Material
	{

	public:

		static Material& CreateMaterial(Shader& shader, const std::string& path = "");
		static Material& LoadMaterialFromFile(const std::string& path = "");
		static Material& GetMaterial(int id);
		static Material& GetMaterial(const std::string& path);
		static bool MaterialExists(int id);
		static bool MaterialExists(const std::string& path);
		static void UnloadMaterialResource(int id);
		static void LoadMaterialData(Material& mat, const std::string& path);
		static void SaveMaterialData(const Material& mat, const std::string& path);
		static Material& SetMaterialShader(Material& material, Shader& shader, bool onlySetID = false);
		static void SetMaterialContainers(Material& material);
		static void UnloadAll();
		static std::set<Material*>& GetShadowMappedMaterials() { return s_shadowMappedMaterials; }
		static std::set<Material*>& GetHDRIMaterials() { return s_hdriMaterials; }
		static std::map<int, Material>& GetLoadedMaterials() { return s_loadedMaterials; }

		void UpdateMaterialData();
		void PostLoadMaterialData(Lina::Graphics::RenderEngine& renderEngine);
		void SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode = TextureBindMode::BINDTEXTURE_TEXTURE2D);
		void RemoveTexture(const std::string& textureName);
		Texture& GetTexture(const std::string& name);

		void SetFloat(const std::string& name, float value)
		{
			m_floats[name] = value;
		}


		void SetBool(const std::string& name, bool value)
		{
			m_bools[name] = value;
		}

		void SetInt(const std::string& name, int value)
		{
			m_ints[name] = value;

			if (name == MAT_SURFACETYPE)
				m_surfaceType = static_cast<MaterialSurfaceType>(value);
		}

		void SetColor(const std::string& name, const Color& color)
		{
			m_colors[name] = color;
		}

		void SetVector2(const std::string& name, const Vector2& vector)
		{
			m_vector2s[name] = vector;
		}

		void SetVector3(const std::string& name, const Vector3& vector)
		{
			m_vector3s[name] = vector;
		}

		void SetVector4(const std::string& name, const Vector4& vector)
		{
			m_vector4s[name] = vector;
		}

		void SetMatrix4(const std::string& name, const Matrix& matrix)
		{
			m_matrices[name] = matrix;
		}

		float GetFloat(const std::string& name)
		{
			return m_floats[name];
		}

		float GetBool(const std::string& name)
		{
			return m_bools[name];
		}

		int GetInt(const std::string& name)
		{
			return m_ints[name];
		}

		Color GetColor(const std::string& name)
		{
			return m_colors[name];
		}

		Vector2 GetVector2(const std::string& name)
		{
			return m_vector2s[name];
		}

		Vector3 GetVector3(const std::string& name)
		{
			return m_vector3s[name];
		}

		Vector4 GetVector4(const std::string& name)
		{
			return m_vector4s[name];
		}

		Matrix GetMatrix(const std::string& name)
		{
			return m_matrices[name];
		}

		int GetID() const { return m_materialID; }
		const std::string& GetPath() const { return m_path; }
		uint32 GetShaderID() { return m_shaderID; }

		void SetSurfaceType(MaterialSurfaceType type)
		{
			m_surfaceType = type;
			SetInt(MAT_SURFACETYPE, type);
		}

		MaterialSurfaceType GetSurfaceType() { return m_surfaceType; }


		friend class cereal::access;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_usesHDRI, m_isPBR, m_isShadowMapped, m_shaderPath, m_surfaceType, m_sampler2Ds, m_floats, m_ints, m_colors, m_vector2s, m_vector3s, m_vector4s, m_matrices, m_bools);
		}


		std::map<std::string, float> m_floats;
		std::map<std::string, int> m_ints;
		std::map<std::string, MaterialSampler2D> m_sampler2Ds;
		std::map<std::string, Color> m_colors;
		std::map<std::string, Vector2> m_vector2s;
		std::map<std::string, Vector3> m_vector3s;
		std::map<std::string, Vector4> m_vector4s;
		std::map<std::string, Matrix> m_matrices;
		std::map<std::string, bool> m_bools;

		bool m_isPBR = false;
		bool m_usesHDRI = false;
		bool m_isShadowMapped = false;
		std::string m_shaderPath = "";
		uint32 m_shaderID = 0;
		std::string m_path = "";

	private:

		static std::map<int, Material> s_loadedMaterials;
		static std::set<Material*> s_shadowMappedMaterials;
		static std::set<Material*> s_hdriMaterials;

	private:

		friend class RenderEngine;
		friend class RenderContext;

		int m_materialID = -1;
	

		MaterialSurfaceType m_surfaceType = MaterialSurfaceType::Opaque;

	};

	struct ModelMaterial
	{
		std::string m_name = "";
		std::map<std::string, std::string> m_textureNames;
		std::map<std::string, float> m_floats;
		std::map<std::string, Vector3> m_vectors;
		std::map<std::string, Matrix> m_matrices;
	};

}


#endif