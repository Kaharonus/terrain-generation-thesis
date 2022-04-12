#version 420 core


out vec4 FragColor;
in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedo;
layout(binding = 3) uniform sampler2D gSpecular;
layout(binding = 4) uniform sampler2D ssao;

uniform vec3 cameraPos;
vec3 CalcDirLight(vec3 normal){
    vec3 ambientColor = vec3(135, 206, 235) / 255.0;

    //Get AO
    float ao = texture(ssao, TexCoords).r;

    //Calculate light direction
    vec3 lightDir = vec3(0.5, 1.0, 0.0);
    //calculate specular
    float specularStrength = texture(gSpecular, TexCoords).b;
    vec3 viewDir = normalize(cameraPos - texture(gPosition, TexCoords).xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * ambientColor;

    //Calculate ambient strengh
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * ambientColor;

    //calculate diffuse strength
    vec3 diffuseColor = vec3(256,251,211) / 255.0;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;
    return (ambient + diffuse + specular) * ao;
}

void main() {
    vec4 AlbedoWithTransparency = texture(gAlbedo, TexCoords);
    if(AlbedoWithTransparency.a == 0.0){
        FragColor = vec4(0.0);
        return;
    }
    vec3 Albedo = AlbedoWithTransparency.rgb;
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 light = CalcDirLight(Normal);
    FragColor = vec4(Albedo * light, 1.0);
    //FragColor = vec4(ao, ao, ao ,1.0);
}
