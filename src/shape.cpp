#include "shape.h"

float EPS = 10e-5;
const double PI = 3.14159265358979323846;

vec2 mousePosCoord(GLFWwindow *window, float SCR_WIDTH, float SCR_HEIGHT, Camera camera)
{
	double pX, pY;
	glfwGetCursorPos(window, &pX, &pY);
	float cX = (2.0f * pX / SCR_WIDTH - 1.0f)* (SCR_WIDTH/SCR_HEIGHT) / camera.Zoom;	// flip y axis
	float cY = (1.0f - 2.0f * pY / SCR_HEIGHT)/ camera.Zoom ;
	
	return vec2(cX + camera.Position.x, cY + camera.Position.y);
}

vec2 lineIntersection(vec2& p0, vec2& v0, vec2& p1, vec2& v1) {
	vec2 deltaP = p0 - p1;
	if (abs(det(v0, v1)) < EPS) return INFINITY;
	vec2 t = 1 / (v1.x * v0.y - v0.x * v1.y) * vec2(dot(deltaP, vec2(v1.y, -v1.x)), dot(deltaP, vec2(v0.y, -v0.x)));
	return t; // Returns t0 and t1 which both describe the intersecting point p0 + t0*dir0 = p1 + t1*dir1
}

inline float sdSegment(const vec2 p, const vec2 a, const vec2 b)
{
	vec2 pa = p - a, ba = b - a;
	float h = std::clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
	return length(pa - ba * h);
}

inline float sdSegment(const vec2 p, const Line l)
{
	vec2 pa = p - l.vertexData[0], ba = l.vertexData[1] - l.vertexData[0];
	float h = std::clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
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
		for (int i = 0; i < directions.size(); i++)
		{
			directions[i] = vertexData[i + 1] - vertexData[i];
		}
}
void shape::roundData()
 {
		for (auto& vertex : vertexData) {
			vertex = round(vertex);
		}
		updateDirections();
}

int shape::computeClosestIndex(GLFWwindow *window, vec2 mousePos)
{
		int closest_i = 0;
		float minDist = INFINITY;
		for (int i = 0; i < vertexData.size(); i = i + 1)
		{
			float distToVertex = length(vertexData[i] - mousePos);
			if (distToVertex < minDist) {
				minDist = distToVertex;
				closest_i = i;
			}
		}
		//std::cout << "Closest index: " << closest_i << std::endl;
		return closest_i;
}

void shape::translateBy(vec2 &deltaPos)
{
	for(vec2 v : vertexData) v = v + deltaPos;
	for (auto it = begin (vertexData); it != end (vertexData); ++it) {
    	*it = *it + deltaPos;
}
}

IndexDistPair shape::computeClosestIndexDistance(GLFWwindow *window, vec2 mousePos)
{
		int closest_i = 0;
		float minDist = INFINITY;
		for (int i = 0; i < vertexData.size(); i = i + 1)
		{
			float distToVertex = length(vertexData[i] - mousePos);
			if (distToVertex < minDist) {
				minDist = distToVertex;
				closest_i = i;
			}
		}
		//std::cout << "Closest index: " << closest_i << std::endl;
		return {closest_i, minDist};
}


void shape::dragDropTo(GLFWwindow *window, vec2& pos)
{ 
		// Right-Click behaviour
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
			pickedVertex = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && vertexData.size() > 0) {
			if (!pickedVertex) {
				pickedVertexIndex = computeClosestIndex(window, pos);
				pickedVertex = true;
			}

			if (length(vertexData[pickedVertexIndex] - pos) > EPS) {
				vertexData[pickedVertexIndex] = pos;
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

void Poly::AddVertex(vec2 p) {
		vertexData.push_back(p);
		int i = vertexData.size() - 1;
		if (i > 0) directions.push_back(vertexData[i] - vertexData[i - 1]);
	}


vec2 Poly::ParamEdgeRatio(float t){
		float tMod = fmod(t, directions.size());
		int edgeIndex = int(tMod);
		float tlin = t - int(tMod);
		return tlin * vertexData[edgeIndex + 1] + (1.0f - tlin) * vertexData[edgeIndex];
	}

void Poly::ClosePolygon(){
		if (!closed) {
			vertexData.push_back(vertexData[0]);
			// std::cout << "before direction update" << std::endl;

			directions.push_back(vertexData[0] - vertexData[vertexData.size() - 2]);
			closed = true;
		}
	}
void Poly::Clear() {
		vertexData.clear();
		directions.clear();
		closed = false;
	}
void Poly::onMouse(GLFWwindow *window, vec2 &mousePos){ // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
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

			if (length(vertexData[pickedVertexIndex] - mousePos) >  EPS) {
				vertexData[pickedVertexIndex] = mousePos;
				vertexChange = true;
			}

			if (closed && pickedVertexIndex == 0) {
				int lastVert = vertexData.size() - 1;
				vertexData[lastVert] = mousePos;
			}
			updateDirections();
		}
	}

float Poly::distance(vec2 &p){
	float dist = INFINITY;
	for (int i = 0; i < vertexData.size()-1; i++)
	{
		float d = sdSegment(p, vertexData[i], vertexData[i+1]);
		if (d < dist)
		{
			dist = d;
		}
		
	}
	return dist;
}

int Poly::closestEdge(const vec2 &p){
		int minEdge = 0;
		float minDist = INFINITY;
		for (int i = 0; i < directions.size(); i++)
		{
			float currentDist = sdSegment(p, vertexData[i], vertexData[i + 1]);
			if (currentDist < minDist) {
				minDist = currentDist;
				minEdge = i;
			}
		}
		// std::cout << minDist << std::endl;
		return minEdge;
	}

void Poly::makeNStar(int n, float innerRadius, float outerRadius){
    if (n <= 2) return; // if n <= 2 then dont do anything as we cant have a polygon
    this->Clear(); // remove all curent vertices
    for (int i = 0; i < n; i++) {
        this->AddVertex(vec2(outerRadius * std::cos(2 * PI * i / n ), outerRadius * sin(2 * PI * i / n))); // add new vertices)
        this->AddVertex(vec2(innerRadius * std::cos(2 * PI * i / n + PI / n), innerRadius * sin(2 * PI * i / n + PI / n))); // add new vertices)
    }
    this->ClosePolygon(); // close the polygon again
}

void Poly::makeRegularNPoly(int n, float radius){
    if (n <= 2) return; // if n <= 2 then dont do anything as we cant have a polygon
    this->Clear(); // remove all curent vertices
    for (int i = 0; i < n; i++) {
        this->AddVertex(vec2(radius * std::cos(2 * PI * i / n), radius * sin(2 * PI * i / n))); // add new vertices
    }
    this->ClosePolygon(); // close the polygon again
}

void Poly::makeRegularNStar(int n, float radius){
	makeNStar(n, (0.5 * radius)/ cos(PI/(n)), radius);
}

void Poly::turnIntoQuad()
{
	Clear();
	AddVertex(vec2(0.0,0.0));
	AddVertex(vec2(4.0,0.0));
	AddVertex(vec2(1.0,3.0));
	AddVertex(vec2(0.0,2.0));
	ClosePolygon();
}

vec2 Poly::computeCenter()
{
	vec2 avg = vec2(0.,0.);
	if(closed){
		for (int i = 0; i < vertexData.size()-1; i++)
		{
			avg = avg + vertexData[i];
		}
    	return (1.0f/(vertexData.size()-1)) * avg;

	} else {
		for (int i = 0; i < vertexData.size(); i++)
		{
			avg = avg + vertexData[i];
		}
    	return (1.0f/vertexData.size()) * avg;
	}
}

void Poly::center()
{
	vec2 avg = computeCenter();
	for (auto it = begin (vertexData); it != end (vertexData); ++it) {
    	*it = *it - avg;
	}
}

Line::Line() {
    vertexData.push_back(vec2());
    vertexData.push_back(vec2());
    updateDirections();
}

void Line::set(vec2 start, vec2 end) {
    vertexData[0] = start;
    vertexData[1] = end;
    updateDirections();
}

void Line::setStart(vec2 start) {
    vertexData[0] = start;
    updateDirections();
}

void Line::setEnd(vec2 end){
    vertexData[1] = end;
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
            float x_val = vertexData[i].x;
            vertexData[i].x = floor(x_val+0.5);
            float y_val = vertexData[i].y;
            vertexData[i].y = floor(y_val+0.5);
        }
        updateDirections();
    }
}

void Line::updateDirections(){
		if(vertexData.size() > 1) direction = vertexData[1] - vertexData[0];
	}

void Line::setClosestVertexTo(GLFWwindow *window, vec2 &mouse) {
		vertexData[computeClosestIndex(window, mouse)] = mouse;
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

vec2 Points::getPos()
{
	return vertexData[0];
}

void Points::clear() {
		vertexData.clear();
	}

void Points::projectOntoPolygon(Poly &polygon){
		for (auto & pos : vertexData)
		{
			int edge = polygon.closestEdge(pos);
			float l = length(polygon.directions[edge]);
			vec2 d = normalize(polygon.directions[edge]);
			pos = std::clamp(dot(pos - polygon.vertexData[edge], d), 0.0f, l) * d + polygon.vertexData[edge];
		}
}

inline vec2 lineIntersection(const vec2& p0, const vec2& v0, const vec2& p1, const vec2& v1) {
	// Note: v0, v1 have to be normalized
	vec2 deltaP = p0 - p1;
	if (abs(det(v0, v1)) < EPS) return INFINITY;
	vec2 t = 1 / (v1.x * v0.y - v0.x * v1.y) * vec2(dot(deltaP, vec2(v1.y, -v1.x)), dot(deltaP, vec2(v0.y, -v0.x)));
	return t; // Returns t0 and t1 which both describe the intersecting point p0 + t0*dir0 = p1 + t1*dir1
}

float firstIntersection(Poly& polygon, vec2 p, vec2 dir) {
	float minDist = INFINITY;
	int minIndex = INFINITY;
	float res = 0;
	for (int i = 0; i < polygon.directions.size(); i++)
	{
		vec2 t = lineIntersection(p, dir, polygon.vertexData[i], polygon.directions[i]);
		if (0.0001 < t.x && t.x < minDist &&  EPS < t.y && t.y < 0.999) {
			//std::cout << "At direction " << i << " = (" << directions[i].x << ", " << directions[i].y << "), we have t = " << t.x << ", " << t.y << ", " << length(directions[i]) << std::endl;
			minDist = t.x;
			minIndex = i;
			res = float(i) + t.y;
		}
	}
	return res; // Returns the distance from p, i.e. the intersection is at p + minDist*dir
}

vec2 projectOntoLineClamped(const vec2 &pos, const Line &line)
{
   	float l = length(line.direction);
	vec2 d = normalize(line.direction);
	return std::clamp(dot(pos - line.vertexData[0], d), 0.0f, l) * d + line.vertexData[0];
}

vec2 projectOntoLine(const vec2 &pos, const Line &line)
{
	vec2 d = normalize(line.direction);
	return dot(pos - line.vertexData[0], d) * d + line.vertexData[0];
}

// inline vec2 projectOntoLine(const vec2& pos, const Line& line) {
// 	float l = length(line.direction);
// 	vec2 d = normalize(line.direction);
// 	return std::clamp(dot(pos - line.vertexData[0], d), 0.0f, l) * d + line.vertexData[0];
// }

vec2 projectOntoPolygon(const vec2& pos, Poly& polygon) {
	int edge = polygon.closestEdge(pos);
	float l = length(polygon.directions[edge]);
	vec2 d = normalize(polygon.directions[edge]);
	return std::clamp(dot(pos - polygon.vertexData[edge], d), 0.0f, l) * d + polygon.vertexData[edge];
}

float inverseParamPolygon(const vec2 &pos, Poly &polygon)
{
    int edge = polygon.closestEdge(pos);
	float l = length(polygon.directions[edge]);
	vec2 d = normalize(polygon.directions[edge]);
	return std::clamp(dot(pos - polygon.vertexData[edge], d), 0.0f, l) / l + float(edge);
}

inline float lineIntersectsSide(const vec2& x, const vec2& y, const Line& line) {
	vec2 startingPiont = line.vertexData[0];
	float t = ((startingPiont.y - x.y) * line.direction.x + (startingPiont.x - x.x) * line.direction.y) / (line.direction.x * (y.y - x.y) + (y.x - x.x) * line.direction.y);
	return t;
}

// goes through the edges of the polygon and calculates wether the line hits or not
// stops if the line goes through one of the edges (this only works because since the polygon is convex there can only be two intersections at most)
inline int numberOfIntersections(const Poly& polygon, const Line& line) {
	int numberIntersections = 0;
	vec2 lineDirection = normalize(line.direction);
	for (int i = 0; i < polygon.vertexData.size()-1; i++) {
		vec2 directionSide = polygon.vertexData[i+1] - polygon.vertexData[i];
		float lengthSide = length(directionSide);
		directionSide = normalize(directionSide);
		vec2 ts = lineIntersection(polygon.vertexData[i], directionSide, line.vertexData[0], lineDirection);
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
inline Circle computeOsculatingCircle(vec2 start, vec2 p1, vec2 p2) {
	vec2 q = length(start-p2) * normalize(start - p1) +  start; // second point where the circle tangents
	vec2 direction1 = normalize(vec2(-(start - p2).y, (start - p2).x)); // rotate directions
	vec2 direction2 = normalize(vec2(-(q - start).y, (q - start).x));
	vec2 intersection = lineIntersection(p2, direction1, q, direction2);
	vec2 center = p2 + intersection[0] * direction1;
	float radius = abs(intersection[0]);
	return Circle(center, radius);
}

// calculates the "left" tangentpoint on the circle for a point.
// TODO there is an error here
// formula has error, calculates the wrong tangentpoint
// potenziell die Rotationsmatrix?
inline vec2 tangentPointCircle(vec2 pt, Circle circle) {
	// known: pt-center, r as distance to solution, angle at pt between pt-center and pt-solution -> arcsin(r/ |pt-center|)
	// first idea: compute tangent and project center onto it
	float angle = float(asin(circle.radius/length(circle.center - pt)));
	mat2 rotator = mat2(float(cos(angle)), -float(sin(angle)), float(sin(angle)), float(cos(angle))); // rotation matrix
	vec2 direction = rotator * (circle.center - pt);  // TODO bei der Matrixmult ist ein Fehler! Der rechnet hier falsch
	Line l = Line();
	l.set(pt, pt + direction);
	vec2 tangentPoint = projectOntoLine(circle.center, l);
	return tangentPoint;
}


// for each point of the polygon calculate the tangent to the circle and check, how many intersection this line has with the polygon
// first point in tuple is from polygon, second is on the circle
inline std::tuple<vec2, vec2> tangentPolyCircle(const int& startVertex, const Poly& polygon, const Circle& circle) {
	int n = polygon.vertexData.size() - 1; // number of vertizes in polygon
	for (int i = startVertex; i < n + startVertex; i++) {
		vec2 tangentPoint = tangentPointCircle(polygon.vertexData[i%n], circle); // get potential tangent point on the circle TODO this can aparently be NAN!!! BUT IT STILL WORKS?!?!? Dont worry, it happens only with the already tangent point. that is ok. should not happen :)
		Line l = Line();
		l.set(tangentPoint, polygon.vertexData[i%n]);
		if (numberOfIntersections(polygon, l) == 1) {
			return std::make_tuple(polygon.vertexData[i % n], tangentPoint);
		}
	}
	return std::make_tuple(vec2(), vec2()); // if it reaches here, we have a problem
}

// for each vertex of the polygon, create a line with pos and find out how many intersections with the polygon this line has.
// if there is only 1, it is tangent
// the return value has to be the index of the vertices for firther calculations
inline std::tuple<int, int> computeTangentVertices(const vec2& pos, const Poly& polygon) {
	std::vector<int> tangentVertices;
	for (int i = 0; i < polygon.vertexData.size()-1; i++) {
		Line l = Line(); 
		l.set(pos, polygon.vertexData[i]);
		int intersections = numberOfIntersections(polygon, l);
		if (intersections == 1) tangentVertices.push_back(i); // hier muss was gemacht werden
	}

	if (tangentVertices.size() < 2) { // if there are not 2 tangent vertizes, return a dummy
		return std::make_tuple(0, 1);
	}

	if (det(polygon.vertexData[tangentVertices[0]] - pos, polygon.vertexData[tangentVertices[tangentVertices.size() - 1]] - pos) > 0.0f) {
		return std::make_tuple(tangentVertices[tangentVertices.size() - 1], tangentVertices[0]); // if tangentVertices does not have size 2, we have a problem TODO
	}
	return std::make_tuple(tangentVertices[0], tangentVertices[tangentVertices.size() - 1]); // if tangentVertices does not have size 2, we have a problem TODO
}

// find the tangent vertices, by finding the 2 vertices, that give the largest angle
inline std::tuple<int, int> computeTangentVerticesAlt(const vec2& pos, const Poly& polygon) {
	int n = polygon.vertexData.size() - 1; // amount of vertices
	float currentLargestAngle = 100;
	std::pair<int, int> currentBestVertices = std::make_pair(0, 1);
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			float angle = dot(polygon.vertexData[i] - pos, polygon.vertexData[j] - pos)/(length(polygon.vertexData[i] - pos) * length(polygon.vertexData[j] - pos));
			if (angle < currentLargestAngle) { // Yes, < is correct here, since angle is actually cos(angle) and cos is monoton decreasing
				currentLargestAngle = angle;
				currentBestVertices = std::make_pair(i, j); 
			}
		}
	}
	if (det(polygon.vertexData[currentBestVertices.first] - pos, polygon.vertexData[currentBestVertices.second] - pos) > 0.0f) {
		return std::make_tuple(currentBestVertices.second, currentBestVertices.first); // if tangentVertices does not have size 2, we have a problem TODO
	}
	return std::make_tuple(currentBestVertices.first, currentBestVertices.second);
	
}


inline vec2 fourthBilliard(const Poly& polygon, const vec2& p) {
	auto tangentVertices = computeTangentVerticesAlt(p, polygon);
	Circle circle = computeOsculatingCircle(p, polygon.vertexData[std::get<0>(tangentVertices)], polygon.vertexData[std::get<1>(tangentVertices)]);
	auto tangentVerticesCircle = tangentPolyCircle(std::get<1>(tangentVertices) + 1, polygon, circle);	// call with +1, since we dont need to check current point but next point
	vec2 direction1 = normalize(polygon.vertexData[std::get<1>(tangentVertices)] - p); // direction from starting point to new point
	vec2 direction2 = normalize(std::get<0>(tangentVerticesCircle) - std::get<1>(tangentVerticesCircle));
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


