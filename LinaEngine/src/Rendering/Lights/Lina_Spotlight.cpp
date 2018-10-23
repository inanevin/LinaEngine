#include "pch.h"
#include "Rendering/Lights/Lina_Spotlight.hpp"
#include "Game/Lina_Actor.hpp"

void Lina_SpotLight::AttachToActor(Lina_Actor& act)
{

	Lina_ActorComponent::AttachToActor(act);

	m_Actor->Lina->RenderingEngine()->AddSpotLight(this);
}
