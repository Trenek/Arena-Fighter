#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragDressColor;
layout(location = 3) in vec4 fragSkinColor;
layout(location = 4) in vec3 fragNormal;
layout(location = 5) in vec3 fragVert;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

float minmax(float minn, float maxx, float val) {
    return max(minn, min(maxx, val));
}

void main() {
    float r = fragColor[0];
    float g = fragColor[1];
    float b = fragColor[2];

    outColor = vec4(
        (r > 0.7 && g < 0.2) ? fragDressColor.xyz :
        (r > 0.7 && g > 0.4 && g < 0.5) ? fragSkinColor.xyz :
        fragColor,
    1.0);
    
    vec3 direction = vec3(-1, -1, 1);
    float diff = minmax(0.1, 0.9, dot(fragNormal, direction));
    outColor = diff * outColor;
}
