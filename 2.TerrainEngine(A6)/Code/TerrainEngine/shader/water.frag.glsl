#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 gl_FragCoord;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal0;
uniform sampler2D texture_normal1;
uniform sampler2D texture_flowmap;
uniform sampler2D texture_noise;
uniform sampler2D texture_reflect;
uniform sampler2D texture_refract;
uniform samplerCube skybox;

uniform float tiling;
uniform float time;
uniform vec3 ViewPos;

struct DirLight{
    vec3 lightColor;
    vec3 direction;
    float intensity;
};

uniform DirLight dirlight;

void main()
{
    float flowSpeed = 0.1f;
    vec2 flowTil = vec2(tiling/1.3f, tiling/1.3f);
    vec2 texTil = vec2(tiling, tiling);
    vec2 flowVec = (texture(texture_flowmap, TexCoords * flowTil).rg * 2.0f - 1.0f);
    float cycleOffset = texture(texture_noise, TexCoords* flowTil).r;
    
    float prePhase = cycleOffset + time;
    float phase0 = prePhase - floor(prePhase);
    float phase1 = prePhase + 0.5f - floor(prePhase + 0.5f);
    float lerpfactor = (abs(.5f - phase0) / .5f);
    
    vec4 normalT0 = texture(texture_normal0, TexCoords* texTil + flowVec * flowSpeed * phase0);
    vec4 normalT1 = texture(texture_normal1, TexCoords* texTil + flowVec * flowSpeed * phase1);
    vec3 normalMix = vec3(mix(normalT0, normalT1, lerpfactor));
    vec3 normalT;
    normalT.xz = normalMix.xy * 2.0f - 1.0f;
    normalT.y = sqrt(1 - normalT.x * normalT.x - normalT.z * normalT.z);

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 refDir = reflect(-viewDir, normalT);
    vec4 skyCol = texture(skybox, refDir);

    vec3 vRefrBump = normalT.xyz * vec3(0.075, 1.0, 0.075);
    vec3 vReflBump = normalT.xyz * vec3(0.02, 1.0, 0.02);

    vec2 texelSize = 1.0 / vec2(textureSize(texture_reflect, 0));
    vec4 refrColor = texture(texture_refract, gl_FragCoord.xy * texelSize + vRefrBump.xz);
    vec4 reflColor = texture(texture_reflect, gl_FragCoord.xy * texelSize + vReflBump.xz);
    vec4 refrB = texture(texture_refract, gl_FragCoord.xy * texelSize);

    vec4 refraction = refrB * refrColor.w + refrColor * (1 - refrColor.w);

    //dirlight highlight
    vec3 halfDir = normalize(viewDir - dirlight.direction);
    vec3 specular = pow(max(dot(normalT, halfDir), 0), 32) * dirlight.lightColor;
    
    color = vec4((skyCol.xyz * 0.8f + specular) * reflColor.xyz + 0.7f * refraction.xyz, 0.6f);
    //color = vec4(1.0f);
}
