#pragma once

#include "Trajectory.h"
#include "shape.h"
#include <utility>


struct FourthBilliard
{
public:
	std::vector<Trajectory>	trajectories;
	Poly					polygon;

	int mode=0;




	FourthBilliard();

	void addTrajectory(vec2 start, vec3 color);

	void iterate(int batch, int nIter);

	// change current starting points and polygon dependant on mode and mouse position
	void updateCoords(vec2_d mouse, GLFWwindow* window);

	bool polygonClosed();

	void snapToGrid();

	void drawTrajectories(Shader& shaderProgram);
	void drawPolygon(Shader& shaderProgram);

	std::vector<vec2> getStartPoints();

	void clearPolygon();
	void resetTrajectories();

	void resetTrajectory(int i);

	// make polygon regular by calling the function from Polygon class
	void makeRegularNPoly(int n);
};
