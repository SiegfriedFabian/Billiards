#ifndef TRAJECTORY_H
#define TRAJECTORY_H
#include "shape.h"
#include <utility>
#include "myVectors.h"

struct Circle_d {
public:
	vec2_d center;
	double radius;

	Circle_d(vec2_d c = vec2_d(), double r = 0) {
		center = c;
		radius = r;
	}
};

struct Trajectory : public shape{
public:
	float size = 3.0f;	// size of points
	int count = 0;		// how many vertices we have

	//std::vector<vec2_d>	vertexData_d; // store trajectory as doubles // should not be neccessary anymore, as this variable now exists in shape

	Poly* polygon; // pointer to polygon on which to reflect TODO: consider smartpointers

	Trajectory();
	Trajectory(vec2_d start, Poly* p, vec3 color);

	void Create();			
	void Draw(Shader& shaderProgram);	// draw function, similar to points
	void iterate();						// calculate the next iteration point using 4th billiard rule
	void iterate(int batch, int nIter);

	vec2 getStartingPoint();

	void Reset();						// resets the trajectory, only first point remains
	void Reset(vec2_d v);					// sets new starting point and clears data

private:
	vec2_d lineIntersection(const vec2_d& p0, const vec2_d& v0, const vec2_d& p1, const vec2_d& v1);
	int numberOfIntersections(const vec2_d lineDirection, const vec2_d start);  // TODO consider moving this into Polygon class?

	std::pair<int, int> computeTangentVertices(const vec2_d pos);

	Circle_d computeOsculatingCircle(const vec2_d pos, const vec2_d p1, const vec2_d p2);

	// calculates the "left" tangentpoint on the circle for a point.
	vec2_d tangentPointCircle(const vec2_d pos, const Circle_d circle);



	// for each point of the polygon calculate the tangent to the circle and check, how many intersection this line has with the polygon
	// first point in tuple is from polygon, second is on the circle
	std::pair<vec2_d, vec2_d> tangentPolyCircle(const int& startVertex, const Circle_d& circle, const vec2_d thirdTangentDirection);


};

#endif