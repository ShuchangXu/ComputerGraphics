#version 410 core

layout( vertices = 16 ) out;

in vec3 FragPos[];
out vec3 fragPos[];

uniform float uOuter02, uOuter13, uInner0, uInner1;

void main(){
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    fragPos[gl_InvocationID] = FragPos[gl_InvocationID];
	// set tessellation levels
	gl_TessLevelOuter[0] = uOuter02;
    gl_TessLevelOuter[1] = uOuter13;
    gl_TessLevelOuter[2] = uOuter02;
    gl_TessLevelOuter[3] = uOuter13;
    gl_TessLevelInner[0] = uInner0;
    gl_TessLevelInner[1] = uInner1;
}
