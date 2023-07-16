#version 410 core
// Outputs colors in RGBA
out vec4 FragColor;
const int ITERATIONS = 1000;

// uniform vec2 VERTICESZERO[100];
// uniform vec2 DIRECTIONSZERO[100];
// uniform int nVerticesZero;

// uniform vec2 VERTICESONE[100];
// uniform vec2 DIRECTIONSONE[100];
// uniform int nVerticesZero;

const int width = 500;
const int height = 500;

// Quad
const int nVerticesZero = 4;
const vec2[5] VERTICESZERO = vec2[](
    vec2( 0.0, 0.0),
    vec2( 3.0, 0.0),
    vec2( 1.0, 2.0),
    vec2( 0.0, 1.0),
    vec2( 0.0, 0.0)
);

// Quad
const int nVerticesOne = 4;
const vec2[5] VERTICESONE = vec2[](
    vec2( 0.0, 0.0),
    vec2( 3.0, 0.0),
    vec2( 1.0, 2.0),
    vec2( 0.0, 1.0),
    vec2( 0.0, 0.0)
);

int IterateSymplecticBilliards(vec2 pt, int n_iter){
    return 1;
}

float testFunc(vec2 pt){
    return pt.x*pt.y;
}

void main()
{  
    vec2 pos = vec2(gl_FragCoord.x/width, gl_FragCoord.y/height);
    int period = IterateSymplecticBilliards(pos, ITERATIONS);
    float scale = testFunc(pos);
	FragColor = scale*vec4(0.0,0.5,1.0, 1.0);
}