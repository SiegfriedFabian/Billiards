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
	float					t0, t1;
	std::vector<float>		t_trajectory;
	// bool					scene_has_changed = false;
	// bool					polygon0And2AreClosed = false;

	int mode=0;

	SymplecticBilliardSystem();
	SymplecticBilliardSystem(float t0, float t1); //Constructor that sets initial values
	void Create();
	// void addTrajectory(vec2 start, vec3 color);
	void reset();
	// void setInitialValues(float t0, float t1);
	void iterateSymplecticBilliards(int batch, int nIter);

	// change current starting points and polygon dependant on mode and mouse position
	void updateCoords(vec2 mouse, GLFWwindow* window);

	void translatepolygon0(vec2 &deltaPos);
	void translatepolygon1(vec2 &deltaPos);
	void translatepolygons(vec2 &mouse, vec2 &deltaPos);

	void centerPolygonsAt(vec2 mouse);

	// bool polygon0And2Closed();

	void closepolygon0();
	void closepolygon1();
	void closepolygon0And2();


	void ClearPolygon0();
	void ClearPolygon1();
	void copyPolygon0OntoPolygon1();
	void copyPolygon1OntoPolygon0();

	void editVertexPositionPolygons(GLFWwindow *window, vec2& pos);
	void editVertexPositionpolygon0(GLFWwindow *window, vec2& pos);
	void editVertexPositionpolygon1(GLFWwindow *window, vec2& pos);
	void editVertexPositionX0(GLFWwindow *window, vec2& pos);
	void editVertexPositionX1(GLFWwindow *window, vec2& pos);
	void editVertexPositionX0X1(GLFWwindow *window, vec2& pos);

	void snapToGridpolygon0();
	void snapToGridpolygon1();
	void snapToGrid();

	void drawPolygons(Shader& shaderProgram);
	// void drawPolygons(Shader& shaderProgram, vec3& p0Color, vec3& p1Color);
	void drawInitialValues(Shader& shaderProgram);
	void drawTrajectories(Shader& shaderProgram);

	std::vector<vec2> getStartPoints();

	void clearpolygon0();
	void clearpolygon1();
	void clearPolygons();
	void resetTrajectories();
};

vec2 SymplecticBilliardMap(Poly polygon0, float t0, Poly polygon1, float t1); 