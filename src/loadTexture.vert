#version 410 core
// Positions/Coordinates
layout (location = 0) in vec3 Pos;

uniform vec2 cameraPos = vec2(0.0,0.0);
void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4((vec3( (Pos.x - cameraPos.x), Pos.y - cameraPos.y, 0.0) ), 1);
}