#version 430 core

in vec2 texpos;

out vec4 out_color;

uniform sampler2D tex;
uniform vec4 color;

void main(){
	out_color = vec4(1, 1, 1, texture2D(tex, texpos).r) * color;	

}

