#version 410 core
out vec4 FragColor;

in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;

uniform vec3 lightPos;
uniform sampler2D Texture;

bool equals(float a, float b) {
    b = b / 255.0;
    return abs(a - b) < 0.000001;
}

void main(){
    float ambientStrength = 0.25;
    vec3 lightColor = vec3(247, 245, 230) / 255.0;

    vec4 metadata = texture(Texture, TexCoord_FS_in);
    float r = metadata.r;
    vec4 color;
    if (r == 255){ // Pixel is used as a "unkown" area
        color = vec4(1.0);
    } else if (equals(r, 1.0)){ //Pixel is used as a forrest
        color = vec4(0, 0.29, 0.04, 1);
    } else if (equals(r, 2.0)){ //Pixel is used as a water
        color = vec4(0.0, 0.0, 0.5, 1.0);
    } else if (equals(r, 3.0)){ //Pixel is used as a field
        color = vec4(0.96, 0.835, 0.013, 1.0);
    } else if (equals(r, 4.0)){ //Pixel is used as a road
        color = vec4(0.7, 0.32, 0.08, 1.0);
    } else if (equals(r, 5.0)){ //Pixel is used as a building
        color = vec4(0.78, 0.78, 0.78, 1.0);
    } else if (equals(r, 6.0)){ //Pixel is used as a park
        color = vec4(0.0, 0.5, 0.0, 1.0);
    } else if (equals(r, 7.0)){ //Pixel is used as a residential/industrial area
        color = vec4(0.5, 0.5, 0.5, 1.0);
    } else {
        color = vec4(1.0f);
    }

    vec3 norm = normalize(Normal_FS_in);
    vec3 lightDir = normalize(lightPos - WorldPos_FS_in);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = ambientStrength * color.rgb;

    float shading =  1 - ((1 - dot(Normal_FS_in, vec3(0.0, 1.0, 0.0))) * 4.0);
    vec3 shadingColor = color.rgb * shading;
    vec3 result = (ambient + diffuse) * shadingColor;
    FragColor = vec4(result, 1.0);
}
