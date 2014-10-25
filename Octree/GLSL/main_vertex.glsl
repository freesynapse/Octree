#version 430 core

in vec2 a_vPositions;


uniform mat4 u_mModel;
uniform mat4 u_mView;
uniform mat4 u_mProjection;


void main()
{
	mat4 mMVP = u_mProjection * u_mView * u_mModel;
	
	vec4 vPos = vec4(a_vPositions.xy, 0.0, 1.0);
	gl_Position = mMVP * vPos;

}

