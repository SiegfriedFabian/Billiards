#include "Trajectory.h"
float eps = 10e-5;
Trajectory::Trajectory() {
	polygon = new Poly();
	size = 3.0f;
}

Trajectory::Trajectory(vec2_d start, Poly* p, vec3 c) {
	size = 3.0f;	// set size of Trajectory as 3.0 to make it smaller
	polygon = p;
	color = c;
	AddVertex(start);
	//vertexData_d.push_back(start);
	//vertexData.push_back(start.toFloat());
}

//void Trajectory::Create() {
//	
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);
//
//	glGenBuffers(1, &vbo); // Generate 1 vertex buffer object
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	// Enable the vertex Attribute arrays
//	glEnableVertexAttribArray(0);
//	//glEnableVertexAttribArray(1);
//	// Map attribute array 0 to the vertex data of the interleaved vbo
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0)); // first two floats are position
//	// Map attribute array 1 to the color data of the interleaved vbo
//	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
//}

//void Trajectory::Draw(Shader& shaderProgram) {
//	if (vertexData.size() > 0) {
//		shaderProgram.setUniform(color, "vertexColor");
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * 2 * sizeof(float), &vertexData[0], GL_DYNAMIC_DRAW);
//		glBindVertexArray(vao);
//		//glEnable(GL_POINT_SMOOTH);
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glPointSize(size);
//		glDrawArrays(GL_POINTS, 0, vertexData.size());
//		//glDisable(GL_POINT_SMOOTH);
//	}
//}

void Trajectory::iterate() {
	vec2_d pos = vertexData_d.back();
	auto tangentVertices = computeTangentVertices(pos);
	Circle_d circle = computeOsculatingCircle(pos, vec2_d(polygon->vertexData[tangentVertices.first]), vec2_d(polygon->vertexData[tangentVertices.second]));
	vec2_d thirdTangentDirection = normalize(pos - vec2_d(polygon->vertexData[tangentVertices.second])); // direction of the third tangent from Circle to Poly (the weird one)
	auto tangentVerticesCircle = tangentPolyCircle(tangentVertices.second + 1, circle, thirdTangentDirection);	// call with +1, since we dont need to check current point but next point
	vec2_d direction1 = normalize(vec2_d(polygon->vertexData[tangentVertices.second]) - pos); // direction from starting point to new point
	vec2_d direction2 = normalize(tangentVerticesCircle.first - tangentVerticesCircle.second);
	vec2_d nextPoint =  pos + lineIntersection(pos, direction1, tangentVerticesCircle.second, direction2)[0] * direction1;

	//vertexData_d.push_back(nextPoint);
	//vertexData.push_back(nextPoint.toFloat());
	AddVertex(nextPoint);

	count++;
}

void Trajectory::iterate(int batch, int nIter)
{
	if (count > nIter)
	{
		return;
	}
	for (size_t i = 0; i < batch; i++)
	{
		iterate();
	}
}

//vec2 Trajectory::getStartingPoint()
//{
//	return vertexData[0];
//}

void Trajectory::Reset() {
	vertexData.resize(1);
	vertexData_d.resize(1);
	vertexData_d[0] = vec2_d(vertexData[0]); // manually set vertexData_d, as vertexDat[0] can be changed from main function
	count = 0;
}

// TODO 
// this function does not work currently for some weird reason, it does everything it should but there is no effect.
// I believe this is due to the Bug from InputScalarN
void Trajectory::Reset(vec2_d v) 
{
	this->clear();
	this->AddVertex(v);
	count = 0;
	//std::cout << vertexData_d.size() << std::endl;
	//Reset();
	//vertexData_d[0] = v;
	//vertexData[0] = v.toFloat();
	//std::cout << vertexData_d.size() << std::endl;
	//std::cout << vertexData_d.back().x << std::endl;
}

vec2_d Trajectory::lineIntersection(const vec2_d& p0, const vec2_d& v0, const vec2_d& p1, const vec2_d& v1) {
	vec2_d deltaP = p0 - p1;
	if (abs(det(v0, v1)) < eps) return INFINITY;
	vec2_d t = 1 / (v1.x * v0.y - v0.x * v1.y) * vec2_d(dot(deltaP, vec2_d(v1.y, -v1.x)), dot(deltaP, vec2_d(v0.y, -v0.x)));
	return t;
}

// SHOULD BE DEPRECATED
//int Trajectory::numberOfIntersections(const vec2_d lineDirection, const vec2_d start) {
//	int numberIntersections = 0;
//	//vec2 lineDirection = normalize(line.direction);
//	for (int i = 0; i < polygon->vertexData.size() - 1; i++) {
//		vec2_d directionSide = polygon->vertexData[i + 1] - polygon->vertexData[i];
//		double lengthSide = length(directionSide);
//		directionSide = normalize(directionSide);
//		vec2_d ts = lineIntersection(vec2_d(polygon->vertexData[i]), directionSide, start, lineDirection);
//		//float t = ts.x * lengthSide; // use fomrula 
//		if (abs(ts.x - lengthSide) < eps || abs(ts.x) < eps) {		// if we go through one of the vertices, we are potentially tangent, but only of we never intersect again.	
//			numberIntersections = 1;
//		}
//		else {
//			if (ts.x < lengthSide && ts.x > 0) {
//				return 2;			// if t is between 0 and 1, the line goes through the polygon
//			}
//		}
//	}
//	return numberIntersections;
//}

std::pair<int, int> Trajectory::computeTangentVertices(vec2_d pos) {
	int n = polygon->vertexData.size() - 1; // amount of vertices
	double currentLargestAngle = 100;
	std::pair<int, int> currentBestVertices = std::make_pair(0, 1);
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			double angle = dot(vec2_d(polygon->vertexData[i]) - pos, vec2_d(polygon->vertexData[j]) - pos) / (length(vec2_d(polygon->vertexData[i]) - pos) * length(vec2_d(polygon->vertexData[j]) - pos));
			if (angle < currentLargestAngle) { // Yes, < is correct here, since angle is actually cos(angle) and cos is monoton decreasing
				currentLargestAngle = angle;
				currentBestVertices = std::make_pair(i, j);
			}
		}
	}
	if (det(vec2_d(polygon->vertexData[currentBestVertices.first]) - pos, vec2_d(polygon->vertexData[currentBestVertices.second]) - pos) > 0.0) {
		return std::make_pair(currentBestVertices.second, currentBestVertices.first); // if tangentVertices does not have size 2, we have a problem TODO
	}
	return std::make_pair(currentBestVertices.first, currentBestVertices.second);

}

Circle_d Trajectory::computeOsculatingCircle(vec2_d pos, vec2_d p1, vec2_d p2) {
	vec2_d q = length(pos - p2) * normalize(pos - p1) + pos; // second point where the circle tangents
	vec2_d direction1 = normalize(vec2_d(-(pos - p2).y, (pos - p2).x)); // rotate directions
	vec2_d direction2 = normalize(vec2_d(-(q - pos).y, (q - pos).x));
	vec2_d intersection = lineIntersection(p2, direction1, q, direction2);
	vec2_d center = p2 + intersection[0] * direction1;
	double radius = abs(intersection[0]);
	return Circle_d(center, radius);
}


vec2_d Trajectory::tangentPointCircle(vec2_d pos, Circle_d circle) {
	vec2_d translatedPos = pos - circle.center;
	double d2 = dot(translatedPos, translatedPos); // distance squared
	if (abs(d2 - circle.radius * circle.radius) < eps )
	{
		std::cout << "error" << std::endl;
		throw std::domain_error("pos inside circle");
		//return vec2_d(0, 0); // for debugging. Later write throw exception
	}
	// formula from wikipedia https://en.wikipedia.org/wiki/Tangent_lines_to_circles#With_analytic_geometry
	vec2_d resultTranslated = circle.radius * circle.radius / d2 * translatedPos 
							 - circle.radius / d2 * sqrt(d2 - circle.radius * circle.radius) * vec2_d(-translatedPos.y, translatedPos.x);
	return resultTranslated + circle.center;
}



std::pair<vec2_d, vec2_d> Trajectory::tangentPolyCircle(const int& startVertex, const Circle_d& circle, const vec2_d thirdTangentDirection) {
	int n = polygon->vertexData.size() - 1; // number of vertizes in polygon

	double maxAngle = 100;
	std::pair<vec2_d, vec2_d> tangent = std::make_pair(vec2_d(), vec2_d());
	for (int i = startVertex; i < n + startVertex - 1; i++) { // -1 because we do not want to check the last vertex, since that for some reason slows down performance and the last one is actually on the circle so cant be theo one we look for
		try
		{
			// ok, this will look very cursed for an angle calculation, but draw a picture and it makes sense:
			// we know that the angle between thirdTangentDirection and the new tangent direction must be between 0� and 180� by construction of circle
			// the biggest angle then gives us the second tangent point
			vec2_d tangentPoint = tangentPointCircle(vec2_d(polygon->vertexData[i % n]), circle); 
			double angle = dot(thirdTangentDirection, normalize(vec2_d(polygon->vertexData[i % n]) - tangentPoint));
			if (angle < maxAngle) {
				maxAngle = angle;
				tangent = std::make_pair(vec2_d(polygon->vertexData[i % n]), tangentPoint);
			}
		}
		catch (const std::domain_error& e)
		{
			continue; // in case a point is inside the circle or on the circle, we want to skip it
		}
		
		
		

		/* DEPRECATED
		vec2_d lineDirection = vec2_d(polygon->vertexData[i % n]) - tangentPoint;
		if (numberOfIntersections(lineDirection, tangentPoint) == 1) {
			return std::make_pair(polygon->vertexData[i % n], tangentPoint);
		}
		*/
	}
	return tangent;
}

