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
		polygon.onMouse(window, mouse);
		if (polygon.vertexChange)
		{
			resetTrajectories();
			polygon.vertexChange = false;
		}
		return;
	}
	if (0 < mode < trajectories.size() + 1)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			trajectories[mode - 1].Reset(mouse);
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

std::vector<vec2> FourthBilliard::getStartPoints()
{
	std::vector<vec2> startingPoints;
	for (auto& traj : trajectories) {
		startingPoints.push_back(traj.getStartingPoint());
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
	for (auto& traj : trajectories) {
		traj.Reset();
	}
}

void FourthBilliard::makeRegularNPoly(int n)
{
	this->polygon.makeRegularNPoly(n);
	this->resetTrajectories(); // reset trajectories, since polygon has changed
}

