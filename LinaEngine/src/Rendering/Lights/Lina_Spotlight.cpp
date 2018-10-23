#include "pch.h"
#include "Rendering/Lights/Lina_Spotlight.h"
#include "Game/Lina_Actor.h"

void Lina_SpotLight::AttachToActor(Lina_Actor& act)
{

	Lina_ActorComponent::AttachToActor(act);

	m_Actor->Lina->RenderingEngine()->AddSpotLight(this);
}
