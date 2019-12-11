#version 410 core

layout(quads, equal_spacing, ccw) in;
in vec3 fragPos[];

uniform mat4 model;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main() {
	vec4 p00 = gl_in[ 0].gl_Position;
    vec4 p10 = gl_in[ 1].gl_Position;
    vec4 p20 = gl_in[ 2].gl_Position;
    vec4 p30 = gl_in[ 3].gl_Position;
    vec4 p01 = gl_in[ 4].gl_Position;
    vec4 p11 = gl_in[ 5].gl_Position;
    vec4 p21 = gl_in[ 6].gl_Position;
    vec4 p31 = gl_in[ 7].gl_Position;
    vec4 p02 = gl_in[ 8].gl_Position;
    vec4 p12 = gl_in[ 9].gl_Position;
    vec4 p22 = gl_in[10].gl_Position;
    vec4 p32 = gl_in[11].gl_Position;
    vec4 p03 = gl_in[12].gl_Position;
    vec4 p13 = gl_in[13].gl_Position;
    vec4 p23 = gl_in[14].gl_Position;
    vec4 p33 = gl_in[15].gl_Position;
    
    vec3 fp00 = fragPos[ 0];
    vec3 fp10 = fragPos[ 1];
    vec3 fp20 = fragPos[ 2];
    vec3 fp30 = fragPos[ 3];
    vec3 fp01 = fragPos[ 4];
    vec3 fp11 = fragPos[ 5];
    vec3 fp21 = fragPos[ 6];
    vec3 fp31 = fragPos[ 7];
    vec3 fp02 = fragPos[ 8];
    vec3 fp12 = fragPos[ 9];
    vec3 fp22 = fragPos[10];
    vec3 fp32 = fragPos[11];
    vec3 fp03 = fragPos[12];
    vec3 fp13 = fragPos[13];
    vec3 fp23 = fragPos[14];
    vec3 fp33 = fragPos[15];
    
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    TexCoord = vec2(u, v);
    
    // Computing the Position, given a u and v
	// the basis functions:
    float bu0 = (1.-u) * (1.-u) * (1.-u);
    float bu1 = 3. * u * (1.-u) * (1.-u);
    float bu2 = 3. * u * u * (1.-u);
    float bu3 = u * u * u;
    
    float bv0 = (1.-v) * (1.-v) * (1.-v);
    float bv1 = 3. * v * (1.-v) * (1.-v);
    float bv2 = 3. * v * v * (1.-v);
    float bv3 = v * v * v;
    
    float dbu0 = - 3. * (1.-u) * (1.-u);
    float dbu1 = 3. * (1.-3*u) * (1.-u);
    float dbu2 = 3 * u * (2.-3*u);
    float dbu3 = 3. * u * u;
    
    float dbv0 = - 3. * (1.-v) * (1.-v);
    float dbv1 = 3. * (1.-3*v) * (1.-v);
    float dbv2 = 3 * v * (2.-3*v);
    float dbv3 = 3. * v * v;
    
    // finally, we get to compute something:
    gl_Position = bu0 * ( bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03 ) + bu1 * ( bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13 ) + bu2 * ( bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23 ) + bu3 * ( bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33 );
    
    // Normal should be computed in world space!!!!!!!!!!!!!!
    vec3 dP_u = dbu0 * ( bv0*fp00 + bv1*fp01 + bv2*fp02 + bv3*fp03 ) + dbu1 * ( bv0*fp10 + bv1*fp11 + bv2*fp12 + bv3*fp13 ) + dbu2 * ( bv0*fp20 + bv1*fp21 + bv2*fp22 + bv3*fp23 ) + dbu3 * ( bv0*fp30 + bv1*fp31 + bv2*fp32 + bv3*fp33 );
    vec3 dP_v = bu0 * ( dbv0*fp00 + dbv1*fp01 + dbv2*fp02 + dbv3*fp03 ) + bu1 * ( dbv0*fp10 + dbv1*fp11 + dbv2*fp12 + dbv3*fp13 ) + bu2 * ( dbv0*fp20 + dbv1*fp21 + dbv2*fp22 + dbv3*fp23 ) + bu3 * ( dbv0*fp30 + dbv1*fp31 + dbv2*fp32 + dbv3*fp33 );
    Normal = cross(dP_v,dP_u);
    
    Normal = mat3(transpose(inverse(model))) * Normal;
    
    FragPos = bu0 * ( bv0*fp00 + bv1*fp01 + bv2*fp02 + bv3*fp03 ) + bu1 * ( bv0*fp10 + bv1*fp11 + bv2*fp12 + bv3*fp13 ) + bu2 * ( bv0*fp20 + bv1*fp21 + bv2*fp22 + bv3*fp23 ) + bu3 * ( bv0*fp30 + bv1*fp31 + bv2*fp32 + bv3*fp33 );
}
