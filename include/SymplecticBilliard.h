#pragma once

#include "Trajectory.h"
#include "shape.h"
#include <utility>


struct SymplecticBilliardSystem
{
public:
	Poly					polygon0, polygon1;
	vec3 					polygon0Color  = vec3(216,27,96), polygon1Color;
	Poly					oddTraj, evenTraj;
	Points					x0, x1;
	double					t0, t1;
	std::vector<double>		t_trajectory;
	// bool					scene_has_changed = false;
	// bool					polygon0And2AreClosed = false;

	int mode=0;

	SymplecticBilliardSystem();
	SymplecticBilliardSystem(double t0, double t1); //Constructor that sets initial values
	void Create();
	// void addTrajectory(vec2_d start, vec3 color);
	void reset();
	// void setInitialValues(double t0, double t1);
	void iterateSymplecticBilliards(int batch, int nIter);

	// change current starting points and polygon dependant on mode and mouse position
	void updateCoords(vec2_d mouse, GLFWwindow* window);

	void translatepolygon0(vec2_d &deltaPos);
	void translatepolygon1(vec2_d &deltaPos);
	void translatepolygons(vec2_d &mouse, vec2_d &deltaPos);

	void centerPolygonsAt(vec2_d mouse);

	// bool polygon0And2Closed();

	void closepolygon0();
	void closepolygon1();
	void closepolygon0And2();


	void ClearPolygon0();
	void ClearPolygon1();
	void copyPolygon0OntoPolygon1();
	void copyPolygon1OntoPolygon0();

	void editVertexPositionPolygons(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionpolygon0(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionpolygon1(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionX0(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionX1(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionX0X1(GLFWwindow *window, vec2_d& pos);

	void snapToGridpolygon0();
	void snapToGridpolygon1();
	void snapToGrid();

	void drawPolygons(Shader& shaderProgram);
	// void drawPolygons(Shader& shaderProgram, vec3& p0Color, vec3& p1Color);
	void drawInitialValues(Shader& shaderProgram);
	void drawTrajectories(Shader& shaderProgram);

	std::vector<vec2_d> getStartPoints();

	void clearpolygon0();
	void clearpolygon1();
	void clearPolygons();
	void resetTrajectories();
};

vec2_d SymplecticBilliardMap(Poly polygon0, double t0, Poly polygon1, double t1); 