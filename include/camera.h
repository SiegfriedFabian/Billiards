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
	}


};
#endif