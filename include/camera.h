#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"myVectors.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	vec2 Position = vec2(0.0,0.0);
	float Zoom = 0.01f;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	unsigned int Width;
	unsigned int Height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.002f;
	float sensitivity = 100.0f;

	// Camera constructor to set up initial values
	Camera(unsigned int width, unsigned int height, vec2 position) {
		Width = width;
		Height = height;
		Position = position;
		Zoom = 0.1f;
	}

	// Handles camera inputs
	
	void Inputs(GLFWwindow* window, float deltaT = 1.0f) {
		// Handles key inputs
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			Position.y += deltaT * speed / Zoom;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS|| glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			Position.x -= deltaT * speed / Zoom;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			Position.y -= deltaT * speed / Zoom;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			Position.x += deltaT * speed / Zoom;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			Zoom *= 1.0f + (deltaT * 0.01);
		}
		if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			Zoom *= 1.0f - (deltaT * 0.03);
		}

		//if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		//{
		//	Position += speed * -Up;
		//}
		//if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		//{
		//	speed = 0.4f;
		//}
		//else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		//{
		//	speed = 0.1f;
		//}


		// Handles mouse inputs
		//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		//{
		//	// Hides mouse cursor
		//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//	// Prevents camera from jumping on the first click
		//	if (firstClick)
		//	{
		//		glfwSetCursorPos(window, (width / 2), (height / 2));
		//		firstClick = false;
		//	}

		//	// Stores the coordinates of the cursor
		//	double mouseX;
		//	double mouseY;
		//	// Fetches the coordinates of the cursor
		//	glfwGetCursorPos(window, &mouseX, &mouseY);

		//	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		//	// and then "transforms" them into degrees 
		//	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		//	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		//	// Calculates upcoming vertical change in the Orientation
		//	glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		//	// Decides whether or not the next vertical Orientation is legal or not
		//	if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		//	{
		//		Orientation = newOrientation;
		//	}

		//	// Rotates the Orientation left and right
		//	Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		//	// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		//	glfwSetCursorPos(window, (width / 2), (height / 2));
		//}
		//else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		//{
		//	// Unhides cursor since camera is not looking around anymore
		//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//	// Makes sure the next time the camera looks around it doesn't jump
		//	firstClick = true;
		//}


	}


};
#endif