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

#include "FPSDemo/FPSDemoLevel.hpp"
#include "Core/EditorCommon.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "Core/Application.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "FPSDemo/HeadbobComponent.hpp"
#include "FPSDemo/PlayerMotionComponent.hpp"
#include "Drawers/ComponentDrawer.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "Utility/Math/Matrix.hpp"
#include "Helpers/DrawParameterHelper.hpp"
#include "glad/glad.h"
#include "glm/gtc/matrix_access.hpp"

namespace LinaEngine
{
	using namespace LinaEngine::ECS;
	using namespace LinaEngine::Graphics;

	bool FPSDemoLevel::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		Level::Install(loadFromFile, path, levelName);
		return true;
	}

	Matrix portalView(Matrix originalView, Transformation* srcTransform, Transformation* destTransform)
	{
		//Vector3 loc = destTransform->GetLocation();
		//Quaternion rot = destTransform->GetRotation();
		//return Matrix::InitLookAt(loc, loc + rot.GetForward(), rot.GetUp());

		Matrix mv = originalView * srcTransform->ToMatrix();
		Vector3 loc= srcTransform->GetLocation();
		Quaternion rot = srcTransform->GetRotation();
		Matrix portalCam = mv * Matrix::InitRotation(rot) * destTransform->ToMatrix().Inverse();
		return portalCam;
	}

	void FPSDemoLevel::Initialize()
	{
		m_registry = &Application::GetECSRegistry();

		Application::GetApp().GetPlayModeStack().PushLayer(m_player);

		if (!m_processOnce)
		{
			m_processOnce = true;
			RenderEngine& renderEngine = Application::GetRenderEngine();
			RenderDevice& rd = renderEngine.GetRenderDevice();
			Vector2 viewportSize = renderEngine.GetViewportSize();

			// Setup portal rt
			m_portalRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
			m_portalRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
			m_portalRTParams.m_textureParams.m_minFilter = m_portalRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
			m_portalRTParams.m_textureParams.m_wrapS = m_portalRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_REPEAT;
			m_portalTexture.ConstructRTTexture(rd, viewportSize, m_portalRTParams, false);
			m_portalBuffer.Construct(rd, RenderBufferStorage::STORAGE_DEPTH, viewportSize);
			m_portalRT.Construct(rd, m_portalTexture, viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_portalBuffer.GetID());

		}
		
		LinaEngine::Application::GetEngineDispatcher().SubscribeAction<void*>("fpsdemo_preDraw", LinaEngine::Action::ActionType::PreDraw, std::bind(&FPSDemoLevel::PreDraw, this));
		//LinaEngine::Application::GetRenderEngine().CustomDrawActivation(true);
		//auto customDrawFunc = std::bind(&FPSDemoLevel::CustomDraw, this);
		//LinaEngine::Application::GetRenderEngine().SetCustomDrawFunction(customDrawFunc);
		// Component drawer.
		m_componentDrawer.AddComponentDrawFunctions();

		ECSEntity portal1 = m_registry->GetEntity("Portal1");
		ECSEntity portal2 = m_registry->GetEntity("Portal2");

		if (portal1 != entt::null && portal2 != entt::null)
		{
			//m_registry->get<MeshRendererComponent>(portal1).m_excludeFromDrawList = true;
		}
	}

	void FPSDemoLevel::Tick(bool isInPlayMode, float delta)
	{
		m_isInPlayMode = isInPlayMode;
	}

	void FPSDemoLevel::Uninstall()
	{
		LinaEngine::Application::GetEngineDispatcher().UnsubscribeAction("fpsdemo_preDraw", LinaEngine::Action::ActionType::PreDraw);
		LinaEngine::Application::GetApp().GetPlayModeStack().PopLayer(m_player);
	}

	void FPSDemoLevel::PreDraw()
	{
		if (!m_isInPlayMode) return;
		
		RenderEngine& renderEngine = Application::GetRenderEngine();
		RenderDevice& rd = renderEngine.GetRenderDevice();
		Vector2 viewportSize = renderEngine.GetViewportSize();

		TransformComponent& p1tr = m_registry->get<TransformComponent>(m_registry->GetEntity("Portal1"));
		TransformComponent& p2tr = m_registry->get<TransformComponent>(m_registry->GetEntity("Portal2"));
		rd.SetFBO(m_portalRT.GetID());
		rd.SetViewport(Vector2::Zero, viewportSize);

		// Clear color.
		rd.Clear(true, true, true, renderEngine.GetCameraSystem()->GetCurrentClearColor(), 0xFF);
		Matrix vm = portalView(renderEngine.GetCameraSystem()->GetViewMatrix(), &p1tr.transform, &p2tr.transform);

		renderEngine.GetCameraSystem()->InjectViewMatrix(vm);
		renderEngine.UpdateSystems();
		renderEngine.DrawSceneObjects(renderEngine.GetMainDrawParams());


		Material::GetMaterial(m_registry->get<MeshRendererComponent>(m_registry->GetEntity("Portal1")).m_materialID).SetTexture(MAT_TEXTURE2D_DIFFUSE, &m_portalTexture);


	}

	Matrix ClipOblique(Matrix& viewMat, Matrix& projection, const Vector3& pos, const Vector3& normal)
	{
		Vector4 viewPos = viewMat * Vector4(pos, 1);
		Vector4 viewNormal = (viewMat * Vector4(normal, 0));
		const Vector3 cpos = viewPos.XYZ();
		const Vector3 cnormal = viewNormal.XYZ();
		const Vector4 cplane(cnormal.x, cnormal.y, cnormal.z, -cpos.Dot(cnormal));

		const Vector4 q = projection.Inverse() * Vector4(
			(cplane.x < 0.0f ? 1.0f : -1.0f),
			(cplane.y < 0.0f ? 1.0f : -1.0f),
			1.0f,
			1.0f
		);
		const Vector4 c = cplane * (2.0f / cplane.Dot(q));

		float c0 = c.x - projection[0][3];
		float c1 = c.y - projection[1][3];
		float c2 = c.z - projection[2][3];
		float c3 = c.w - projection[3][3];

		projection[0][2] = c0;
		projection[1][2] = c1;
		projection[2][2] = c2;
		projection[3][2] = c3;

		return projection;
	}
	glm::mat4 clippedProjMat(Matrix const& viewMat, glm::mat4 const& projMat, float dist, Quaternion rot)
	{
		Vector3 mult = rot * Vector3(0.0f, 0.0f, -1.0f);
		Vector4 clipPlane(mult.x, mult.y, mult.z, dist);
		clipPlane = viewMat.Transpose().Inverse() * clipPlane;
	
		if (clipPlane.w > 0.0f)
			return projMat;
		LINA_CORE_TRACE("hah");

		glm::vec4 q = glm::inverse(projMat) * glm::vec4(
			glm::sign(clipPlane.x),
			glm::sign(clipPlane.y),
			1.0f,
			1.0f
		);

		Vector4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));

		glm::mat4 newProj = projMat;
		// third row = clip plane - fourth row
		Vector4 row = glm::row(newProj, 3);
		newProj = glm::row(newProj, 2, c - row);

		return newProj;
	}

	void FPSDemoLevel::CustomDraw()
	{
		return;
		RenderEngine& renderEngine = Application::GetRenderEngine();
		RenderDevice& rd = renderEngine.GetRenderDevice();

		MeshRendererComponent& mrc = m_registry->get<MeshRendererComponent>(m_registry->GetEntity("Portal1"));
		TransformComponent& tr = m_registry->get<TransformComponent>(m_registry->GetEntity("Portal1"));

		// Clear color.
		rd.Clear(true, true, true, renderEngine.GetCameraSystem()->GetCurrentClearColor(), 0xFF);
	//	renderEngine.UpdateSystems();
	//	renderEngine.DrawSkybox();
	//	renderEngine.GetMeshRendererSystem()->FlushSingleRenderer(mrc, tr, DrawParameterHelper::GetDefault());		
	//return;

		// Disable color and depth drawing
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		// Disable depth test
		glDisable(GL_DEPTH_TEST);

		// Enable stencil test, to prevent drawing outside
		// region of current portal depth
		glEnable(GL_STENCIL_TEST);

		// Fail stencil test when inside of outer portal
		// (fail where we should be drawing the inner portal)
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

		// Increment stencil value on stencil fail
		// (on area of inner portal)
		glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);

		// Enable (writing into) all stencil bits
		glStencilMask(0xFF);

		DrawParams portalParams;
		portalParams.skipParameters = true;
		renderEngine.UpdateSystems();
		renderEngine.GetMeshRendererSystem()->FlushSingleRenderer(mrc, tr, portalParams);
		
		{
			// Enable color and depth drawing
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);

			// Clear the depth buffer so we don't interfere with stuff
			// outside of this inner portal
			glClear(GL_DEPTH_BUFFER_BIT);

			// Enable the depth test
			// So the stuff we render here is rendered correctly
			glEnable(GL_DEPTH_TEST);

			// Enable stencil test
			// So we can limit drawing inside of the inner portal
			glEnable(GL_STENCIL_TEST);

			// Disable drawing into stencil buffer
			glStencilMask(0x00);

			// Draw only where stencil value == recursionLevel + 1
			// which is where we just drew the new portal
			glStencilFunc(GL_EQUAL, 1, 0xFF);

			// Draw scene objects with destView, limited to stencil buffer
			// use an edited projection matrix to set the near plane to the portal plane
			Matrix vm = renderEngine.GetCameraSystem()->GetViewMatrix() * tr.transform.ToMatrix() * Matrix::InitRotation(tr.transform.GetRotation()) * tr.transform.ToMatrix().Inverse();
			Matrix vproj = ClipOblique(vm, renderEngine.GetCameraSystem()->GetProjectionMatrix(), tr.transform.GetLocation(), tr.transform.GetRotation().GetForward());
			
			renderEngine.GetCameraSystem()->InjectViewMatrix(vm);
			renderEngine.GetCameraSystem()->InjectProjMatrix(vproj);

			//glm::length(tr.transform.GetLocation() - renderEngine.GetCameraSystem()->GetCameraLocation());
			// Draw scene
			renderEngine.UpdateSystems();
			renderEngine.DrawSceneObjects(portalParams);
		}

		// Disable color and depth drawing
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		// Enable stencil test and stencil drawing
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);

		// Fail stencil test when inside of our newly rendered
		// inner portal
		glStencilFunc(GL_NOTEQUAL,  1, 0xFF);

		// Decrement stencil value on stencil fail
		// This resets the incremented values to what they were before,
		// eventually ending up with a stencil buffer full of zero's again
		// after the last (outer) step.
		glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);
		renderEngine.UpdateSystems();
		renderEngine.GetMeshRendererSystem()->FlushSingleRenderer(mrc, tr, portalParams);

		// Disable the stencil test and stencil writing
		glDisable(GL_STENCIL_TEST);
		glStencilMask(0x00);

		// Disable color writing
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// Enable the depth test, and depth writing.
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		// Make sure we always write the data into the buffer
		glDepthFunc(GL_ALWAYS);

		// Clear the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);

		renderEngine.UpdateSystems();
		renderEngine.GetMeshRendererSystem()->FlushSingleRenderer(mrc, tr, portalParams);

		// Reset the depth function to the default
		glDepthFunc(GL_LESS);

		// Enable stencil test and disable writing to stencil buffer
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0x00);

		// Draw at stencil >= recursionlevel
		// which is at the current level or higher (more to the inside)
		// This basically prevents drawing on the outside of this level.
		glStencilFunc(GL_LEQUAL, 0, 0xFF);

		// Enable color and depth drawing again
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		// And enable the depth test
		glEnable(GL_DEPTH_TEST);

		renderEngine.UpdateSystems();
		renderEngine.DrawSceneObjects(portalParams);
	}
}