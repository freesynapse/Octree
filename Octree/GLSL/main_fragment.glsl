#version 430 core

out vec4 fragColor;

uniform vec3 u_vRenderColor;


void main()
{
	fragColor = vec4(u_vRenderColor, 1.0);

}

