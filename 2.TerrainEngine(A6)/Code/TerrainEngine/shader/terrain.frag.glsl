#version 330 core

struct DirLight{
    vec3 lightColor;
    vec3 direction;
    float intensity;
};

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir);

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_detail;

uniform vec4 clipping;
uniform vec3 ViewPos;
uniform vec3 AmbientColor;
uniform DirLight dirlight;

void main()
{
    float clipPos = dot(FragPos, clipping.xyz) + clipping.w;
    if (clipPos < 0.0) {
        discard;
    }
    vec3 ViewDir = normalize(vec3(ViewPos - FragPos));
    vec3 normal = normalize(Normal);
    vec3 result = AmbientColor;
    result += CalcDirColor(dirlight, normal, ViewDir);

    color = vec4(result, 1.0f);
}

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir){
    vec3 albedo = vec3(texture(texture_diffuse, TexCoords));
    vec2 detailTil = vec2(20.0f, 20.0f);
    vec3 detail = vec3(texture(texture_detail, TexCoords * detailTil));
    vec3 diffuse = (0.5f * dot(-light.direction, normal) + 0.5f) * light.lightColor * albedo;
    vec3 halfDir = normalize(viewDir - light.direction);
    vec3 specular = pow(max(dot(normal, halfDir), 0), 32) * light.lightColor * albedo;
    return (diffuse * detail) * light.intensity;
}
