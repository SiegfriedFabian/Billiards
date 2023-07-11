#ifndef SHAPE_CLASS_H
#define SHAPE_CLASS_H
#include<iostream>
#include"myVectors.h"
// #include<glad/glad.h>
#include<vector>
// #include"GLFW/glfw3.h"
#include "camera.h"
#include "shaderClass.h"
#include <algorithm>



vec2_d mousePosCoord(GLFWwindow* window, double SCR_WIDTH, double SCR_HEIGHT, Camera camera);

struct Circle {
public:
	vec2_d center;
	double radius;

	Circle(vec2_d c = vec2_d(), double r = 0) {
		center = c;
		radius = r;
	}
};


struct IndexDistPair {
	int index;
	double dist;
};

IndexDistPair computeClosestIndex(const std::vector<vec2_d>& vertices, const vec2_d & pos);


// Distance of p to a line segment a---b
double sdSegment(const vec2_d p, const vec2_d a, const vec2_d b);


struct Rectangle{
	public:
	        //////////////////Creation of the drawing Buffer and the texture to be drawn in///////////////////////////////
    GLuint textureFBO;
    GLuint texture;

    GLfloat vertices[12] = {
         1.0f,  0.0f,-1.0f,  // Top Right       
         1.0f,  0.0f, 1.0f,  // Bottom Right    
        -1.0f,  0.0f, 1.0f,  // Bottom Left     
        -1.0f,  0.0f,-1.0f,  // Top Left        
    };

    GLuint indices[6] = {  
        0, 1, 2,  // First Triangle
        2, 3, 0,  // Second Triangle
    };

	GLuint VBO, VAO, EBO;
	
	void create(float width, float height);
    void draw(Shader &shaderprogram, float width, float height, vec2* vertices); 
};

// Shape is a class of basic methods and variables from which we will derive the classes point, line and polygon
struct shape {

public:
	std::vector<vec2>	vertexData;						// Contains the 2D points of the Objects
	std::vector<vec2_d>	vertexData_d;						// Contains the 2D points of the Objects
	std::vector<vec2_d>	directions_d;						// Contains the 2D points of the Objects
	unsigned int		vao, vbo;						// addresses to allocate buffers for openGL
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

	vec2 getVertexData(int index);
	vec2_d getVertexData_d(int index);
	vec2_d getDirection(int index);

	void setVertexData(int index, vec2_d p);
	

	void roundData();

	void translateBy(vec2_d& deltaPos);

	int computeClosestIndex(GLFWwindow* window, vec2_d mousePos);
	IndexDistPair computeClosestIndexDistance(GLFWwindow *window, vec2_d mousePos);

	// Mouse click event
	void dragDropTo(GLFWwindow* window, vec2_d& pos);
};

struct Poly: public shape {
public:
	bool				closed = false;

	void Draw(Shader& shaderProgram);
	void Draw(Shader &shaderProgram, vec3 &col);

	void AddVertex(vec2_d p);
	vec2_d ParamEdgeRatio(double t);
	vec2_d ParamLength(double t);
	
	void ClosePolygon();

	void Clear();


	// Mouse click event
	void onMouse(GLFWwindow* window, vec2_d & mousePos);

	double distance(vec2_d  &p);

	int closestEdge(const vec2_d & p);

		// turn current Polygon into Regular n-polygon around 0,0 with radius 1
	void makeRegularNPoly(int n, double radius = 1.0f);
	void makeNStar(int n, double innerRadius = 1.0f, double outerRadius = 2.0f);
	void makeRegularNStar(int n, double radius = 1.0f);
	void turnIntoQuad();
	vec2_d computeCenter();
	void center();
	void scaleBy(double scaleFactor);
	void updateData(); // used to match vertexData_d with vertexData
};


struct Line:public shape {
public:
	vec2_d				direction_d = vec2_d();
	bool				drawLine = true;

	Line();
	void set(vec2_d start, vec2_d end);

	void setStart(vec2_d start);

	void setEnd(vec2_d end);

	void Create();

	void Draw(Shader& shaderProgram);
	void Draw(Shader& shaderProgram, vec3 &col);

	void round();
	
	void updateDirections();

	void setClosestVertexTo(GLFWwindow* window, vec2_d & mouse);

};

struct Points : public shape{

	float	size = 10.0f;

	vec2_d getPos();

	void Create();

	void Draw(Shader& shaderProgram);
	void Draw(Shader &shaderProgram, vec3 &col);
	
	void AddVertex(vec2_d p);

	void clear();

	void projectOntoPolygon( Poly& polygon);

};

vec2_d lineIntersection(vec2_d & p0, vec2_d & v0, vec2_d & p1, vec2_d & v1);
double firstIntersection(Poly& polygon, vec2_d p, vec2_d dir);
vec2_d projectOntoLine(const vec2_d & pos, const Line& line);
vec2_d projectOntoPolygon(const vec2_d & pos, Poly& polygon);
double inverseParamPolygon(const vec2_d & pos, Poly& polygon);

#endif
