#ifndef SHAPE_CLASS_H
#define SHAPE_CLASS_H
#include<iostream>
#include"myVectors.h"
// #include<glad/glad.h>
// #include<vector>
// #include"GLFW/glfw3.h"
#include "camera.h"
#include "shaderClass.h"
// #include <algorithm>



vec2 mousePosCoord(GLFWwindow* window, float SCR_WIDTH, float SCR_HEIGHT, Camera camera);

struct Circle {
public:
	vec2 center;
	float radius;

	Circle(vec2 c = vec2(), float r = 0) {
		center = c;
		radius = r;
	}
};


struct IndexDistPair {
	int index;
	float dist;
};

IndexDistPair computeClosestIndex(const std::vector<vec2>& vertices, const vec2& pos);


// Distance of p to a line segment a---b
float sdSegment(const vec2 p, const vec2 a, const vec2 b);

// Shape is a class of basic methods and variables from which we will derive the classes point, line and polygon
struct shape {
public:
	unsigned int		vao, vbo;						// addresses to allocate buffers for openGL
	std::vector<vec2>	vertexData;						// Contains the 2D points of the Objects
	std::vector<vec2>	directions;						// Contains the 2D points of the Objects
	bool				firstClickLeft = true;			// Variable to keep Track if left-mouse has already picked a closest vertex
	bool				firstClickRight = true;			// Variable to keep Track if right-mouse has already picked a closest vertex
	vec3				color;						// color of the shape 
	float				lineWidth = 1.0;
	bool				pickedVertex = false;
	bool				vertexChange = false;
	int					pickedVertexIndex;

	virtual void Create();

	virtual void Draw();

	virtual void updateDirections();

	void roundData();

	void translateBy(vec2& deltaPos);

	int computeClosestIndex(GLFWwindow* window, vec2 mousePos);
	IndexDistPair computeClosestIndexDistance(GLFWwindow *window, vec2 mousePos);

	// Mouse click event
	void dragDropTo(GLFWwindow* window, vec2& pos);
};

struct Poly:public shape {
public:
	bool				closed = false;

	void Draw(Shader& shaderProgram);
	void Draw(Shader &shaderProgram, vec3 &col);

	void AddVertex(vec2 p);
	vec2 ParamEdgeRatio(float t);

	void ClosePolygon();

	void Clear();


	// Mouse click event
	void onMouse(GLFWwindow* window, vec2& mousePos);

	float distance(vec2  &p);

	int closestEdge(const vec2& p);

		// turn current Polygon into Regular n-polygon around 0,0 with radius 1
	void makeRegularNPoly(int n, float radius = 1.0f);
	void makeNStar(int n, float innerRadius = 1.0f, float outerRadius = 2.0f);
	void makeRegularNStar(int n, float radius = 1.0f);
	void turnIntoQuad();
	vec2 computeCenter();
	void center();
	void scaleBy(float scaleFactor);
};


struct Line:public shape {
public:
	vec2				direction = vec2();
	bool				drawLine = true;

	Line();
	void set(vec2 start, vec2 end);

	void setStart(vec2 start);

	void setEnd(vec2 end);

	void Create();

	void Draw(Shader& shaderProgram);
	void Draw(Shader &shaderProgram, vec3 &col);

	void round();
	
	void updateDirections();

	void setClosestVertexTo(GLFWwindow* window, vec2 & mouse);

};

struct Points : public shape{

	float	size = 10.0f;

	vec2 getPos();

	void Create();

	void Draw(Shader& shaderProgram);
	void Draw(Shader &shaderProgram, vec3 &col);

	void clear();

	void projectOntoPolygon( Poly& polygon);

};

vec2 lineIntersection(vec2& p0, vec2& v0, vec2& p1, vec2& v1);
float firstIntersection(Poly& polygon, vec2 p, vec2 dir);
vec2 projectOntoLine(const vec2& pos, const Line& line);
vec2 projectOntoPolygon(const vec2& pos, Poly& polygon);
float inverseParamPolygon(const vec2& pos, Poly& polygon);

#endif
