#include "Core/ApplicationDelegate.hpp"
#include "Core/Application.hpp"

namespace Lina
{
	System* ApplicationDelegate::GetSystem()
	{
		return m_app->GetSystem();
	}
} // namespace Lina
