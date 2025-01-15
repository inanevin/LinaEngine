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
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>

namespace Lina
{
	class PhysicsBPLayerInterface final : public JPH::BroadPhaseLayerInterface
	{
	public:
		virtual ~PhysicsBPLayerInterface() = default;

		PhysicsBPLayerInterface()
		{
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[PhysicsObjectLayers::NON_MOVING] = PhysicsBroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[PhysicsObjectLayers::MOVING]	 = PhysicsBroadPhaseLayers::MOVING;
		}

		virtual uint GetNumBroadPhaseLayers() const override
		{
			return PhysicsBroadPhaseLayers::NUM_LAYERS;
		}

		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
		{
			JPH_ASSERT(inLayer < PhysicsObjectLayers::NUM_LAYERS);
			return mObjectToBroadPhase[inLayer];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
		{
			switch ((JPH::BroadPhaseLayer::Type)inLayer)
			{
			case (JPH::BroadPhaseLayer::Type)PhysicsBroadPhaseLayers::NON_MOVING:
				return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type)PhysicsBroadPhaseLayers::MOVING:
				return "MOVING";
			default:
				JPH_ASSERT(false);
				return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		JPH::BroadPhaseLayer mObjectToBroadPhase[PhysicsObjectLayers::NUM_LAYERS];
	};

} // namespace Lina
