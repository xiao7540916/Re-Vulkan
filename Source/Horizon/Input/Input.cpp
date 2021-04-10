#include "Input.h"

#include "Horizon/Core/Window.h"
#include "Horizon/Core/Application.h"

namespace Horizon
{
	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		Window* window = Application::Get().GetMainWindow();
		int state = glfwGetKey(static_cast<GLFWwindow*>(window->GetGLFWwindow()), static_cast<int32>(keyCode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseButtonID button)
	{
		Window* window = Application::Get().GetMainWindow();
		int state = glfwGetMouseButton(static_cast<GLFWwindow*>(window->GetGLFWwindow()), (int)button);
		return state == GLFW_PRESS;
	}

	Vector2 Input::GetMousePos()
	{
		Window* window = Application::Get().GetMainWindow();
		double x, y;
		glfwGetCursorPos(static_cast<GLFWwindow*>(window->GetGLFWwindow()), &x, &y);
		return Vector2((float)x, (float)y);
	}
}
