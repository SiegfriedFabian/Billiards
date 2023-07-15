#version 410 core
// Outputs colors in RGBA
out vec4 FragColor;

uniform sampler2D phasespace;
uniform float width;
uniform float height;


void main()
{   
    vec2 pos = vec2(gl_FragCoord.x/width, gl_FragCoord.y/height);
	FragColor = vec4(texture(phasespace, pos).rgb, 1.0);
}

