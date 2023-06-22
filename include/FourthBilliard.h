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

	float lineWidth = 1.0;	// default line width for all objects



	FourthBilliard();

	void addTrajectory(vec2 start, vec3 color);

	void iterate(int batch, int nIter);

	// change current starting points and polygon dependant on mode and mouse position
	void updateCoords(vec2_d mouse, GLFWwindow* window);

	void resetLineWidth();	// resets the line width of all objects

	bool polygonClosed();

	void snapToGrid();

	void drawTrajectories(Shader& shaderProgram);
	void drawPolygon(Shader& shaderProgram);

	std::vector<vec2_d> getStartPoints();

	void clearPolygon();
	void resetTrajectories();

	void updatePolygon(); // this function is used to update the polygon, ones user has made Input

	void resetTrajectory(int i);

	void removeTrajectory(int i);

	// make polygon regular by calling the function from Polygon class
	void makeRegularNPoly(int n);
};
