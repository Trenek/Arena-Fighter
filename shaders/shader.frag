#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat uint fragTexIndex;
layout(location = 3) in flat uint shadow;
layout(location = 4) in vec3 fragNormal;
layout(location = 5) in vec3 fragVertex;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

float minmax(float minn, float maxx, float val) {
    return max(minn, min(maxx, val));
}

void main() {
    vec3 color = texture(texSampler[fragTexIndex], fragTexCoord).rgb;

    const vec3 lightColor = vec3(
        100.0 / 256.0, 200.0 / 256.0, 255.0 / 256.0
        //1.0, 1.0, 1.0
    );
    
    const vec3 center = vec3(0, 0, 3);
    const vec3 direction = center - fragVertex;
    const float nor = distance(center.xy, fragVertex.xy);
    const float diff = minmax(0.1, 0.9, exp(-nor) * dot(fragNormal, direction));

    outColor = vec4((diff * lightColor) * color, 1.0);
}
