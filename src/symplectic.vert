#version 410 core
// Positions/Coordinates
layout (location = 0) in vec3 Pos;

void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4((vec3(Pos.x, Pos.y, 0.0) ), 1);
}