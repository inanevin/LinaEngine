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

#include "Rendering/Material.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Utility/UtilityFunctions.hpp"
#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <fstream>

namespace LinaEngine::Graphics
{

	std::map<int, Material> Material::s_loadedMaterials;
	std::set<Material*> Material::s_shadowMappedMaterials;
	std::set<Material*> Material::s_hdriMaterials;

	void Material::PostLoadMaterialData(LinaEngine::Graphics::RenderEngine& renderEngine)
	{
		for (std::map<std::string, MaterialSampler2D>::iterator it = m_sampler2Ds.begin(); it != m_sampler2Ds.end(); ++it)
		{
			if (Texture::TextureExists(it->second.m_path))
			{
				SetTexture(it->first, &Texture::GetTexture(it->second.m_path), it->second.m_bindMode);
			}
		}

		m_shaderID = Shader::GetShader(m_shaderType).GetID();
	}


	void Material::LoadMaterialData(Material& mat, const std::string& path)
	{
		std::ifstream stream(path);
		{
			cereal::BinaryInputArchive iarchive(stream);

			// Read the data into it.
			iarchive(mat);
		}
	}

	void Material::SaveMaterialData(const Material& mat, const std::string& path)
	{
		std::ofstream stream(path);
		{
			cereal::BinaryOutputArchive oarchive(stream); // Create an output archive

			oarchive(mat); // Write the data to the archive
		}
	}


	void Material::SetTexture(const std::string& textureName, Texture* texture, TextureBindMode bindMode)
	{
		if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
		{
			m_sampler2Ds[textureName].m_boundTexture = texture;
			m_sampler2Ds[textureName].m_bindMode = bindMode;
			m_sampler2Ds[textureName].m_isActive = texture == nullptr ? false : true;

			if (texture != nullptr)
			{
				m_sampler2Ds[textureName].m_path = texture->GetPath();
				m_sampler2Ds[textureName].m_paramsPath = texture->GetParamsPath();
			}		
		}
		else
		{
			LINA_CORE_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
			return;
		}
	}
	void Material::RemoveTexture(const std::string& textureName)
	{
		if (!(m_sampler2Ds.find(textureName) == m_sampler2Ds.end()))
		{
			m_sampler2Ds[textureName].m_boundTexture = nullptr;
			m_sampler2Ds[textureName].m_isActive = false;
		}
		else
		{
			LINA_CORE_ERR("This material doesn't support texture slot with the name {0}, returning...", textureName);
			return;
		}
	}
	Texture& Material::GetTexture(const std::string& name)
	{
		if (!(m_sampler2Ds.find(name) == m_sampler2Ds.end()))
			return *m_sampler2Ds[name].m_boundTexture;
		else
		{
			LINA_CORE_WARN("This material doesn't support texture slot with the name {0}, returning empty texture", name);
			return Texture();
		}
	}
	Material& Material::CreateMaterial(Shaders shader, const std::string& path)
	{
		// Create material & set it's shader.
		int id = Utility::GetUniqueID();
		Material& mat = s_loadedMaterials[id];
		SetMaterialShader(mat, shader);
		SetMaterialContainers(mat);
		mat.m_materialID = id;
		mat.m_path = path.compare("") == 0 ? INTERNAL_MAT_PATH : path;
		return s_loadedMaterials[id];
	}

	Material& Material::LoadMaterialFromFile(const std::string& path)
	{
		// Create material & set it's shader.
		int id = Utility::GetUniqueID();
		Material& mat = s_loadedMaterials[id];
		Material::LoadMaterialData(mat, path);
		SetMaterialContainers(mat);
		SetMaterialShader(mat, mat.m_shaderType, true);
		mat.m_materialID = id;
		mat.m_path = path;
		return s_loadedMaterials[id];
	}
	Material& Material::GetMaterial(int id)
	{
		if (!MaterialExists(id))
		{
			// Mesh not found.
			LINA_CORE_WARN("Material with the id {0} was not found, returning default material...", id);
			return RenderEngine::GetDefaultUnlitMaterial();
		}

		return s_loadedMaterials[id];
	}

	Material& Material::GetMaterial(const std::string& path)
	{
		const auto it = std::find_if(s_loadedMaterials.begin(), s_loadedMaterials.end(), [path]
		(const auto& item) -> bool { return item.second.GetPath().compare(path) == 0; });

		if (it == s_loadedMaterials.end())
		{
			// Mesh not found.
			LINA_CORE_WARN("Material with the path {0} was not found, returning un-constructed material...", path);
			return Material();
		}

		return it->second;
	}


	Material& Material::SetMaterialShader(Material& material, Shaders shader, bool onlySetID)
	{
		// If no shader found, fall back to standardLit
		std::map<int, Shader>& shaders = Shader::GetLoadedShaders();
		if (shaders.find(shader) == shaders.end()) {
			LINA_CORE_WARN("Shader with engine ID {0} was not found. Setting material's shader to standardUnlit.", shader);
			material.m_shaderID = shaders[Shaders::Standard_Unlit].GetID();
		}
		else
			material.m_shaderID = shaders[shader].GetID();

		if (onlySetID) return material;


		// Clear all shader related material data.
		material.m_sampler2Ds.clear();
		material.m_colors.clear();
		material.m_floats.clear();
		material.m_ints.clear();
		material.m_vector3s.clear();
		material.m_vector2s.clear();
		material.m_matrices.clear();
		material.m_vector4s.clear();
		material.m_shaderType = shader;
		material.m_isShadowMapped = false;
		material.m_receivesLighting = false;
		material.m_usesHDRI = false;

		if (shader == Shaders::Standard_Unlit)
		{
			material.m_colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.m_sampler2Ds[MAT_TEXTURE2D_DIFFUSE] = { 0 };
			material.m_ints[MAT_SURFACETYPE] = 0;
		}
		else if (shader == Shaders::Skybox_SingleColor)
		{
			material.m_colors[MAT_COLOR] = Color::Gray;
		}
		else if (shader == Shaders::Skybox_Gradient)
		{
			material.m_colors[MAT_STARTCOLOR] = Color::Black;
			material.m_colors[MAT_ENDCOLOR] = Color::White;
		}
		else if (shader == Shaders::Skybox_Procedural)
		{
			material.m_colors[MAT_STARTCOLOR] = Color::Black;
			material.m_colors[MAT_ENDCOLOR] = Color::White;
			material.m_vector3s[MAT_SUNDIRECTION] = Vector3(0, -1, 0);
		}
		else if (shader == Shaders::Skybox_Cubemap)
		{
			material.m_sampler2Ds[MAT_MAP_ENVIRONMENT] = { 0 };
		}
		else if (shader == Shaders::Skybox_HDRI)
		{
			material.m_sampler2Ds[MAT_MAP_ENVIRONMENT] = { 0 };
		}
		else if (shader == Shaders::Skybox_Atmospheric)
		{
			material.m_floats[MAT_TIME] = 0.0f;
			material.m_floats[MAT_CIRRUS] = 0.4f;
			material.m_floats[MAT_CUMULUS] = 0.8f;
			material.m_floats[UF_FLOAT_TIME] = 0.0f;
		}

		else if (shader == Shaders::ScreenQuad_Final)
		{
			material.m_sampler2Ds[MAT_MAP_SCREEN] = { 0 };
			material.m_sampler2Ds[MAT_MAP_BLOOM] = { 1 };
			material.m_sampler2Ds[MAT_MAP_OUTLINE] = { 2 };
			material.m_floats[MAT_EXPOSURE] = 1.0f;
			material.m_floats[MAT_FXAAREDUCEMIN] = 1.0f / 128.0f;
			material.m_floats[MAT_FXAAREDUCEMUL] = 1.0f / 8.0f;
			material.m_floats[MAT_FXAASPANMAX] = 8.0f;
			material.m_bools[MAT_BLOOMENABLED] = false;
			material.m_bools[MAT_FXAAENABLED] = false;
			material.m_vector3s[MAT_INVERSESCREENMAPSIZE] = Vector3();
		}
		else if (shader == Shaders::ScreenQuad_Blur)
		{
			material.m_sampler2Ds[MAT_MAP_SCREEN] = { 0 };
			material.m_bools[MAT_ISHORIZONTAL] = false;
		}
		else if (shader == Shaders::ScreenQuad_Outline)
		{
			material.m_sampler2Ds[MAT_MAP_SCREEN] = { 0 };
		}
		else if (shader == Shaders::ScreenQuad_Shadowmap)
		{


		}
		else if (shader == Shaders::PBR_Lit)
		{
			material.m_sampler2Ds[MAT_TEXTURE2D_ALBEDOMAP] = { 0 };
			material.m_sampler2Ds[MAT_TEXTURE2D_NORMALMAP] = { 1 };
			material.m_sampler2Ds[MAT_TEXTURE2D_ROUGHNESSMAP] = { 2 };
			material.m_sampler2Ds[MAT_TEXTURE2D_METALLICMAP] = { 3 };
			material.m_sampler2Ds[MAT_TEXTURE2D_AOMAP] = { 4 };
		//	material.m_sampler2Ds[MAT_TEXTURE2D_SHADOWMAP] = { 5 };
			material.m_sampler2Ds[MAT_TEXTURE2D_BRDFLUTMAP] = { 5 };
			material.m_sampler2Ds[MAT_TEXTURE2D_IRRADIANCEMAP] = { 6, nullptr, "", "", TextureBindMode::BINDTEXTURE_CUBEMAP, false };
			material.m_sampler2Ds[MAT_TEXTURE2D_PREFILTERMAP] = { 7,nullptr, "", "", TextureBindMode::BINDTEXTURE_CUBEMAP, false };
			material.m_floats[MAT_METALLICMULTIPLIER] = 1.0f;
			material.m_floats[MAT_ROUGHNESSMULTIPLIER] = 1.0f;
			material.m_ints[MAT_WORKFLOW] = 0;
			material.m_ints[MAT_SURFACETYPE] = 0;
			material.m_vector2s[MAT_TILING] = Vector2::One;
			material.m_colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.m_receivesLighting = true;
		//	material.m_isShadowMapped = true;
			material.m_usesHDRI = true;
		}
		else if (shader == Shaders::HDRI_Equirectangular)
		{
			material.m_sampler2Ds[MAT_MAP_EQUIRECTANGULAR] = { 0 };
			material.m_matrices[UF_MATRIX_VIEW] = Matrix();
			material.m_matrices[UF_MATRIX_PROJECTION] = Matrix();
		}
		else if (shader == Shaders::Debug_Line)
		{
			material.m_colors[MAT_COLOR] = Color::White;
		}
		else if (shader == Shaders::Standard_Sprite)
		{
			material.m_colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.m_sampler2Ds[MAT_TEXTURE2D_DIFFUSE] = { 0 };
		}


		return material;
	}

	void Material::SetMaterialContainers(Material& material)
	{
		if (material.m_usesHDRI)
			s_hdriMaterials.emplace(&material);

		if(material.m_isShadowMapped)
			s_shadowMappedMaterials.emplace(&material);

	}

	void Material::UnloadAll()
	{
		s_loadedMaterials.clear();
		s_hdriMaterials.clear();
		s_shadowMappedMaterials.clear();
	}


	bool Material::MaterialExists(int id)
	{
		if (id < 0) return false;
		return !(s_loadedMaterials.find(id) == s_loadedMaterials.end());
	}

	bool Material::MaterialExists(const std::string& path)
	{
		const auto it = std::find_if(s_loadedMaterials.begin(), s_loadedMaterials.end(), [path]
		(const auto& it) -> bool { 	return it.second.GetPath().compare(path) == 0; 	});
		return it != s_loadedMaterials.end();
	}

	void Material::UnloadMaterialResource(int id)
	{
		if (!MaterialExists(id))
		{
			LINA_CORE_WARN("Material not found! Aborting... ");
			return;
		}

		// If its in the internal list, remove first.
		if (s_shadowMappedMaterials.find(&s_loadedMaterials[id]) != s_shadowMappedMaterials.end())
			s_shadowMappedMaterials.erase(&s_loadedMaterials[id]);

		s_loadedMaterials.erase(id);
	}
}