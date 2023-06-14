#include "FourthBilliard.h"
#include <exception>
#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"
#include<glad/glad.h>
#include"GLFW/glfw3.h"
	


FourthBilliard::FourthBilliard()
{
	/* For debugging
	p1.vertexData.push_back(vec2());
	p2.vertexData.push_back(vec2());
	p3.vertexData.push_back(vec2());
	p1.color = vec3(1.0, 0.0, 0.0);
	p2.color = vec3(0.0, 0.0, 1.0);
	p3.color = vec3(0.5, 0.5, 0.5);
	p1.Create();
	p2.Create();
	p3.Create();
	*/

	// create initial polygon
	polygon.color = vec3(0.0, 1.0, 1.0);
	polygon.lineWidth = 2;
	polygon.AddVertex(vec2(0.0, 0.0));
	polygon.AddVertex(vec2(5.0, 0.0));
	polygon.AddVertex(vec2(0.0, 5.0));



	// Adding objects to the scene
	polygon.Create();

	this->makeRegularNPoly(10);

	// add initial trajectory
	addTrajectory(vec2(5, 5), vec3(0.0, 1.0, 0.0));

}




void FourthBilliard::addTrajectory(vec2 start, vec3 color) {
	trajectories.push_back(Trajectory(vec2_d(start), &polygon, color));
	trajectories.back().Create();
}


void FourthBilliard::iterate(int batch, int nIter)
{
	for (auto& traj : trajectories)
	{
		traj.iterate(batch, nIter);
	}
}

void FourthBilliard::updateCoords(vec2_d mouse, GLFWwindow* window)
{
	if (mode == 0) {
		this->polygon.onMouse(window, mouse);
		if (polygon.vertexChange)
		{
			std::cout << "Data: " << std::endl;
			std::cout << &polygon.vertexData[0].x << std::endl;
			std::cout << &trajectories[0].vertexData[0].x << std::endl;
			this->resetTrajectories();
			this->polygon.vertexChange = false;
		}
		return;
	}
	if (0 < mode < trajectories.size() + 1)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			this->trajectories[mode - 1].Reset(mouse);
		}
	}
}

bool FourthBilliard::polygonClosed()
{
	return polygon.closed;
}

void FourthBilliard::snapToGrid()
{
	polygon.roundData();
	resetTrajectories();
}

void FourthBilliard::drawTrajectories(Shader& shaderProgram)
{
	for (auto& traj : trajectories) {
		traj.Draw(shaderProgram);
	}
}

void FourthBilliard::drawPolygon(Shader& shaderProgram)
{
	polygon.Draw(shaderProgram);
}

std::vector<vec2_d> FourthBilliard::getStartPoints()
{
	std::vector<vec2_d> startingPoints;
	for (auto& traj : trajectories) {
		startingPoints.push_back(traj.getPos());
	}
	return startingPoints;
}

void FourthBilliard::clearPolygon()
{
	polygon.Clear();
	polygon.AddVertex(vec2(0, 0));
	polygon.AddVertex(vec2(1, 0));
	polygon.vertexChange = true;
}

void FourthBilliard::resetTrajectories()
{
	for (auto& traj : this->trajectories) {
		traj.Reset();
	}
}

void FourthBilliard::resetTrajectory(int i)
{
	if (i < 0 || i > this->trajectories.size()) { std::cout << "does not work";  return; } // check if i is valid index of trajectory. 
	std::cout << "works";
	this->trajectories[i].Reset();
}

void FourthBilliard::removeTrajectory(int i)
{
	if (0 < i < trajectories.size()) {
		trajectories.erase(trajectories.begin() + i);
	}
}

void FourthBilliard::makeRegularNPoly(int n)
{
	this->polygon.makeRegularNPoly(n);
	this->resetTrajectories(); // reset trajectories, since polygon has changed
}

