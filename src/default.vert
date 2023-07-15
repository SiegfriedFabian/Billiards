#version 330 core
// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Outputs the color for the Fragment Shader
out vec3 color;
// Imports the camera matrix from the main function
uniform float zoom = 1.0;
uniform vec2 cameraPos = vec2(0.0,0.0);
uniform vec3 vertexColor;
uniform float x_scale = 1.0;
void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(zoom*(vec3(x_scale * (aPos.x - cameraPos.x), aPos.y - cameraPos.y, 0.0) ), 1);
	// Assigns the colors from the Vertex Data to "color"
	color = vertexColor;
}