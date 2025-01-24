/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/MeshManager.hpp"

#include "Common/FileSystem/FileSystem.hpp"
#include <LinaGX/Utility/ModelUtility.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lina
{

	void Model::Metadata::SaveToStream(OStream& stream) const
	{
		stream << materials;
	}

	void Model::Metadata::LoadFromStream(IStream& stream)
	{
		stream >> materials;
	}

	Model::~Model()
	{
		DestroyTextureDefs();
	}

	void Model::DestroyTextureDefs()
	{
		for (ModelTexture& txt : m_textureDefs)
		{
			if (txt.buffer.pixels == nullptr)
				continue;

			delete[] txt.buffer.pixels;
			txt.buffer.pixels = nullptr;
		}
	}

	namespace
	{
		Matrix4 TranslateRotateScale(const LinaGX::LGXVector3& pos, const LinaGX::LGXVector4& rot, const LinaGX::LGXVector3& scale)
		{
			glm::vec3 p		= glm::vec3(pos.x, pos.y, pos.z);
			glm::vec3 s		= glm::vec3(scale.x, scale.y, scale.z);
			glm::quat r		= glm::quat(rot.w, rot.x, rot.y, rot.z);
			glm::mat4 model = glm::mat4(1.0f);
			model			= glm::translate(model, p);
			model *= glm::mat4_cast(r);
			model = glm::scale(model, s);
			return model;
		}
	} // namespace

	bool Model::LoadFromFile(const String& path)
	{
		if (!FileSystem::FileOrPathExists(path))
			return false;

		const String	  ext		= FileSystem::GetFileExtension(path);
		LinaGX::ModelData modelData = {};

		bool success = false;
		if (ext.compare("glb") == 0)
			success = LinaGX::LoadGLTFBinary(path.c_str(), modelData);
		else
			success = LinaGX::LoadGLTFASCII(path.c_str(), modelData);

		if (!success)
		{
			LINA_ERR("Failed loading model! {0}", path);
			return false;
		}
		m_materialDefs.resize(modelData.allMaterials.size());
		m_meta.materials.resize(m_materialDefs.size());

		size_t idx = 0;
		for (auto* lgxMat : modelData.allMaterials)
		{
			auto& mat	 = m_materialDefs[idx];
			mat.name	 = lgxMat->name;
			mat.isOpaque = lgxMat->isOpaque;
			mat.albedo	 = Color(lgxMat->baseColor);

			for (auto [type, index] : lgxMat->textureIndices)
				mat.textureIndices[static_cast<uint8>(type)] = index;

			idx++;
		}

		for (auto* lgxTexture : modelData.allTextures)
		{
			ModelTexture txt = {};
			txt.name		 = lgxTexture->name;

			const size_t sz = static_cast<size_t>(lgxTexture->buffer.width * lgxTexture->buffer.height * lgxTexture->buffer.bytesPerPixel);
			txt.buffer		= {
					 .pixels		= new uint8[sz],
					 .width			= lgxTexture->buffer.width,
					 .height		= lgxTexture->buffer.height,
					 .bytesPerPixel = lgxTexture->buffer.bytesPerPixel,
			 };

			MEMCPY(txt.buffer.pixels, lgxTexture->buffer.pixels, sz);
			m_textureDefs.push_back(txt);
		}

		const size_t allNodesSz = modelData.allNodes.size();
		m_allNodes.resize(allNodesSz);

		for (size_t i = 0; i < allNodesSz; i++)
		{
			LinaGX::ModelNode* lgxNode = modelData.allNodes.at(i);

			m_allNodes[i] = {
				.name			   = lgxNode->name,
				.parentIndex	   = lgxNode->parent != nullptr ? static_cast<int32>(lgxNode->parent->index) : -1,
				.meshIndex		   = lgxNode->meshIndex,
				.localMatrix	   = lgxNode->localMatrix.empty() ? TranslateRotateScale(lgxNode->position, lgxNode->quatRot, lgxNode->scale) : glm::make_mat4(lgxNode->localMatrix.data()),
				.inverseBindMatrix = lgxNode->inverseBindMatrix.empty() ? Matrix4::Identity() : glm::make_mat4(lgxNode->inverseBindMatrix.data()),
			};
		}

		const size_t meshesSz = modelData.allMeshes.size();
		m_allMeshes.resize(meshesSz);

		for (size_t i = 0; i < meshesSz; i++)
		{
			LinaGX::ModelMesh* lgxMesh = modelData.allMeshes.at(i);

			Mesh& mesh = m_allMeshes[i];

			mesh.name	   = lgxMesh->name;
			mesh.nodeIndex = lgxMesh->nodeIndex;
			mesh.primitivesSkinned.clear();
			mesh.primitivesStatic.clear();

			for (LinaGX::ModelMeshPrimitive* prim : lgxMesh->primitives)
			{
				const size_t vtxCount = prim->vertexCount;
				LINA_ASSERT(prim->indexType == LinaGX::IndexType::Uint16, "");
				uint16*		 indices	 = reinterpret_cast<uint16*>(prim->indices.data());
				const size_t indicesSize = prim->indices.size() / 2;

				if (!prim->weights.empty())
				{
					PrimitiveSkinned primitive = {};
					primitive.materialIndex	   = prim->materialIndex == -1 ? 0 : static_cast<uint32>(prim->materialIndex);
					primitive.vertices.resize(vtxCount);
					primitive.indices.insert(primitive.indices.end(), indices, indices + indicesSize);

					const bool boneIndices16 = !prim->jointsui16.empty();

					for (size_t j = 0; j < vtxCount; j++)
					{
						VertexSkinned& vtx = primitive.vertices[j];
						vtx.pos.x		   = prim->positions[j].x;
						vtx.pos.y		   = prim->positions[j].y;
						vtx.pos.z		   = prim->positions[j].z;

						vtx.uv.x = prim->texCoords[j].x;
						vtx.uv.y = prim->texCoords[j].y;

						vtx.normal.x = prim->normals[j].x;
						vtx.normal.y = prim->normals[j].y;
						vtx.normal.z = prim->normals[j].z;

						vtx.boneWeights.x = prim->weights[j].x;
						vtx.boneWeights.y = prim->weights[j].y;
						vtx.boneWeights.z = prim->weights[j].z;
						vtx.boneWeights.w = prim->weights[j].w;

						if (boneIndices16)
						{
							vtx.boneIndices.x = prim->jointsui16[j].x;
							vtx.boneIndices.y = prim->jointsui16[j].y;
							vtx.boneIndices.z = prim->jointsui16[j].z;
							vtx.boneIndices.w = prim->jointsui16[j].w;
						}
						else
						{
							vtx.boneIndices.x = static_cast<int32>(prim->jointsui8[j].x);
							vtx.boneIndices.y = static_cast<int32>(prim->jointsui8[j].y);
							vtx.boneIndices.z = static_cast<int32>(prim->jointsui8[j].z);
							vtx.boneIndices.w = static_cast<int32>(prim->jointsui8[j].w);
						}
					}

					mesh.primitivesSkinned.push_back(primitive);
				}
				else
				{
					PrimitiveStatic primitive = {};
					primitive.materialIndex	  = prim->materialIndex == -1 ? 0 : static_cast<uint32>(prim->materialIndex);
					primitive.vertices.resize(vtxCount);
					primitive.indices.insert(primitive.indices.end(), indices, indices + indicesSize);

					for (size_t j = 0; j < vtxCount; j++)
					{
						VertexStatic& vtx = primitive.vertices[j];
						vtx.pos.x		  = prim->positions[j].x;
						vtx.pos.y		  = prim->positions[j].y;
						vtx.pos.z		  = prim->positions[j].z;

						vtx.uv.x = prim->texCoords[j].x;
						vtx.uv.y = prim->texCoords[j].y;

						vtx.normal.x = prim->normals[j].x;
						vtx.normal.y = prim->normals[j].y;
						vtx.normal.z = prim->normals[j].z;
					}

					mesh.primitivesStatic.push_back(primitive);
				}
			}
		}

		const size_t skinsSz = modelData.allSkins.size();
		m_allSkins.resize(skinsSz);

		for (size_t i = 0; i < skinsSz; i++)
		{
			LinaGX::ModelSkin* lgxSkin = modelData.allSkins.at(i);
			ModelSkin&		   skin	   = m_allSkins[i];
			skin.rootJointIndex		   = lgxSkin->rootJoint == nullptr ? -1 : lgxSkin->rootJoint->index;

			for (LinaGX::ModelNode* node : lgxSkin->joints)
				skin.jointIndices.push_back(node->index);
		}

		const size_t animsSz = modelData.allAnims.size();
		m_allAnimations.resize(animsSz);

		for (size_t i = 0; i < animsSz; i++)
		{
			LinaGX::ModelAnimation* lgxAnim = modelData.allAnims.at(i);
			ModelAnimation&			anim	= m_allAnimations[i];
			anim.duration					= lgxAnim->duration;
			anim.name						= lgxAnim->name;

			const size_t channelsSz = lgxAnim->channels.size();

			for (size_t j = 0; j < channelsSz; j++)
			{
				const LinaGX::ModelAnimationChannel& lgxChannel = lgxAnim->channels.at(j);

				if (lgxChannel.targetProperty == LinaGX::GLTFAnimationProperty::Position)
				{
					anim.positionChannels.push_back({});
					ModelAnimationChannelV3& posChannel = anim.positionChannels.back();

					posChannel.interpolation = static_cast<ModelAnimationInterpolation>((uint32)lgxChannel.interpolation);
					posChannel.nodeIndex	 = lgxChannel.targetNode->index;

					for (size_t k = 0; k < lgxChannel.keyframeTimes.size(); k++)
					{
						const float kf = lgxChannel.keyframeTimes[k];

						if (lgxChannel.interpolation == LinaGX::GLTFInterpolation::CubicSpline)
						{
							const Vector3 vec = Vector3(lgxChannel.values[k * 3], lgxChannel.values[k * 3 + 1], lgxChannel.values[k * 3 + 2]);
							const Vector3 in  = Vector3(lgxChannel.inTangents[k * 3], lgxChannel.inTangents[k * 3 + 1], lgxChannel.inTangents[k * 3 + 2]);
							const Vector3 out = Vector3(lgxChannel.outTangents[k * 3], lgxChannel.outTangents[k * 3 + 1], lgxChannel.outTangents[k * 3 + 2]);

							posChannel.keyframesSpline.push_back({
								.time		= kf,
								.inTangent	= in,
								.value		= vec,
								.outTangent = out,
							});
						}
						else
						{
							const Vector3 vec = Vector3(lgxChannel.values[k * 3], lgxChannel.values[k * 3 + 1], lgxChannel.values[k * 3 + 2]);
							posChannel.keyframes.push_back({
								.time  = kf,
								.value = vec,
							});
						}
					}
				}
				else if (lgxChannel.targetProperty == LinaGX::GLTFAnimationProperty::Scale)
				{
					anim.scaleChannels.push_back({});
					ModelAnimationChannelV3& channel = anim.scaleChannels.back();

					channel.interpolation = static_cast<ModelAnimationInterpolation>((uint32)lgxChannel.interpolation);
					channel.nodeIndex	  = lgxChannel.targetNode->index;

					for (size_t k = 0; k < lgxChannel.keyframeTimes.size(); k++)
					{
						const float kf = lgxChannel.keyframeTimes[k];

						if (lgxChannel.interpolation == LinaGX::GLTFInterpolation::CubicSpline)
						{
							const Vector3 vec = Vector3(lgxChannel.values[k * 3], lgxChannel.values[k * 3 + 1], lgxChannel.values[k * 3 + 2]);
							const Vector3 in  = Vector3(lgxChannel.inTangents[k * 3], lgxChannel.inTangents[k * 3 + 1], lgxChannel.inTangents[k * 3 + 2]);
							const Vector3 out = Vector3(lgxChannel.outTangents[k * 3], lgxChannel.outTangents[k * 3 + 1], lgxChannel.outTangents[k * 3 + 2]);

							channel.keyframesSpline.push_back({
								.time		= kf,
								.inTangent	= in,
								.value		= vec,
								.outTangent = out,
							});
						}
						else
						{
							const Vector3 vec = Vector3(lgxChannel.values[k * 3], lgxChannel.values[k * 3 + 1], lgxChannel.values[k * 3 + 2]);
							channel.keyframes.push_back({
								.time  = kf,
								.value = vec,
							});
						}
					}
				}
				else if (lgxChannel.targetProperty == LinaGX::GLTFAnimationProperty::Rotation)
				{
					anim.rotationChannels.push_back({});
					ModelAnimationChannelQ& channel = anim.rotationChannels.back();

					channel.interpolation = static_cast<ModelAnimationInterpolation>((uint32)lgxChannel.interpolation);
					channel.nodeIndex	  = lgxChannel.targetNode->index;

					for (size_t k = 0; k < lgxChannel.keyframeTimes.size(); k++)
					{
						const float kf = lgxChannel.keyframeTimes[k];

						if (lgxChannel.interpolation == LinaGX::GLTFInterpolation::CubicSpline)
						{
							const Vector4 vec = Vector4(lgxChannel.values[k * 4], lgxChannel.values[k * 4 + 1], lgxChannel.values[k * 4 + 2], lgxChannel.values[k * 4 + 3]);
							const Vector4 in  = Vector4(lgxChannel.inTangents[k * 4], lgxChannel.inTangents[k * 4 + 1], lgxChannel.inTangents[k * 4 + 2], lgxChannel.inTangents[k * 4 + 3]);
							const Vector4 out = Vector4(lgxChannel.outTangents[k * 4], lgxChannel.outTangents[k * 4 + 1], lgxChannel.outTangents[k * 4 + 2], lgxChannel.outTangents[k * 4 + 3]);

							channel.keyframesSpline.push_back({
								.time		= kf,
								.inTangent	= Quaternion(in),
								.value		= Quaternion(vec),
								.outTangent = Quaternion(out),
							});
						}
						else
						{
							const Vector4 vec = Vector4(lgxChannel.values[k * 4], lgxChannel.values[k * 4 + 1], lgxChannel.values[k * 4 + 2], lgxChannel.values[k * 4 + 3]);
							channel.keyframes.push_back({
								.time  = kf,
								.value = Quaternion(vec),
							});
						}
					}
				}
			}
		}

		return true;
	}

	void Model::LoadFromStream(IStream& stream)
	{
		Resource::LoadFromStream(stream);
		uint32 version = 0;
		stream >> version;
		stream >> m_id;
		stream >> m_materialDefs;
		stream >> m_meta;
		stream >> m_totalAABB;
		stream >> m_allNodes;
		stream >> m_allMeshes;
		stream >> m_allSkins;
		stream >> m_allAnimations;
	}

	size_t Model::GetSize() const
	{
		size_t total = sizeof(ModelMaterial) * m_materialDefs.size() + sizeof(ModelTexture) * m_textureDefs.size() + sizeof(Model);
		total += sizeof(Metadata);

		for (const Mesh& mesh : m_allMeshes)
		{
			total += sizeof(Mesh);
			total += sizeof(PrimitiveSkinned) * mesh.primitivesSkinned.size();
			total += sizeof(PrimitiveStatic) * mesh.primitivesStatic.size();

			for (const PrimitiveSkinned& prim : mesh.primitivesSkinned)
			{
				total += prim.vertices.size() * sizeof(VertexSkinned);
				total += prim.indices.size() * sizeof(uint16);
			}

			for (const PrimitiveStatic& prim : mesh.primitivesStatic)
			{
				total += prim.vertices.size() * sizeof(VertexStatic);
				total += prim.indices.size() * sizeof(uint16);
			}
		}
		return total;
	}

	void Model::SaveToStream(OStream& stream) const
	{
		Resource::SaveToStream(stream);
		stream << VERSION;
		stream << m_id;
		stream << m_materialDefs;
		stream << m_meta;
		stream << m_totalAABB;
		stream << m_allNodes;
		stream << m_allMeshes;
		stream << m_allSkins;
		stream << m_allAnimations;
	}

	void Model::GenerateHW()
	{
		LINA_ASSERT(m_hwValid == false, "");
		m_hwValid = true;
	}

	void Model::Upload(MeshManager* mm)
	{
		for (Mesh& mesh : m_allMeshes)
			mm->AddMesh(&mesh);

		m_hwUploadValid = true;
	}

	void Model::RemoveUpload(MeshManager* mm)
	{
		for (Mesh& mesh : m_allMeshes)
			mm->RemoveMesh(&mesh);
		m_hwUploadValid = true;
	}

	void Model::DestroyHW()
	{
		LINA_ASSERT(m_hwValid, "");
		m_hwValid		= false;
		m_hwUploadValid = false;
	}

} // namespace Lina
