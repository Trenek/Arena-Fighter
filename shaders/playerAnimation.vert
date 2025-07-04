#version 450

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inColor;
layout(location = 2) in  vec2 inTexCoord;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in  vec4 inWeights;
layout(location = 5) in  vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragDressColor;
layout(location = 3) out vec4 fragSkinColor;
layout(location = 4) out vec3 fragNormal;
layout(location = 5) out vec3 fragVert;

layout(set = 2, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

struct ObjectData {
    mat4 model;
    vec4 dressColor;
    vec4 skinColor;
};

layout(std140, set = 0, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} instance;

layout(std140, set = 0, binding = 1) readonly buffer MeshBuffer{
	mat4 localModel[];
} mesh;

layout(std140, set = 0, binding = 2) readonly buffer AnimBuffer{
	mat4 transform[];
} joint;

layout(push_constant) uniform constants {
	int meshID;
} PushConstants;

vec4 applyBoneTransform(vec4 p) {
    vec4 result = vec4(0.0);

    for (int i = 0; i < 4; i += 1) {
        result += joint.transform[inBoneIDs[i]] * (inWeights[i] * p);
    }

    return result;
}

void main() {
    vec4 position = applyBoneTransform(vec4(inPosition, 1.0));
    vec4 normal = applyBoneTransform(vec4(inNormal, 0.0));
    mat4 worldTransform = (
        instance.objects[gl_InstanceIndex].model *
        mesh.localModel[PushConstants.meshID]
    );

    fragVert = (
        worldTransform *
        position
    ).xyz;

    gl_Position = (
        ubo.proj * 
        ubo.view * 
        vec4(fragVert, 1.0)
    );

    fragNormal = normalize(
        worldTransform *
        normal
    ).xyz;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragDressColor = instance.objects[gl_InstanceIndex].dressColor;
    fragSkinColor = instance.objects[gl_InstanceIndex].skinColor;
}
