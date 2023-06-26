#pragma once

#include "shape.h"
#include <utility>

struct lightSource{
public:
	Points p;
	std::vector<vec2> rays;

	lightSource(int n);
};



struct EuclideanBilliardSystem
{
public:
	Poly					polygon;
	vec3 					polygonColor  = vec3(216,27,96);
	std::vector<Poly>	    trajectories;
	Points					sources;


	int mode=0;

	EuclideanBilliardSystem();
	EuclideanBilliardSystem(vec2_d pos, vec2_d direction); //Constructor that sets initial values
	void Create();
	// void addTrajectory(vec2_d start, vec3 color);
	void reset();
	// void setInitialValues(double t0, double t1);
	void iterateEuclideanBilliards(int batch, int nIter);

	// change current starting points and polygon dependant on mode and mouse position
	void updateCoords(vec2_d mouse, GLFWwindow* window);

	void translatepolygon(vec2_d &deltaPos);

	void centerPolygonAt(vec2_d mouse);

	// bool polygon0And2Closed();

	void closepolygon();
	void clearPolygon();


	void editVertexPositionPolygon(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionX0(GLFWwindow *window, vec2_d& pos);
	void editVertexPositionX1(GLFWwindow *window, vec2_d& pos);
	void snapToGrid();
	void drawPolygon(Shader& shaderProgram);
	void drawPolygon(Shader& shaderProgram, vec3& p0Color, vec3& p1Color);
	void drawInitialValues(Shader& shaderProgram);
	void drawTrajectories(Shader& shaderProgram);
	std::vector<vec2_d> getStartPoints();
	void resetTrajectories();
};

vec2_d EuclideanBilliardMap(Poly polygon0, double t0, Poly polygon1, double t1); 