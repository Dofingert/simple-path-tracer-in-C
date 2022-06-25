#include "scene.h"

void print_vec3(vec3_t *vec3)
{
    printf("%f %f %f ", vec3->v[0], vec3->v[1], vec3->v[2]);
}

void miss_ray(ray_t *ray)
{
    static vec3_t sky_color = {{0.75f, 0.75f, 0.75f}};
    self_mul_vec(&ray->muler, &sky_color);
    self_add(&ray->adder, &ray->muler);
    return;
}

void trace_ray(ray_t *ray, hitable_t *scene)
{
    // Color is writen to ray.adder
    static int max_trace_depth = 4;
    static vec3_t zero_vec = {{0.f, 0.f, 0.f}};
    static vec3_t one_vec = {{1.f, 1.f, 1.f}};
    copy(&zero_vec, &ray->adder);
    copy(&one_vec, &ray->muler);
    hitable_t *last_hit = 0;
    tri_t *h_tri = 0;
    for (int i = 0; i < max_trace_depth; i++)
    {
        float distance;
        if (scene->type == BVH_OBJECT)
        {
            hitable_t *next_hit;
            // Do some prepare work to make it possible for Triangle hit test.
            if ((distance = interact_bvh(&scene->bvh, ray, last_hit, &next_hit)) != MAX_FLOAT)
            {
                h_tri = &next_hit->tri;
                goto tri_cal;
            }
            else
            {
                miss_ray(ray);
                return;
            }
        }
        h_tri = &scene->tri;
        distance = interact_triangle(h_tri, last_hit, ray);
        if (distance == MAX_FLOAT)
        {
            miss_ray(ray);
            return;
        }
    tri_cal:
        vec3_t hit_point;
        mul(&ray->dir, distance, &hit_point);
        self_add(&hit_point, &ray->src);
        // Material has 32bit, lower 24 bit is used for color discription, high 8 bit is used for matalitic.
        // Firstly, we will only create a render with only diffuse.
        vec3_t color_muler = {{((float)((h_tri->material_index >> 16) & 0xff)) / 255.f,
                               ((float)((h_tri->material_index >> 8) & 0xff)) / 255.f,
                               ((float)((h_tri->material_index >> 0) & 0xff)) / 255.f}}; // RGB
        self_mul_vec(&ray->muler, &color_muler);

        // Next, we generate a random vec3 in space, and then transfer its into subsphare_space according to normal.
        vec3_t tmp_random_vec;
        random_vec(&tmp_random_vec);
        normalize(&tmp_random_vec);
        self_add(&tmp_random_vec, &h_tri->n);
        if (dot(&ray->dir, &h_tri->n) > 0.f)
        {
            self_mul(&tmp_random_vec, -1.f);
        }
        normalize(&tmp_random_vec);
        copy(&tmp_random_vec, &ray->dir);
        copy(&hit_point, &ray->src);
        last_hit = h_tri;
    }
    static vec3_t no_hit_color = {{0.01f, 0.01f, 0.01f}};
    self_mul_vec(&ray->muler, &no_hit_color);
    self_add(&ray->adder, &ray->muler);
    return;
}