#version 450

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inNormal;
layout(location = 2) in  vec2 inTexCoord;
layout(location = 3) in  vec3 inColor;
layout(location = 4) in  vec4 inWeights;
layout(location = 5) in  vec4 inBoneIDs;
layout(location = 6) in  vec3 inMorphPos1;
layout(location = 7) in  vec3 inMorphPos2;
layout(location = 8) in  vec3 inMorphPos3;
layout(location = 9) in  vec3 inMorphPos4;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragDressColor;
layout(location = 3) out vec4 fragSkinColor;
layout(location = 4) out vec3 fragNormal;
layout(location = 5) out vec3 fragVert;

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

struct ObjectData {
    mat4 model;
    vec4 dressColor;
    vec4 skinColor;
};

struct NodeData {
    mat4 animation;
    float weight1;
    float weight2;
    float weight3;
    float weight4;

    int jointToNodeID;
    int a;
    int b;
    int c;
};

layout(std140, set = 2, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} instance;

layout(std140, set = 2, binding = 1) readonly buffer MeshBuffer{
	mat4 localModel[];
} mesh;

layout(std140, set = 2, binding = 2) readonly buffer AnimBuffer{
	NodeData transform[];
} joint;

layout(push_constant) uniform constants {
	int meshID;
} PushConstants;

vec3 applyMorph(vec3 pos) {
    vec3 result = pos;

    result += joint.transform[PushConstants.meshID].weight1 * inMorphPos1;
    result += joint.transform[PushConstants.meshID].weight2 * inMorphPos2;
    result += joint.transform[PushConstants.meshID].weight3 * inMorphPos3;
    result += joint.transform[PushConstants.meshID].weight4 * inMorphPos4;

    return result;
}

int getNodeID(float ID) {
    return joint.transform[int(ID)].jointToNodeID;
}

mat4 applyBoneTransform() {
    mat4 result = mat4(0.0);

    if (all(equal(inWeights, vec4(0.0)))) {
        result = mat4(1.0);
    }
    else for (int i = 0; i < 4; i += 1) {
        result += joint.transform[getNodeID(inBoneIDs[i])].animation * inWeights[i];
    }

    return result;
}

void main() {
    vec4 position = vec4(inPosition, 1.0);
    vec4 normal = vec4(inNormal, 0.0);
    mat4 worldTransform = (
        instance.objects[gl_InstanceIndex].model *
        mesh.localModel[PushConstants.meshID]
    );
    mat4 boneTransform = applyBoneTransform();

    fragVert = (
        worldTransform *
        boneTransform *
        position
    ).xyz;

    gl_Position = (
        ubo.proj * 
        ubo.view * 
        vec4(fragVert, 1.0)
    );

    fragNormal = normalize(
        worldTransform *
        boneTransform *
        normal
    ).xyz;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragDressColor = instance.objects[gl_InstanceIndex].dressColor;
    fragSkinColor = instance.objects[gl_InstanceIndex].skinColor;
}
