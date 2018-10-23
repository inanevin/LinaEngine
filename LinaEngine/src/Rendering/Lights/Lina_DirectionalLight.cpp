#include "pch.h"
#include "Rendering/Lights/Lina_DirectionalLight.h"
#include "Game/Lina_Actor.h"

void Lina_DirectionalLight::AttachToActor(Lina_Actor& act)
{
	Lina_ActorComponent::AttachToActor(act);

	m_Actor->Lina->RenderingEngine()->AddDirectionalLight(this);

}
