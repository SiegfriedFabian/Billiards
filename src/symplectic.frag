
uniform vec2 VERTICESZERO[100];
uniform vec2 VERTICESONE[100];

uniform float width;
uniform float height;
uniform int ITERATIONS;

// // Quad1
// const vec2[nVerticesZero + 1] VERTICESZERO = vec2[](
//     vec2( 0.0, 0.0),
//     vec2( 3.0, 0.0),
//     vec2( 1.0, 2.0),
//     vec2( 0.0, 1.0),
//     vec2( 0.0, 0.0)
// );

// // Quad2
// const vec2[nVerticesOne+1] VERTICESONE = vec2[](
//     vec2( 0.0, 0.0),
//     vec2( 3.0, 0.0),
//     vec2( 1.0, 2.0),
//     vec2( 0.0, 1.0),
//     vec2( 0.0, 0.0)
// );




const float EPS = 10e-6;
const float INFINITY = 10e9;
// uniform vec2 VERTICESZERO[100];
// uniform vec2 DIRECTIONSZERO[100];
// uniform int nVerticesZero;

// uniform vec2 VERTICESONE[100];
// uniform vec2 DIRECTIONSONE[100];
// uniform int nVerticesZero;




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
// Triangle
const int nVerticesZero = 3;
const vec2[nVerticesZero + 1] VERTICESZERO = vec2[](
    vec2( 1.0, 0.0),
    vec2( 0.0, 1.0),
    vec2( 0.0, -1.0),
    vec2( 1.0, 0.0)
);

// Square
// Quad2
const int nVerticesOne = 4;
const vec2[nVerticesOne+1] VERTICESONE = vec2[](
    vec2( 1.0, 0.0),
    vec2( 0.0, 1.0),
    vec2( -1.0, 0.0),
    vec2( 0.0, -1.0),
    vec2( 1.0, 0.0)
);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
/*
// HEXAGON1
const int nVerticesZero = 6;
const vec2[nVerticesZero + 1] VERTICESZERO = vec2[](
    vec2( 1.0, 0.0),
    vec2( 0.5, 0.866),
    vec2(-0.5, 0.866),
    vec2(-1.0, 0.0),
    vec2(-0.5, -0.866),
    vec2( 0.5, -0.866),
    vec2( 1.0, 0.0)
);

// Heaxagon2
const int nVerticesOne = 6;
const vec2[nVerticesOne+1] VERTICESONE = vec2[](
    vec2( 1.0, 0.0),
    vec2( 0.5, 0.866),
    vec2(-0.5, 0.866),
    vec2(-1.0, 0.0),
    vec2(-0.5, -0.866),
    vec2( 0.5, -0.866),
    vec2( 1.0, 0.0)
);
*/

/*
// HEXAGON1
const int nVerticesZero = 4;
const vec2[nVerticesZero + 1] VERTICESZERO = vec2[](
    vec2( 0.0, 0.0),
    vec2( 8.0, 0.0),
    vec2( 6.5, 2.0),
    vec2( 0.0, 2.0),
    vec2( 0.0, 0.0)
);

// Heaxagon2
const int nVerticesOne = 4;
const vec2[nVerticesOne+1] VERTICESONE = vec2[](
    vec2( 0.0, 0.0),
    vec2( 8.0, 0.0),
    vec2( 6.5, 2.0),
    vec2( 0.0, 2.0),
    vec2( 0.0, 0.0)
);
*/

float det(vec2 v, vec2 w){
    return (v.x * w.y) - (v.y * w.x);
}

vec2 lineIntersection(vec2 p0, vec2 v0, vec2 p1, vec2 v1) {
	vec2 deltaP = p0 - p1;
	if (abs(det(normalize(v0), normalize(v1))) < EPS) return vec2(INFINITY);
	vec2 t = 1.0 / (v1.x * v0.y - v0.x * v1.y) * vec2(dot(deltaP, vec2(v1.y, -v1.x)), dot(deltaP, vec2(v0.y, -v0.x)));
	return t; // Returns t0 and t1 which both describe the intersecting point p0 + t0*dir0 = p1 + t1*dir1
}


float firstIntersectionZero(vec2 p, vec2 dir) {
	float minDist = INFINITY;
	int minIndex = int(INFINITY);
	float res = 0.0;
	for (int i = 0; i < nVerticesZero; i++)
	{
		vec2 t = lineIntersection(p, dir, VERTICESZERO[i], VERTICESZERO[i+1] - VERTICESZERO[i]);
		if (EPS < t.x && t.x < minDist &&  EPS < t.y && t.y < 1.0 - EPS) {
			//std::cout << "At direction " << i << " = (" << directions[i].x << ", " << directions[i].y << "), we have t = " << t.x << ", " << t.y << ", " << length(directions[i]) << std::endl;
			minDist = t.x;
			minIndex = i;
			res = float(i) + t.y;
		}
	}
	return res; // Returns the distance from p, i.e. the intersection is at p + minDist*dir
}

float firstIntersectionOne(vec2 p, vec2 dir) {
	float minDist = INFINITY;
	int minIndex =  int(INFINITY);
	float res = 0.0;
	for (int i = 0; i < nVerticesOne; i++)
	{
		vec2 t = lineIntersection(p, dir, VERTICESONE[i], VERTICESONE[i+1] - VERTICESONE[i]);
		if (EPS < t.x && t.x < minDist &&  EPS < t.y && t.y < 1.0 - EPS) {
			//std::cout << "At direction " << i << " = (" << directions[i].x << ", " << directions[i].y << "), we have t = " << t.x << ", " << t.y << ", " << length(directions[i]) << std::endl;
			minDist = t.x;
			minIndex = i;
			res = float(i) + t.y;
		}
	}
	return res; // Returns the distance from p, i.e. the intersection is at p + minDist*dir
}
 
vec2 direction_on_zero(float t){
    // Returns the direction of the edge on which t lies on VERTICESZERO. We interpret t by 
    //t = mod(t,float(nVerticesZero)); 
    int index = int(t);
    return VERTICESZERO[index+1] - VERTICESZERO[index]; //We should definetly precompute the (normalized?) directions beforehand...
}
vec2 direction_on_one(float t){
    // Returns the direction of the edge on which t lies on VERTICESZERO. We interpret t by 
    //t = mod(t,float(nVerticesOne)); 
    int index = int(t);
    return VERTICESONE[index+1] - VERTICESONE[index]; //We should definetly precompute the (normalized?) directions beforehand...
}

vec2 PosOnZeroAt(float t){
    //t = mod(t,float(nVerticesZero)); 
    int i = int(t);
    return VERTICESZERO[i] + (t - floor(t))*(VERTICESZERO[i+1] - VERTICESZERO[i]);
}

vec2 PosOnOneAt(float t){
    //t = mod(t,float(nVerticesOne)); 
    int i = int(t);
    return VERTICESONE[i] + (t - floor(t))*(VERTICESONE[i+1] - VERTICESONE[i]);
}

int IterateSymplecticBilliards(vec2 t, int n_iter){
    // int counter = 0;
    vec2 nextPos = t;
    for(int counter = 0; counter < n_iter; counter += 2){
        
        // First iteration
        vec2 dirX = VERTICESZERO[int(floor(nextPos.x))+1] - VERTICESZERO[int(floor(nextPos.x))];
        vec2 dirY = VERTICESONE[int(floor(nextPos.y)) + 1] - VERTICESONE[int(floor(nextPos.y))];
        float signf = det(dirX, dirY);
        if (abs(signf) <  10e-5) return int(INFINITY);
        vec2 inwardDirY = sign(signf) * dirY;
        if (sign(signf) == 0.0) return  int(INFINITY);
        float z = firstIntersectionZero(PosOnZeroAt(nextPos.x), inwardDirY);
        nextPos.x = nextPos.y;
        nextPos.y = z;
        
        // Second iteration
        dirX = VERTICESONE[int(floor(nextPos.x))+1] - VERTICESONE[int(floor(nextPos.x))];
        dirY = VERTICESZERO[int(floor(nextPos.y)) + 1] - VERTICESZERO[int(floor(nextPos.y))];
        signf = det(dirX, dirY);
        if (abs(signf) <  10e-5) return  int(INFINITY);
        inwardDirY = sign(signf) * dirY;
        if (sign(signf) == 0.0) return  int(INFINITY);
        z = firstIntersectionOne(PosOnOneAt(nextPos.x),inwardDirY);
        nextPos.x = nextPos.y;
        nextPos.y = z;
        
        // If periodic quit
        if(length(t - nextPos) < 0.0001){
           return counter+2;
       }
    }
    return n_iter;
}

vec2 pixelToWorld(in vec2 px) {
    return 2.0 * (px/vec2(width)
        - vec2(0.5, 0.5 * height / width));
}

void main()
{
    vec2 pos = vec2((gl_FragCoord.x/width) * float(nVerticesZero), (gl_FragCoord.y/height) * float(nVerticesOne));
    int period = IterateSymplecticBilliards(vec2(pos.x, pos.y), ITERATIONS);
    float scale = float(period)/float(ITERATIONS);
	if(period < ITERATIONS)
    {
        FragColor = scale*vec4(0.0,0.5,1.0, 1.0);
    }  else{
        FragColor = vec4(0.01,0.0,0.0, 1.0);    
    } 
}