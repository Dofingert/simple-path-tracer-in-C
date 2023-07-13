#pragma once
#include "my_types.h"
#include "math.h"

#define MAX_SCEEN_SIZE 128 * 128
#define MINIMAL_DISTANCE -10.f
typedef enum
{
    BVH_OBJECT,
    TRI_OBJECT
} type_t;

typedef struct
{
    float v[3];
} vec3_t;

typedef struct
{
    uint32_t type;
    uint32_t son[2];
    vec3_t bound[2];
} bvh_t;

typedef struct
{
    uint32_t type;
    vec3_t p[3];
    vec3_t u[3];
    vec3_t v[3];
    vec3_t n;
    vec3_t s;
    uint32_t material_index;
} tri_t;

typedef union
{
    type_t type;
    uint32_t aabb_id;
    bvh_t bvh;
    tri_t tri;
} hitable_t;

typedef struct
{
    vec3_t src;
    vec3_t dir;
    vec3_t adder;
    vec3_t muler;
} ray_t;

hitable_t *alloc();
tri_t *init_a_triangle(tri_t *r, vec3_t *p, vec3_t *u, vec3_t *v, uint32_t material);
float interact_triangle(tri_t *tri, tri_t *last_hit, ray_t *ray);
int is_in_tri(tri_t *tri, vec3_t *p);
float interact_bvh(bvh_t *bvh, ray_t *ray, hitable_t *last_hit, hitable_t **next_hit);
uint32_t get_id(hitable_t *target);
hitable_t *get_hitable(uint32_t id);
hitable_t *build_scene(hitable_t *list, uint32_t sceen_size);
int get_scene(char *);
void trace_ray(ray_t *ray, hitable_t *scene);
void render(hitable_t *scene, vec3_t ori_point, vec3_t target_position, float fov_tan, int render_width,
            int render_height, int sample_rate, uint32_t *paint_table);

extern float MAX_FLOAT;

static inline vec3_t *self_add(vec3_t *self, vec3_t *other)
{
    for (int i = 0; i < 3; i++)
    {
        self->v[i] += other->v[i];
    }
    return self;
}

static inline vec3_t *self_sub(vec3_t *self, vec3_t *other)
{
    for (int i = 0; i < 3; i++)
    {
        self->v[i] -= other->v[i];
    }
    return self;
}

static inline vec3_t *self_mul(vec3_t *self, float value)
{
    for (int i = 0; i < 3; i++)
    {
        self->v[i] *= value;
    }
    return self;
}

static inline vec3_t *self_mul_vec(vec3_t *self, vec3_t *other)
{
    for (int i = 0; i < 3; i++)
    {
        self->v[i] *= other->v[i];
    }
    return self;
}

static inline vec3_t *sub(vec3_t *a, vec3_t *b, vec3_t *r)
{
    for (int i = 0; i < 3; i++)
    {
        r->v[i] = a->v[i] - b->v[i];
    }
    return r;
}

static inline vec3_t *add(vec3_t *a, vec3_t *b, vec3_t *r)
{
    for (int i = 0; i < 3; i++)
    {
        r->v[i] = a->v[i] + b->v[i];
    }
    return r;
}

static inline vec3_t *mul(vec3_t *a, float b, vec3_t *r)
{
    for (int i = 0; i < 3; i++)
    {
        r->v[i] = a->v[i] * b;
    }
    return r;
}

static inline vec3_t *cross(vec3_t *a, vec3_t *b, vec3_t *r)
{
    r->v[0] = a->v[1] * b->v[2] - a->v[2] * b->v[1];
    r->v[1] = a->v[2] * b->v[0] - a->v[0] * b->v[2];
    r->v[2] = a->v[0] * b->v[1] - a->v[1] * b->v[0];
    return r;
}

static inline vec3_t *random_vec(vec3_t *self)
{
    for (int i = 0; i < 3; i++)
    {
        uint32_t random_float = ((random_val() & 0x007FFFFF) | (128 << 23));
        self->v[i] = (*(float *)(&random_float)) - 3;
    }
    return self;
}

static inline float dot(vec3_t *a, vec3_t *b)
{
    float result = 0.f;
    for (int i = 0; i < 3; i++)
    {
        result += a->v[i] * b->v[i];
    }
    return result;
}

static inline float length(vec3_t *self)
{
    float result = 0.f;
    for (int i = 0; i < 3; i++)
    {
        result += self->v[i] * self->v[i];
    }
    return user_sqrt(result);
}

static inline vec3_t *normalize(vec3_t *self)
{
    float result = 0.f;
    for (int i = 0; i < 3; i++)
    {
        result += self->v[i] * self->v[i];
    }
    result = inv_sqrt(result);
    for (int i = 0; i < 3; i++)
    {
        self->v[i] *= result;
    }
    return self;
}

static inline vec3_t *copy(vec3_t *src, vec3_t *target)
{
    for (int i = 0; i < 3; i++)
    {
        target->v[i] = src->v[i];
    }
    return target;
}