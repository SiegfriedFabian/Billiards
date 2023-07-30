#version 410 core
// Outputs colors in RGBA
out vec4 FragColor;
const int ITERATIONS = 1000;
const float EPS = 10e-5;
const float INFINITY = 10e10;
// uniform vec2 VERTICESZERO[100];
// uniform vec2 DIRECTIONSZERO[100];
// uniform int nVerticesZero;

// uniform vec2 VERTICESONE[100];
// uniform vec2 DIRECTIONSONE[100];
// uniform int nVerticesZero;

const int width = 500;
const int height = 500;
const int ITERATIONS = 100;
const float pos_infinity = 10e9;


// Quad1
const int nVerticesZero = 4;
const vec2[5] VERTICESZERO = vec2[](
    vec2( 0.0, 0.0),
    vec2( 3.0, 0.0),
    vec2( 1.0, 2.0),
    vec2( 0.0, 1.0),
    vec2( 0.0, 0.0)
);

// Quad2
const int nVerticesOne = 4;
const vec2[5] VERTICESONE = vec2[](
    vec2( 0.0, 0.0),
    vec2( 3.0, 0.0),
    vec2( 1.0, 2.0),
    vec2( 0.0, 1.0),
    vec2( 0.0, 0.0)
);

struct Ray {
    vec2 source;
    vec2 direction;
};

struct Line {
    float a;
    float b;
    float c;
};

Line lineFromRay(in Ray ray) {
    return Line(ray.direction.y, 
        -ray.direction.x, 
        -(ray.direction.y * ray.source.x -ray.direction.x * ray.source.y));
}

Line lineFromTwoPoints(in vec2 p1, in vec2 p2) {
    return lineFromRay(Ray(p1, p2 - p1));
}

vec2 intersectLines(in Line l1, in Line l2) {
    float d = l1.a * l2.b - l1.b * l2.a;
    return vec2(
        -l2.b * l1.c + l1.b * l2.c,
        l2.a * l1.c - l1.a * l2.c) / d;
}

vec2 intersectRays(in Ray r1, in Ray r2) {
    return intersectLines(lineFromRay(r1), lineFromRay(r2));
}

// vec2 SymplecticBilliards(vec2 pt){
//     int edge_x = int(pt.x);
//     vec2 x = VERTICESZERO[edge_x] + (pt.x - floor(pt.x))*(VERTICESZERO[edge_x + 1] - VERTICESZERO[edge_x]);
    
//     return vec2(0.0); 
// }

// vec2 getLineIntersection(vec2 p0, vec2 direction, vec2 p2, vec2 p3) 
// {
//     vec2 P = p2;
//     vec2 R = p3 - p2;  
//     vec2 Q = p0;
//     vec2 S = direction;

//     vec2 N = vec2(S.y, -S.x);
//     float t = dot(Q-P, N) / dot(R, N);

//     if (t > 0.0 && t < 1.0)
//         return P + R * t;

//     return vec2(-1.0);
// }   

vec2 lineIntersection(vec2 p0, vec2 v0, vec2 p1, vec2 v1) {
	// Note: v0, v1 have to be normalized
	vec2 deltaP = p0 - p1;
	if (abs(det(v0, v1)) < EPS) return vec2(INFINITY);
	vec2 t = 1 / (v1.x * v0.y - v0.x * v1.y) * vec2_d(dot(deltaP, vec2_d(v1.y, -v1.x)), dot(deltaP, vec2_d(v0.y, -v0.x)));
	return t; // Returns t0 and t1 which both describe the intersecting point p0 + t0*dir0 = p1 + t1*dir1
}



//////////////////////////////////////////////////////////////////////////////////// TODO ///////////////////////////////////////////////////////////////
// double firstIntersection(Poly& polygon, vec2_d p, vec2_d dir) {
// 	double minDist = INFINITY;
// 	int minIndex = INFINITY;
// 	double res = 0;
// 	for (int i = 0; i < polygon.directions_d.size(); i++)
// 	{
// 		vec2_d t = lineIntersection(p, dir, polygon.vertexData_d[i], polygon.directions_d[i]);
// 		if (0.0001 < t.x && t.x < minDist &&  EPS < t.y && t.y < 0.999) {
// 			//std::cout << "At direction " << i << " = (" << directions[i].x << ", " << directions[i].y << "), we have t = " << t.x << ", " << t.y << ", " << length(directions[i]) << std::endl;
// 			minDist = t.x;
// 			minIndex = i;
// 			res = double(i) + t.y;
// 		}
// 	}
// 	return res; // Returns the distance from p, i.e. the intersection is at p + minDist*dir
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


float getLineIntersectionTime(vec2 p0, vec2 direction, vec2 p2, vec2 p3) 
{
    vec2 P = p2;
    vec2 R = p3 - p2;  
    vec2 Q = p0;
    vec2 S = direction;

    vec2 N = vec2(S.y, -S.x);
    float t = dot(Q-P, N) / dot(R, N);

    if (t > 0.0 && t < 1.0)
        return t;

    return 10000.0;
}   

float first_intersectionZero(float p, vec2 direction)
{
    // This computes the closest intersection point with polygon VERTICESONE given a point (via t) on it and a direction
    float dist = 10e5;
    float current = 0.0;
    // make the direction point inwards
    for(int i=0;i<nVerticesZero;++i)
    {
        current = getLineIntersectionTime(p, direction, VERTICESZERO[i], VERTICESZERO[i+1])
        if (current < dist){
            dist = current;
        }
    }
}

int direction_on_zero(float t)
{
    // Returns the direction of the edge on which t lies on VERTICESZERO. We interpret t by 
    t = mod(t,float(nVerticesZero)); 
    int index = floor(t);
    return VERTICESZERO[index+1] - VERTICESZERO[index]; //We should definetly precompute the (normalized?) directions beforehand...
}

int IterateSymplecticBilliards(vec2 pt, int n_iter){
    int counter = 0;
    vec2 nextPos = pt;
    while(length(pt - nextPos) < 0.01 and counter < n_iter){
       counter += 1;
       float z = first_intersectionOne(pt.x, direction_on_first(nextPos.y);
       nextPos.x = nextPos.y

    }
    return counter;
}

float testFunc(vec2 pt){
    return pt.x*pt.y;
}

vec2 pixelToWorld(in vec2 px) {
    return 2.0 * (px/vec2(iResolution.x)
        - vec2(0.5, 0.5 * iResolution.y / iResolution.x));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 pos = vec2(fragCoord.x/iResolution.x, (fragCoord.y)/iResolution.y);
    int period = IterateSymplecticBilliards(pos, ITERATIONS);
    float scale = float(IterateSymplecticBilliards(pos,ITERATIONS));
	fragColor = scale*vec4(0.0,0.5,1.0, 1.0);
}