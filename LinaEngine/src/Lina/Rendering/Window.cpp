#include "LinaPch.hpp"
#include "Window.hpp"

LinaEngine::Window::Window(const WindowProps & wp)
{
	LINA_CORE_TRACE("[Constructor] -> Window ({0})", typeid(*this).name());
	m_WindowProps = wp;
}
