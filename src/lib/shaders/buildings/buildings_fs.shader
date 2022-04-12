#version 420 core
in vec4 gl_FragCoord;

in vec3 WorldPos_VS_Out;
in vec3 Normal_VS_Out;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gSpecular;

out vec4 FragColor;
//in vec2 TexCoord;

uniform vec3 lightPos;


void main() {
    vec3 baseColor = vec3(70, 70, 100) / 255.0;
    gPosition = WorldPos_VS_Out.xyz;
    gNormal = normalize(Normal_VS_Out).xyz;
    gAlbedo = vec4(baseColor.rgb, 1.0);
    gSpecular = vec4(0.0);
}
