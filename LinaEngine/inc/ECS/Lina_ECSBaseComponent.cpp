#include "pch.h"
#include "Lina_ECSBaseComponent.hpp"

static uint32_t componentID = 0;

uint32_t Lina_ECSBaseComponent::nextID()
{
	return componentID++;
}
