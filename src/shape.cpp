#include "shape.h"

double EPS = 10e-5;
const double PI = 3.14159265358979323846;

vec2_d mousePosCoord(GLFWwindow *window, double SCR_WIDTH, double SCR_HEIGHT, Camera camera)
{
	double pX, pY;
	glfwGetCursorPos(window, &pX, &pY);
	double cX = (2.0f * pX / SCR_WIDTH - 1.0f)* (SCR_WIDTH/SCR_HEIGHT) / camera.Zoom;	// flip y axis
	double cY = (1.0f - 2.0f * pY / SCR_HEIGHT)/ camera.Zoom ;
	
	return vec2_d(cX + camera.Position.x, cY + camera.Position.y);
}

vec2_d lineIntersection(vec2_d& p0, vec2_d& v0, vec2_d& p1, vec2_d& v1) {
	vec2_d deltaP = p0 - p1;
	if (abs(det(v0, v1)) < EPS) return INFINITY;
	vec2_d t = 1 / (v1.x * v0.y - v0.x * v1.y) * vec2_d(dot(deltaP, vec2_d(v1.y, -v1.x)), dot(deltaP, vec2_d(v0.y, -v0.x)));
	return t; // Returns t0 and t1 which both describe the intersecting point p0 + t0*dir0 = p1 + t1*dir1
}

inline double sdSegment(const vec2_d p, const vec2_d a, const vec2_d b)
{
	vec2_d pa = p - a, ba = b - a;
	double h = std::clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h);
}

inline double sdSegment(const vec2_d p, Line l)
{
	vec2_d pa = p - l.getVertexData_d(0), ba = l.getVertexData_d(1) - l.getVertexData_d(0);
	double h = std::clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h);
}



// float firstIntersection(Poly &polygon, vec2 p, vec2 dir)
// {
// 	float minDist = INFINITY;
// 	int minIndex = INFINITY;
// 	float res = 0;
// 	//std::cout << "jhu" << std::endl;
// 	for (int i = 0; i < polygon.directions.size(); i++)
// 	{
// 		vec2 t = lineIntersection(p, dir, polygon.vertexData[i], polygon.directions[i]);
// 		if (0.0001 < t.x && t.x < minDist && EPS < t.y && t.y < 0.999) {
// 			//std::cout << "At direction " << i << " = (" << directions[i].x << ", " << directions[i].y << "), we have t = " << t.x << ", " << t.y << ", " << length(directions[i]) << std::endl;
// 			minDist = t.x;
// 			minIndex = i;
// 			res = float(i) + t.y;
// 		}
// 	}
// 	return res; // Returns the distance from p, i.e. the intersection is at p + minDist*dir
// }

void shape::Create()
{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo); // Generate 1 vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Enable the vertex Attribute arrays
		glEnableVertexAttribArray(0);
		// Map attribute array 0 to the vertex data of the interleaved vbo
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0)); // first two floats are position, the next three are color
}

void shape::Draw(){}

void shape::updateDirections() 
{
		for (int i = 0; i < directions_d.size(); i++)
		{
			directions_d[i] = vertexData_d[i + 1] - vertexData_d[i];
		}
}
vec2 shape::getVertexData(int index)
{
        return vertexData[index];
}
vec2_d shape::getVertexData_d(int index)
{
        return vertexData_d[index];
}
void shape::setVertexData(int index, vec2_d p)
{
	vertexData_d[index] = p;
	vertexData[index] = p.toFloat();
	
}

void shape::roundData()
{
		for (int i = 0; i < vertexData_d.size(); i++)
		{
			vertexData[i] = round(vertexData[i]);
			vertexData_d[i] = round(vertexData_d[i]);
		}
		updateDirections();
}

int shape::computeClosestIndex(GLFWwindow *window, vec2_d mousePos)
{
		int closest_i = 0;
		double minDist = INFINITY;
		for (int i = 0; i < vertexData.size(); i = i + 1)
		{
			double distToVertex = length(vertexData_d[i] - mousePos);
			if (distToVertex < minDist) {
				minDist = distToVertex;
				closest_i = i;
			}
		}
		//std::cout << "Closest index: " << closest_i << std::endl;
		return closest_i;
}

void shape::translateBy(vec2_d &deltaPos)
{
	for (int i = 0; i < vertexData.size(); i++)
	{
		vertexData_d[i] = vertexData_d[i] + deltaPos;
		vertexData[i] = vertexData[i] + deltaPos.toFloat();
	}
	
}

IndexDistPair shape::computeClosestIndexDistance(GLFWwindow *window, vec2_d mousePos)
{
		int closest_i = 0;
		double minDist = INFINITY;
		for (int i = 0; i < vertexData.size(); i = i + 1)
		{
			double distToVertex = length(vertexData_d[i] - mousePos);
			if (distToVertex < minDist) {
				minDist = distToVertex;
				closest_i = i;
			}
		}
		//std::cout << "Closest index: " << closest_i << std::endl;
		return {closest_i, minDist};
}


void shape::dragDropTo(GLFWwindow *window, vec2_d& pos)
{ 
		// Right-Click behaviour
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
			pickedVertex = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && vertexData.size() > 0) {
			if (!pickedVertex) {
				pickedVertexIndex = computeClosestIndex(window, pos);
				// std::cout << pickedVertexIndex << std::endl;
				pickedVertex = true;
			}

			if (length(vertexData_d[pickedVertexIndex] - pos) > EPS) {
				vertexData_d[pickedVertexIndex] = pos;
				vertexData[pickedVertex] = pos.toFloat();
				vertexChange = true;
			}
			updateDirections();
		}
}

void Poly::Draw(Shader &shaderProgram){
		if (vertexData.size() > 0) {
			glLineWidth(lineWidth);
			shaderProgram.setUniform(color, "vertexColor");
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertexData.size() * 2 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
			glBindVertexArray(vao);
			glDrawArrays(GL_LINE_STRIP, 0, vertexData.size());
			glLineWidth(1);
		}
	}

void Poly::Draw(Shader &shaderProgram, vec3 &col){
		if (vertexData.size() > 0) {
			glLineWidth(lineWidth);
			shaderProgram.setUniform(col, "vertexColor");
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertexData.size() * 2 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
			glBindVertexArray(vao);
			glDrawArrays(GL_LINE_STRIP, 0, vertexData.size());
			glLineWidth(1);
		}
	}

void Poly::AddVertex(vec2_d p) {
		vertexData.push_back(p.toFloat());
		vertexData_d.push_back(p);
		int i = vertexData.size() - 1;
		if (i > 0) {
			directions_d.push_back(vertexData_d[i] - vertexData_d[i - 1]);
		}
	}


vec2_d Poly::ParamEdgeRatio(double t){
		double tMod = fmod(t, directions_d.size());
		int edgeIndex = int(tMod);
		double tlin = t - int(tMod);
		return tlin * vertexData_d[edgeIndex + 1] + (1.0f - tlin) * vertexData_d[edgeIndex];
	}

vec2_d Poly::ParamLength(double t){
		t = fmod(t,1);
		
        return vec2_d();
    }

void Poly::ClosePolygon(){
		if (!closed) {
			vertexData.push_back(vertexData[0]);
			vertexData_d.push_back(vertexData_d[0]);
			// std::cout << "before direction update" << std::endl;

			directions_d.push_back(vertexData_d[0] - vertexData_d[vertexData.size() - 2]);
			closed = true;
		}
	}
void Poly::Clear() {
		vertexData.clear();
		vertexData_d.clear();
		directions_d.clear();
		closed = false;
	}
void Poly::onMouse(GLFWwindow *window, vec2_d &mousePos){ // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
		//int w, h;
		//glfwGetWindowSize(window, &w, &h);

		// Left-Click behaviour
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			firstClickLeft = true;
		}
		if (!closed && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && vertexData.size() > 1) {
			ClosePolygon();
		}
		if (!closed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && firstClickLeft && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			AddVertex(mousePos);
			firstClickLeft = false;
		}

		// Right-Click behaviour
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
			pickedVertex = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && vertexData.size() > 0) {
			if (!pickedVertex) {
				pickedVertexIndex = computeClosestIndex(window, mousePos);
				pickedVertex = true;
			}

			if (length(vertexData_d[pickedVertexIndex] - mousePos) >  EPS) {
				vertexData[pickedVertexIndex] = mousePos.toFloat();
				vertexData_d[pickedVertexIndex] = mousePos;
				vertexChange = true;
			}

			if (closed && pickedVertexIndex == 0) {
				int lastVert = vertexData.size() - 1;
				vertexData[lastVert] = mousePos.toFloat();
				vertexData_d[lastVert] = mousePos;
			}
			updateDirections();
		}
	}

double Poly::distance(vec2_d &p){
	double dist = INFINITY;
	for (int i = 0; i < vertexData.size()-1; i++)
	{
		double d = sdSegment(p, vertexData_d[i], vertexData_d[i+1]);
		if (d < dist)
		{
			dist = d;
		}
		
	}
	return dist;
}

int Poly::closestEdge(const vec2_d &p){
		int minEdge = 0;
		double minDist = INFINITY;
		for (int i = 0; i < directions_d.size(); i++)
		{
			double currentDist = sdSegment(p, vertexData_d[i], vertexData_d[i + 1]);
			if (currentDist < minDist) {
				minDist = currentDist;
				minEdge = i;
			}
		}
		// std::cout << minDist << std::endl;
		return minEdge;
	}

void Poly::makeNStar(int n, double innerRadius, double outerRadius){
    if (n <= 2) return; // if n <= 2 then dont do anything as we cant have a polygon
    this->Clear(); // remove all curent vertices
    for (int i = 0; i < n; i++) {
        this->AddVertex(vec2_d(outerRadius * cos(2 * PI * i / n ), outerRadius * sin(2 * PI * i / n))); // add new vertices)
        this->AddVertex(vec2_d(innerRadius * cos(2 * PI * i / n + PI / n), innerRadius * sin(2 * PI * i / n + PI / n))); // add new vertices)
    }
    this->ClosePolygon(); // close the polygon again
}

void Poly::makeRegularNPoly(int n, double radius){
    if (n <= 2) return; // if n <= 2 then dont do anything as we cant have a polygon
    this->Clear(); // remove all curent vertices
    for (int i = 0; i < n; i++) {
        this->AddVertex(vec2_d(radius * cos(2 * PI * i / n), radius * sin(2 * PI * i / n))); // add new vertices
    }
    this->ClosePolygon(); // close the polygon again
}

void Poly::makeRegularNStar(int n, double radius){
	makeNStar(n, (0.5 * radius)/ cos(PI/(n)), radius);
}

void Poly::turnIntoQuad()
{
	Clear();
	AddVertex(vec2_d(0.0,0.0));
	AddVertex(vec2_d(4.0,0.0));
	AddVertex(vec2_d(1.0,3.0));
	AddVertex(vec2_d(0.0,2.0));
	ClosePolygon();
}

vec2_d Poly::computeCenter()
{
	vec2_d avg = vec2_d(0.,0.);
	if(closed){
		for (int i = 0; i < vertexData.size()-1; i++)
		{
			avg = avg + vertexData_d[i];
		}
    	return (1.0/(vertexData_d.size()-1)) * avg;

	} else {
		for (int i = 0; i < vertexData.size(); i++)
		{
			avg = avg + vertexData_d[i];
		}
    	return (1.0/vertexData.size()) * avg;
	}
}

void Poly::center()
{
	vec2_d avg = computeCenter();
	for (int i = 0; i < vertexData.size(); i++)
	{
		vertexData[i] = vertexData[i] - avg.toFloat();
		vertexData_d[i] = vertexData_d[i] - avg;
	}
	
}

void Poly::updateData()
{
	bool wasClosed = false;
	if (closed) {
		vertexData.pop_back(); // if polygon was closed, we have to manually close it again
		wasClosed = true;
	}
	// this could be done more intelligently, since vertexData is already correct, but this works for now
	std::vector<vec2> newData = vertexData;
	Clear();
	for (auto& vertex : newData) {
		AddVertex(vec2_d(vertex));
	}
	
	if (wasClosed) {
		ClosePolygon();
	}
}

Line::Line() {
    vertexData.push_back(vec2());
    vertexData.push_back(vec2());
    vertexData_d.push_back(vec2_d());
    vertexData_d.push_back(vec2_d());
    updateDirections();
}

void Line::set(vec2_d start, vec2_d end) {
    vertexData_d[0] = start;
    vertexData[0] = start.toFloat();
    vertexData_d[1] = end;
    vertexData[1] = end.toFloat();
    updateDirections();
}

void Line::setStart(vec2_d start) {
    vertexData_d[0] = start;
    vertexData[0] = start.toFloat();
    updateDirections();
}

void Line::setEnd(vec2_d end){
    vertexData_d[1] = end;
    vertexData[1] = end.toFloat();
    updateDirections();
}

void Line::Create() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo); // Generate 1 vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Enable the vertex Attribute arrays
    glEnableVertexAttribArray(0);
    // Map attribute array 0 to the vertex data of the interleaved vbo
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0)); // first two floats are position, the next three are color
}

void Line::Draw(Shader &shaderProgram) {
    if (drawLine) {
        shaderProgram.setUniform(color, "vertexColor");
        glLineWidth(lineWidth);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
        glLineWidth(1);
    }
}

void Line::Draw(Shader &shaderProgram, vec3 &col)
{
	    if (drawLine) {
        shaderProgram.setUniform(col, "vertexColor");
        glLineWidth(lineWidth);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
        glLineWidth(1);
    }
}

void Line::round() {
    if (vertexData.size() > 1) {
        for (int i = 0; i < vertexData.size(); i = i + 1)
        {
            double x_val = vertexData_d[i].x;
			x_val = floor(x_val+0.5);
            vertexData_d[i].x = x_val;
            vertexData[i].x = x_val;
            double y_val = vertexData_d[i].y;
			y_val = floor(y_val+0.5);
            vertexData_d[i].y = y_val;
            vertexData[i].y = y_val;
        }
        updateDirections();
    }
}

void Line::updateDirections(){
		if(vertexData.size() > 1){
			direction_d = vertexData_d[1] - vertexData_d[0];
		} 
	}

void Line::setClosestVertexTo(GLFWwindow *window, vec2_d &mouse) {
		vertexData_d[computeClosestIndex(window, mouse)] = mouse;
		vertexData[computeClosestIndex(window, mouse)] = mouse.toFloat();
	}

void Points::Create(){
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo); // Generate 1 vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Enable the vertex Attribute arrays
		glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);
		// Map attribute array 0 to the vertex data of the interleaved vbo
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,  2 * sizeof(float), reinterpret_cast<void*>(0)); // first two floats are position
		// Map attribute array 1 to the color data of the interleaved vbo
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
	}

void Points::Draw(Shader &shaderProgram){
		if (vertexData.size() > 0) {
			shaderProgram.setUniform(color, "vertexColor");
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertexData.size() * 2 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
			glBindVertexArray(vao);
			//glEnable(GL_POINT_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPointSize(size);
			glDrawArrays(GL_POINTS, 0, vertexData.size());
			//glDisable(GL_POINT_SMOOTH);
		}
	}

void Points::Draw(Shader &shaderProgram, vec3 &col)
{
	if (vertexData.size() > 0) {
		shaderProgram.setUniform(col, "vertexColor");
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * 2 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
		glBindVertexArray(vao);
		//glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPointSize(size);
		glDrawArrays(GL_POINTS, 0, vertexData.size());
		//glDisable(GL_POINT_SMOOTH);
	}
}

void Points::AddVertex(vec2_d p)
{
		vertexData.push_back(p.toFloat());
		vertexData_d.push_back(p);
}

vec2_d Points::getPos()
{
	return vertexData_d[0];
}

void Points::clear() {
		vertexData.clear();
		vertexData_d.clear();
	}

void Points::projectOntoPolygon(Poly &polygon){
		for (int i = 0; i < vertexData.size(); i++)
		{
			int edge = polygon.closestEdge(vertexData_d[i]);
			double l = length(polygon.directions_d[edge]);
			vec2_d d = normalize(polygon.directions_d[edge]);
			vertexData_d[i] = std::clamp(dot(vertexData_d[i] - polygon.vertexData_d[edge], d), 0.0, l) * d + polygon.vertexData_d[edge];
			vertexData[i] = vertexData_d[i].toFloat();
		}
}

inline vec2_d lineIntersection(const vec2_d& p0, const vec2_d& v0, const vec2_d& p1, const vec2_d& v1) {
	// Note: v0, v1 have to be normalized
	vec2_d deltaP = p0 - p1;
	if (abs(det(v0, v1)) < EPS) return INFINITY;
	vec2_d t = 1 / (v1.x * v0.y - v0.x * v1.y) * vec2_d(dot(deltaP, vec2_d(v1.y, -v1.x)), dot(deltaP, vec2_d(v0.y, -v0.x)));
	return t; // Returns t0 and t1 which both describe the intersecting point p0 + t0*dir0 = p1 + t1*dir1
}

double firstIntersection(Poly& polygon, vec2_d p, vec2_d dir) {
	double minDist = INFINITY;
	int minIndex = INFINITY;
	double res = 0;
	for (int i = 0; i < polygon.directions_d.size(); i++)
	{
		vec2_d t = lineIntersection(p, dir, polygon.vertexData_d[i], polygon.directions_d[i]);
		if (0.0001 < t.x && t.x < minDist &&  EPS < t.y && t.y < 0.999) {
			//std::cout << "At direction " << i << " = (" << directions[i].x << ", " << directions[i].y << "), we have t = " << t.x << ", " << t.y << ", " << length(directions[i]) << std::endl;
			minDist = t.x;
			minIndex = i;
			res = double(i) + t.y;
		}
	}
	return res; // Returns the distance from p, i.e. the intersection is at p + minDist*dir
}

vec2_d projectOntoLineClamped(const vec2_d &pos, const Line &line)
{
   	double l = length(line.direction_d);
	vec2_d d = normalize(line.direction_d);
	return std::clamp(dot(pos - line.vertexData_d[0], d), 0.0, l) * d + line.vertexData_d[0];
}

vec2_d projectOntoLine(const vec2_d &pos, const Line &line)
{
	vec2_d d = normalize(line.direction_d);
	return dot(pos - line.vertexData_d[0], d) * d + line.vertexData_d[0];
}

// inline vec2 projectOntoLine(const vec2& pos, const Line& line) {
// 	float l = length(line.direction);
// 	vec2 d = normalize(line.direction);
// 	return std::clamp(dot(pos - line.vertexData[0], d), 0.0f, l) * d + line.vertexData[0];
// }

vec2_d projectOntoPolygon(const vec2_d& pos, Poly& polygon) {
	int edge = polygon.closestEdge(pos);
	double l = length(polygon.directions_d[edge]);
	vec2_d d = normalize(polygon.directions_d[edge]);
	return std::clamp(dot(pos - polygon.vertexData_d[edge], d), 0.0, l) * d + polygon.vertexData_d[edge];
}

double inverseParamPolygon(const vec2_d &pos, Poly &polygon)
{
    int edge = polygon.closestEdge(pos);
	double l = length(polygon.directions_d[edge]);
	vec2_d d = normalize(polygon.directions_d[edge]);
	return std::clamp(dot(pos - polygon.vertexData_d[edge], d), 0.0, l) / l + float(edge);
}

inline float lineIntersectsSide(const vec2_d& x, const vec2_d& y, const Line& line) {
	vec2_d startingPiont = line.vertexData_d[0];
	float t = ((startingPiont.y - x.y) * line.direction_d.x + (startingPiont.x - x.x) * line.direction_d.y) / (line.direction_d.x * (y.y - x.y) + (y.x - x.x) * line.direction_d.y);
	return t;
}

// goes through the edges of the polygon and calculates wether the line hits or not
// stops if the line goes through one of the edges (this only works because since the polygon is convex there can only be two intersections at most)
inline int numberOfIntersections(const Poly& polygon, const Line& line) {
	int numberIntersections = 0;
	vec2_d lineDirection = normalize(line.direction_d);
	for (int i = 0; i < polygon.vertexData.size()-1; i++) {
		vec2_d directionSide = polygon.vertexData_d[i+1] - polygon.vertexData_d[i];
		float lengthSide = length(directionSide);
		directionSide = normalize(directionSide);
		vec2_d ts = lineIntersection(polygon.vertexData_d[i], directionSide, line.vertexData_d[0], lineDirection);
		//float t = ts.x * lengthSide; // use fomrula 
		if (abs(ts.x - lengthSide) < EPS || abs(ts.x) < EPS) {		// if we go through one of the vertices, we are potentially tangent, but only of we never intersect again.	
			numberIntersections = 1;
		}
		else { 
			if (ts.x < lengthSide && ts.x > 0) {
				return 2;			// if t is between 0 and 1, the line goes through the polygon
			}
		} 
	}
	return numberIntersections;
}

// start is the startingpoint for the iteration, p1, p2 are the tangents vertices of the polygon
// calculates the osculating cirlce for p2
inline Circle computeOsculatingCircle(vec2_d start, vec2_d p1, vec2_d p2) {
	vec2_d q = length(start-p2) * normalize(start - p1) +  start; // second point where the circle tangents
	vec2_d direction1 = normalize(vec2_d(-(start - p2).y, (start - p2).x)); // rotate directions
	vec2_d direction2 = normalize(vec2_d(-(q - start).y, (q - start).x));
	vec2_d intersection = lineIntersection(p2, direction1, q, direction2);
	vec2_d center = p2 + intersection[0] * direction1;
	float radius = abs(intersection[0]);
	return Circle(center, radius);
}

// calculates the "left" tangentpoint on the circle for a point.
// TODO there is an error here
// formula has error, calculates the wrong tangentpoint
// potenziell die Rotationsmatrix?
inline vec2_d tangentPointCircle(vec2_d pt, Circle circle) {
	// known: pt-center, r as distance to solution, angle at pt between pt-center and pt-solution -> arcsin(r/ |pt-center|)
	// first idea: compute tangent and project center onto it
	double angle = asin(circle.radius/length(circle.center - pt));
	mat2_d rotator = mat2_d((cos(angle)), - sin(angle), sin(angle), cos(angle)); // rotation matrix
	vec2_d direction = rotator * (circle.center - pt);  // TODO bei der Matrixmult ist ein Fehler! Der rechnet hier falsch
	Line l = Line();
	l.set(pt, pt + direction);
	vec2_d tangentPoint = projectOntoLine(circle.center, l);
	return tangentPoint;
}


// for each point of the polygon calculate the tangent to the circle and check, how many intersection this line has with the polygon
// first point in tuple is from polygon, second is on the circle
inline std::tuple<vec2_d, vec2_d> tangentPolyCircle(const int& startVertex, const Poly& polygon, const Circle& circle) {
	int n = polygon.vertexData.size() - 1; // number of vertizes in polygon
	for (int i = startVertex; i < n + startVertex; i++) {
		vec2_d tangentPoint = tangentPointCircle(polygon.vertexData_d[i%n], circle); // get potential tangent point on the circle TODO this can aparently be NAN!!! BUT IT STILL WORKS?!?!? Dont worry, it happens only with the already tangent point. that is ok. should not happen :)
		Line l = Line();
		l.set(tangentPoint, polygon.vertexData_d[i%n]);
		if (numberOfIntersections(polygon, l) == 1) {
			return std::make_tuple(polygon.vertexData_d[i % n], tangentPoint);
		}
	}
	return std::make_tuple(vec2_d(), vec2_d()); // if it reaches here, we have a problem
}

// for each vertex of the polygon, create a line with pos and find out how many intersections with the polygon this line has.
// if there is only 1, it is tangent
// the return value has to be the index of the vertices for firther calculations
inline std::tuple<int, int> computeTangentVertices(const vec2_d& pos, const Poly& polygon) {
	std::vector<int> tangentVertices;
	for (int i = 0; i < polygon.vertexData.size()-1; i++) {
		Line l = Line(); 
		l.set(pos, polygon.vertexData_d[i]);
		int intersections = numberOfIntersections(polygon, l);
		if (intersections == 1) tangentVertices.push_back(i); // hier muss was gemacht werden
	}

	if (tangentVertices.size() < 2) { // if there are not 2 tangent vertizes, return a dummy
		return std::make_tuple(0, 1);
	}

	if (det(polygon.vertexData_d[tangentVertices[0]] - pos, polygon.vertexData_d[tangentVertices[tangentVertices.size() - 1]] - pos) > 0.0f) {
		return std::make_tuple(tangentVertices[tangentVertices.size() - 1], tangentVertices[0]); // if tangentVertices does not have size 2, we have a problem TODO
	}
	return std::make_tuple(tangentVertices[0], tangentVertices[tangentVertices.size() - 1]); // if tangentVertices does not have size 2, we have a problem TODO
}

// find the tangent vertices, by finding the 2 vertices, that give the largest angle
inline std::tuple<int, int> computeTangentVerticesAlt(const vec2_d& pos, const Poly& polygon) {
	int n = polygon.vertexData.size() - 1; // amount of vertices
	float currentLargestAngle = 100;
	std::pair<int, int> currentBestVertices = std::make_pair(0, 1);
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			float angle = dot(polygon.vertexData_d[i] - pos, polygon.vertexData_d[j] - pos)/(length(polygon.vertexData_d[i] - pos) * length(polygon.vertexData_d[j] - pos));
			if (angle < currentLargestAngle) { // Yes, < is correct here, since angle is actually cos(angle) and cos is monoton decreasing
				currentLargestAngle = angle;
				currentBestVertices = std::make_pair(i, j); 
			}
		}
	}
	if (det(polygon.vertexData_d[currentBestVertices.first] - pos, polygon.vertexData_d[currentBestVertices.second] - pos) > 0.0f) {
		return std::make_tuple(currentBestVertices.second, currentBestVertices.first); // if tangentVertices does not have size 2, we have a problem TODO
	}
	return std::make_tuple(currentBestVertices.first, currentBestVertices.second);
	
}


inline vec2_d fourthBilliard(const Poly& polygon, const vec2_d& p) {
	auto tangentVertices = computeTangentVerticesAlt(p, polygon);
	Circle circle = computeOsculatingCircle(p, polygon.vertexData_d[std::get<0>(tangentVertices)], polygon.vertexData_d[std::get<1>(tangentVertices)]);
	auto tangentVerticesCircle = tangentPolyCircle(std::get<1>(tangentVertices) + 1, polygon, circle);	// call with +1, since we dont need to check current point but next point
	vec2_d direction1 = normalize(polygon.vertexData_d[std::get<1>(tangentVertices)] - p); // direction from starting point to new point
	vec2_d direction2 = normalize(std::get<0>(tangentVerticesCircle) - std::get<1>(tangentVerticesCircle));
	return p + lineIntersection(p, direction1, std::get<1>(tangentVerticesCircle), direction2)[0] * direction1;
	//return p + vec2(0.1, 0.1);
}

// vec2 SymplecticBilliardMap(Poly& polygon, vec2 t) {
// 	float t0 = t.x;
// 	float t1 = t.y;
// 	if ((!polygon.closed || abs(t0 - int(t0)) <  EPS) || abs(t1 - int(t1)) <  EPS) {
// 		return t; // Polygon has to be closed
// 	}
// 	vec2 x = polygon.ParamEdgeRatio(double(t0));
// 	vec2 dirX = polygon.directions[int(floorf(t0))];
// 	vec2 dirY = polygon.directions[int(floorf(t1))];
// 	float signf = det(dirX, dirY);
// 	if (abs(signf) <  EPS) return vec2(t0, t1);
// 	vec2 inwardDirY = sgn(signf) * dirY;
// 	if (sgn(signf) == 0) std::cout << "WARNING: the inward-direction became zero" << std::endl;
// 	float t2 = firstIntersection(polygon, x, inwardDirY);
// 	return vec2(t1, t2);
// }

void Rectangle::create(float width, float height)
{
	glGenFramebuffers(1, &textureFBO);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB,  2*width,  2*height, 0,GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, textureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    glDrawBuffer(GL_FRONT_AND_BACK);
    glReadBuffer(GL_FRONT_AND_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);  

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);   

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void Rectangle::draw(Shader &shaderprogram, float width, float height, std::vector<vec2>& vertexData)
{
	    glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderprogram.ID, "phasespace"), 0);
        glUniform1f(glGetUniformLocation(shaderprogram.ID, "width"), 2*width);
        glUniform1f(glGetUniformLocation(shaderprogram.ID, "height"), 2*height);
		glUniform2fv(glGetUniformLocation(shaderprogram.ID, "VERTICES"), 6, &vertexData[0].x);
        
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
}


