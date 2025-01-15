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

#pragma once

#include "Core/Physics/CommonPhysics.hpp"
#include <Jolt/Jolt.h>

namespace Lina
{
	class PhysicsBPLayerInterface final : public JPH::BroadPhaseLayerInterface
	{
	public:
		PhysicsBPLayerInterface()		   = default;
		virtual ~PhysicsBPLayerInterface() = default;

		virtual uint GetNumBroadPhaseLayers() const override
		{
			return PhysicsBroadPhaseLayers::PHYSICS_BP_LAYER_MAX;
		}

		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
		{
			LINA_ASSERT(inLayer < PhysicsObjectLayers::PHYSICS_OBJ_LAYER_MAX, "");

			if (inLayer == PhysicsObjectLayers::PHYSICS_OBJ_LAYER_NONMOVING)
				return m_layers[PhysicsBroadPhaseLayers::PHYSICS_BP_LAYER_NONMOVING];

			return m_layers[PhysicsBroadPhaseLayers::PHYSICS_BP_LAYER_MOVING];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
		{
			switch ((JPH::BroadPhaseLayer::Type)inLayer)
			{
			case (JPH::BroadPhaseLayer::Type)PHYSICS_BP_LAYER_NONMOVING:
				return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type)PHYSICS_BP_LAYER_MOVING:
				return "MOVING";
			default:
				LINA_ASSERT(false, "");
				return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		JPH::BroadPhaseLayer m_layers[PhysicsBroadPhaseLayers::PHYSICS_BP_LAYER_MAX];
	};

} // namespace Lina
