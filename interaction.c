#include "scene.h"

float MAX_FLOAT = 1/0.f;

int is_in_tri(tri_t *tri, vec3_t *p)
{
    vec3_t v0, v1;
    vec3_t v2;
    sub(tri->p + 1, tri->p, &v0);
    sub(tri->p + 2, tri->p, &v1);
    sub(p, tri->p, &v2);
    float dot00 = dot(&v0, &v0);
    float dot01 = dot(&v0, &v1);
    float dot02 = dot(&v0, &v2);
    float dot11 = dot(&v1, &v1);
    float dot12 = dot(&v1, &v2);
    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
    if (u < 0 || u > 1)
    {
        return 0;
    }

    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
    if (v < 0 || v > 1)
    {
        return 0;
    }

    if (u + v <= 1.f)
    {
        return 1;
    }
    return 0;
}

float interact_triangle(tri_t *tri, tri_t *last_hit, ray_t *ray)
{
    if (last_hit == tri)
    {
        return MAX_FLOAT;
    }
    vec3_t line;
    sub(&ray->src, &tri->s, &line);
    float distance = dot(&line, &tri->n);
    float dotnline = dot(&tri->n, &ray->dir);
    if (distance * dotnline > 0)
    {
        return MAX_FLOAT;
    }
    distance /= -dotnline;
    mul(&ray->dir, float_abs(distance), &line);
    self_add(&line, &ray->src);
    if (is_in_tri(tri, &line))
    {
        return distance;
    }
    else
    {
        return MAX_FLOAT;
    }
}

int is_in_bvh(bvh_t *bvh, ray_t *ray)
{
    float t_min, t_max;
    int shift = 0;
    while (ray->dir.v[shift] <= 0.0001f)
    {
        shift++;
    }
    float invD = 1.0f / ray->dir.v[shift];
    float t0 = (bvh->bound[1].v[shift] - ray->src.v[shift]) * invD;
    float t1 = (bvh->bound[0].v[shift] - ray->src.v[shift]) * invD;
    if (invD < 0.0f)
    {
        t_min = t1;
        t_max = t0;
    }
    else
    {
        t_min = t0;
        t_max = t1;
    }
    for (int a = 1; a < 3; a++)
    {
        invD = 1.0f / ray->dir.v[(a + shift) % 3];
        t0 = (bvh->bound[1].v[(a + shift) % 3] - ray->src.v[(a + shift) % 3]) * invD;
        t1 = (bvh->bound[0].v[(a + shift) % 3] - ray->src.v[(a + shift) % 3]) * invD;
        if (invD < 0.0f)
        {
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max < t_min)
            return 0;
    }
    return 1;
}

float interact_bvh(bvh_t *bvh, ray_t *ray, hitable_t *last_hit, hitable_t **next_hit)
{
    if (is_in_bvh(bvh, ray))
    {
        float left_hit;
        float right_hit;
        hitable_t *l_son;
        hitable_t *l_hit;
        hitable_t *r_son;
        hitable_t *r_hit;
        l_son = get_hitable(bvh->son[0]);
        r_son = get_hitable(bvh->son[1]);
        if (l_son->type == BVH_OBJECT)
        {
            left_hit = interact_bvh(&l_son->bvh, ray, last_hit, &l_hit);
        }
        else
        {
            left_hit = interact_triangle(&l_son->tri, last_hit, ray);
            l_hit = &l_son->tri;
        }
        if (r_son == 0xffffffff) // Should never happen!!
        {
            *next_hit = l_hit;
            return left_hit;
        }
        if (r_son->type == BVH_OBJECT)
        {
            right_hit = interact_bvh(&r_son->bvh, ray, last_hit, &r_hit);
        }
        else
        {
            right_hit = interact_triangle(&r_son->tri, last_hit, ray);
            r_hit = &r_son->tri;
        }
        if (left_hit < right_hit && left_hit > MINIMAL_DISTANCE)
        {
            *next_hit = l_hit;
            return left_hit;
        }
        else if (right_hit > MINIMAL_DISTANCE)
        {
            *next_hit = r_hit;
            return right_hit;
        }
        *next_hit = 0;
        return MAX_FLOAT;
    }
    else
    {
        *next_hit = 0;
        return MAX_FLOAT;
    }
}