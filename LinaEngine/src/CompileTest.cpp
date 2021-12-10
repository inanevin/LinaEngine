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

	
// Core

//#include "Core/Application.hpp"
//#include "Core/Common.hpp"
//#include "Core/Environment.hpp"
//#include "Core/Internal.hpp"
//#include "Core/Layer.hpp"
//#include "Core/LayerStack.hpp"
//
//#include "Core/SizeDefinitions.hpp"
//#include "Core/Time.hpp"
//
//// ECS
//
//#include "ECS/Components/CameraComponent.hpp"
//#include "ECS/Components/ColliderComponent.hpp"
//#include "ECS/Components/CubeChunkComponent.hpp"
//#include "ECS/Components/FreeLookComponent.hpp"
//#include "ECS/Components/LightComponent.hpp"
//#include "ECS/Components/MeshRendererComponent.hpp"
//#include "ECS/Components/MotionComponent.hpp"
//#include "ECS/Components/MovementControlComponent.hpp"
//#include "ECS/Components/EntityDataComponent.hpp"
//#include "ECS/Interactions/TestInteraction.hpp"
//#include "ECS/Systems/CameraSystem.hpp"
//#include "ECS/Systems/CubeChunkRenderSystem.hpp"
//#include "ECS/Systems/CubeChunkSystem.hpp"
//#include "ECS/Systems/FreeLookSystem.hpp"
//#include "ECS/Systems/LightingSystem.hpp"
//#include "ECS/Systems/MeshRendererSystem.hpp"
//#include "ECS/Systems/MotionSystem.hpp"
//#include "ECS/Systems/MovementControlSystem.hpp"
//#include "ECS/Utility/MotionIntegrators.hpp"
//#include "ECS/ECSComponent.hpp"
//#include "ECS/ECSInteraction.hpp"
//#include "ECS/ECSSystem.hpp"
//#include "ECS/EntityComponentSystem.hpp"
//
//// Events
//
//#include "Actions/Action.hpp"
//#include "Actions/ActionDispatcher.hpp"
//#include "Actions/ActionSubscriber.hpp"
//#include "Actions/ApplicationEvent.hpp"
//#include "Actions/Event.hpp"
//#include "Actions/KeyEvent.hpp"
//#include "Actions/MouseEvent.hpp"
//
//// Input
//
//#include "Input/InputAxisBinder.hpp"
//#include "Input/InputCommon.hpp"
//#include "Input/InputEngine.hpp"
//#include "Input/InputMappings.hpp"
//#include "Interfaces/IInputSubscriber.hpp"
//
//// PAM
//
//#include "PackageManager/Generic/cmwc4096.hpp"
//#include "PackageManager/Generic/GenericMath.hpp"
//#include "PackageManager/Generic/GenericMemory.hpp"
//#include "PackageManager/Generic/GenericVectorMath.hpp"
//#include "PackageManager/OpenGL/GLInputDevice.hpp"
//#include "PackageManager/OpenGL/GLRenderDevice.hpp"
//#include "PackageManager/OpenGL/GLWindow.hpp"
//#include "PackageManager/SSE/SSEVectorMath.hpp"
//#include "PackageManager/PAMInputDevice.hpp"
//#include "PackageManager/PAMMath.hpp"
//#include "Memory/Memory.hpp"
//#include "PackageManager/PAMSIMD.hpp"
//#include "PackageManager/PAMVectorMath.hpp"
//#include "PackageManager/PAMWindow.hpp"
//#include "PackageManager/PAMRenderDevice.hpp"
//
//// Physics
//
//#include "Physics/PhysicsInteractionWorld.hpp"
//#include "Physics/PhysicsEngine.hpp"
//
//// Rendering
//
//#include "Rendering/ArrayBitmap.hpp"
//#include "Rendering/DDSTexture.hpp"
//#include "Rendering/RenderEngine.hpp"
//#include "Rendering/Shader.hpp"
//#include "Rendering/Texture.hpp"
//#include "Rendering/Window.hpp"
//#include "Rendering/Mesh.hpp"
//#include "Rendering/ModelLoader.hpp"
//#include "Rendering/VertexArray.hpp"
//#include "Rendering/Material.hpp"
//#include "Rendering/Sampler.hpp"
//#include "Rendering/UniformBuffer.hpp"
//#include "Rendering/RenderContext.hpp"
//#include "Rendering/RenderTarget.hpp"
//#include "Rendering/Model.hpp"
//#include "Rendering/RenderingCommon.hpp"
//#include "Rendering/ShaderConstants.hpp"
//
//
//// Utility
//
//#include "Math/Color.hpp"
//#include "Math/Matrix.hpp"
//#include "Math/Quaternion.hpp"
//#include "Math/Transformation.hpp"
//#include "Math/Vector.hpp"
//#include "Math/AABB.hpp"
//#include "Math/Plane.hpp"
//#include "Log/Log.hpp"
//#include "Utility/UtilityFunctions.hpp"
//
//// World
//
//#include "World/Level.hpp"
//
//// Others
//
//#include "EntryPoint.hpp"
//#include "Lina.hpp"
//#include "LinaPch.hpp"