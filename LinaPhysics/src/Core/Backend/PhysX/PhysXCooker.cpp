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

#include "Log/Log.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/Backend/PhysX/PhysXCooker.hpp"
#include "Core/Backend/PhysX/PhysXPhysicsEngine.hpp"
#include <PxPhysicsAPI.h>

namespace Lina::Physics
{
	using namespace physx;

	PxCooking* m_pxCooking = nullptr;

	void PhysXCooker::Initialize(ApplicationMode appMode, physx::PxFoundation* foundation)
	{
		m_appMode = appMode;
		m_pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));
		Event::EventSystem::Get()->Connect<Event::EShutdown, &PhysXCooker::OnShutdown>(this);
	}

	void PhysXCooker::CookConvexMesh(std::vector<Vector3>& vertices, std::vector<uint8>& bufferData)
	{
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = (PxU32)vertices.size();
		convexDesc.points.stride = sizeof(Vector3);
		convexDesc.points.data = &vertices[0];
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxDefaultMemoryOutputStream buf;
		PxConvexMeshCookingResult::Enum result;
		if (!m_pxCooking->cookConvexMesh(convexDesc, buf, &result))
		{
			LINA_ERR("Cooking convex mesh failed! {0}", typeid(*this).name());
			return;
		}

		bufferData.clear();
		bufferData = std::vector<uint8>(buf.getData(), buf.getData() + buf.getSize());

		PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

		// LINA_TRACE("Cooked! Creating convex mesh with sid {0} and node id {1}", sid, nodeIndex);

		// PHYSICSENGINE CreateConvexMesh(bufferData, sid, nodeIndex);
	}

	void PhysXCooker::OnShutdown(const Event::EShutdown& ev)
	{
		m_pxCooking->release();
	}

	void PhysXCooker::CookModelNodeVertices(Graphics::ModelNode& node, Graphics::Model& model)
	{
		std::vector<Vector3> vertices;

		//if (node.m_meshIndexes.size() > 0)
		//{
		//	// Get model meshes.
		//	auto& meshes = model.GetMeshes();
		//
		//	// For each mesh attach to this node.
		//	for (uint32 i = 0; i < node.m_meshIndexes.size(); i++)
		//	{
		//		// Get the mesh & it's vertex position data.
		//		auto& mesh = meshes[node.m_meshIndexes[i]];
		//		auto& data = mesh.GetVertexPositions();
		//
		//		// Add each of the 3 positions to the vertices array.
		//		for (uint32 j = 0; j < data.m_floatElements.size(); j += 3)
		//			vertices.push_back(Vector3(data.m_floatElements[j], data.m_floatElements[j + 1], data.m_floatElements[j + 2]));
		//	}
		//
		//	// Now we have a vertex vector, that contains all the vertices from a model node's all meshes.
		//	// This is the combined vertex data from sub-meshes.
		//	// Now we cook this data & assign a model & node id to it.
		//	CookConvexMesh(vertices, model.GetAssetData().m_convexMeshData[node.m_nodeID]);
		//}
		//
		//for (auto& child : node.m_children)
		//	CookModelNodeVertices(child, model);
	}

	void CreateConvexMeshesFromNodes(Graphics::ModelNode& node, Graphics::Model& model)
	{
		//if (node.m_meshIndexes.size() > 0)
		//{
		//	Graphics::ModelAssetData& data = model.GetAssetData();
		//	Physics::PhysicsEngine::Get()->CreateConvexMesh(data.m_convexMeshData[node.m_nodeID], model.GetID(), node.m_nodeID);
		//}
		//
		//for (auto& child : node.m_children)
		//	CreateConvexMeshesFromNodes(child, model);
	}

	void PhysXCooker::OnResourceLoadCompleted(const Event::EResourceLoadCompleted& ev)
	{
		// Handle convex mesh cooking if the loaded model does not contain any.
		//if (ev.m_type == Resources::ResourceType::Model)
		//{
		//	auto& model = Graphics::Model::GetModel(ev.m_sid);
		//	auto& meshes = model.GetMeshes();
		//
		//	if (m_appMode == ApplicationMode::Editor)
		//	{
		//		Graphics::ModelNode& root = model.GetRoot();
		//
		//		// If the model parameters does not contain a convex mesh data for the current mesh or regeneration is marked.
		//		if (model.GetAssetData().m_convexMeshData.size() == 0 || model.GetAssetData().m_regenerateConvexMeshes)
		//		{
		//			CookModelNodeVertices(root, model);
		//			model.SaveAssetData(model.GetAssetDataPath(), model.GetAssetData());
		//		}
		//		else
		//			CreateConvexMeshesFromNodes(root, model);
		//	}
		//	else
		//	{
		//		if (model.GetAssetData().m_convexMeshData.size() == 0)
		//			LINA_ERR("You are running in Standalone mode but your loaded models does not contain any convex mesh data. This might result in inaccurate collision simulation! {0}", typeid(*this).name());
		//
		//	}
		//
		//}
	}
}