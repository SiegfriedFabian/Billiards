#version 410 core
// Positions/Coordinates
layout (location = 0) in vec3 Pos;

// Imports the camera matrix from the main function

void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(Pos.x, Pos.z, 0.0, 1.0);//vec4(zoom*(vec3(x_scale * (aPos.x - cameraPos.x), aPos.y - cameraPos.y, 0.0) ), 1);
}


