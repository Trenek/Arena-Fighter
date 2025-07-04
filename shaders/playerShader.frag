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
    const float r = fragColor[0];
    const float g = fragColor[1];
    const float b = fragColor[2];

    const vec3 color = vec3(
        (r > 0.7 && g < 0.2) ? fragDressColor.xyz :
        (r > 0.7 && g > 0.4 && g < 0.5) ? fragSkinColor.xyz :
        fragColor
    );
    const vec3 lightColor = vec3(
        100.0 / 256.0, 200.0 / 256.0, 255.0 / 256.0
        //1.0, 1.0, 1.0
    );
    
    const vec3 center = vec3(0, 0, 3);
    const vec3 direction = normalize(center - fragVert);
    const float nor = distance(center.xy, fragVert.xy);
    const float diff = minmax(0.1, minmax(0.3, 0.9, log(5 - min(nor, 4)) * 2), dot(fragNormal, direction));

    outColor = vec4((diff * lightColor) * color, 1.0);
}
