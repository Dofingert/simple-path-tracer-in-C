#include "scene.h"
#include "math.h"

tri_t *init_a_triangle(tri_t *r, vec3_t *p, vec3_t *u, vec3_t *v, uint32_t material)
{
    for (int i = 0; i < 3; i++)
    {
        copy(p + i, &r->p[i]);
        copy(u + i, &r->u[i]);
        copy(v + i, &r->v[i]);
    }
    r->material_index = material;
    copy(p, &r->s);
    self_add(&r->s, p + 1);
    self_add(&r->s, p + 2);
    self_mul(&r->s, 1.f / 3);
    vec3_t v0v1;
    vec3_t v0v2;
    sub(p + 1, p, &v0v1);
    sub(p + 2, p, &v0v2);
    cross(&v0v1, &v0v2, &r->n);
    normalize(&r->n);
    r->type = TRI_OBJECT;
    return r;
}