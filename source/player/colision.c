#include <assert.h>
#include <string.h>

#include <ccd/ccd.h>
#include <cglm/cglm.h>

#include "player.h"
#include "entity.h"

#include "myInstance.h"
#include "model.h"
#include "gltf.h"

typedef float vec3[3];

void support_func(const void *obj, const ccd_vec3_t *dir, ccd_vec3_t *out) {
    const ccd_vec3_t *shape = obj;

    int max_idx = 0;
    float max_dot = ccdVec3Dot(&shape[0], dir);

    static_assert(sizeof(vec3) == sizeof(ccd_vec3_t));

    for (int i = 1; i < 8; i += 1) {
        float dot = ccdVec3Dot(&shape[i], dir);
        if (max_dot < dot) {
            max_dot = dot;
            max_idx = i;
        }
    }

    memcpy(out->v, &shape[max_idx], sizeof(vec3));
}

bool checkCubeCollision(vec3 shape1[8], vec3 shape2[8]) {
    return ccdGJKIntersect(shape1, shape2, &(ccd_t) {
        .first_dir = ccdFirstDirDefault,
        .support1 = support_func,
        .support2 = support_func,

        .max_iterations = -1,
        .epa_tolerance = CCD_REAL(0.0001),
        .mpr_tolerance = CCD_REAL(0.0001),
        .dist_tolerance = CCD_REAL(1e-6),
    });
}

static struct ColisionBox *find(size_t q, struct ColisionBox s[q], const char *name) {
    size_t i = 0;

    while (i < q && 0 != strcmp(s[i].name, name)) i += 1;

    return i == q ? NULL : &s[i];
}

static int getNodeID(struct AnimationData *anim, float ID) {
    return anim[(int)ID].jointToNodeID;
}

static void applyTransformations(struct ColisionBox cB, struct Entity *model, vec3 *out) {
    struct GltfVertex *vert = cB.vertex;
    struct playerInstanceBuffer *ins = model->buffer[0];
    struct AnimationData *anim = model->buffer[2];

    for (size_t i = 0; i < cB.qVertex; i += 1) {
        float *jointID = vert[i].joint;
        float *weight = vert[i].weight;

        size_t nodeID[] = {
            getNodeID(anim, jointID[0]),
            getNodeID(anim, jointID[1]),
            getNodeID(anim, jointID[2]),
            getNodeID(anim, jointID[3])
        };

        mat4 result;
        
        if (weight[0] == 0 && weight[1] == 0 && weight[2] == 0 && weight[3] == 0) {
            glm_mat4_identity(result);
        }
        else for (size_t j = 0; j < 4; j += 1) {
            mat4 *aaa[] = {
                &anim[nodeID[0]].animation,
                &anim[nodeID[1]].animation,
                &anim[nodeID[2]].animation,
                &anim[nodeID[3]].animation,
            };

            for (size_t k = 0; k < 4; k += 1)
            for (size_t z = 0; z < 4; z += 1)
                result[k][z] =
                    (*aaa[0])[k][z] * vert[i].weight[0] +
                    (*aaa[1])[k][z] * vert[i].weight[1] +
                    (*aaa[2])[k][z] * vert[i].weight[2] +
                    (*aaa[3])[k][z] * vert[i].weight[3]
                ;
        }

        glm_mat4_mulv3(result, vert[i].pos, 1, out[i]);
        glm_mat4_mulv3(ins->modelMatrix, out[i], 1, out[i]);
    }
}

static void add(size_t q, vec3 arr[q], vec3 toAdd) {
    for (size_t i = 0; i < q; i += 1) {
        glm_vec3_add(arr[i], toAdd, arr[i]);
    }
}

static bool checkCubesColision(struct ColisionBox cB1, struct Entity *model1, struct ColisionBox cB2, struct Entity *model2, vec3 toAdd) {
    vec3 transformed1[cB1.qVertex]; {
        applyTransformations(cB1, model1, transformed1);
        add(cB1.qVertex, transformed1, toAdd);
    }
    vec3 transformed2[cB2.qVertex]; {
        applyTransformations(cB2, model2, transformed2);
    }

    bool result = false;

    for (size_t i = 0; result == false && i < cB1.qVertex - 4; i += 4) {
        for (size_t j = 0; result == false && j < cB2.qVertex - 4; j += 4) {
            result = checkCubeCollision(transformed1 + i, transformed2 + j);
        }
    }

    return result;
}

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd) {
    struct GltfModelInfo *pInfo = p->model->info;
    struct GltfModelInfo *eInfo = p->enemy->model->info;

    struct ColisionBox *a = find(pInfo->qHitbox, pInfo->hitBox, name); {
        if (a == NULL) a = find(pInfo->qHurtBox, pInfo->hurtBox, name); 
        assert(a != NULL);
    }
    bool result = false;

    for (size_t i = 0; result == false && i < eInfo->qHurtBox; i += 1) {
        result = checkCubesColision(*a, p->entity, eInfo->hurtBox[i], p->enemy->entity, toAdd);
    }

    return result;
}

bool checkForColision(struct player *p, const char *name) {
    return checkForColisionToAdd(p, name, (vec3){});
}
