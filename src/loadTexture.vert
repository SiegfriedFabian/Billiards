#version 410 core
// Positions/Coordinates
layout (location = 0) in vec3 Pos;

uniform float zoom = 1.0;
uniform vec2 cameraPos = vec2(0.0,0.0);
uniform float x_scale = 1.0;
void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(zoom*(vec3(x_scale * (Pos.x - cameraPos.x), Pos.y - cameraPos.y, 0.0) ), 1);
}