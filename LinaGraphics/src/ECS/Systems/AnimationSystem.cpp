#include "ECS/Systems/AnimationSystem.hpp"

#include "ECS/Components/AnimationComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Rendering/Model.hpp"
#include "..\..\..\include\ECS\Systems\AnimationSystem.hpp"

namespace Lina::ECS
{
    void AnimationSystem::Initialize(const String& name)
    {
        System::Initialize(name);
    }
    void AnimationSystem::UpdateComponents(float delta)
    {
    }
} // namespace Lina::ECS
