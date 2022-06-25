#include "scene.h"

float random_float_val()
{
    uint32_t random_float = ((random_val() & 0x007FFFFF) | (127 << 23));
    return (*(float *)(&random_float)) - 1;
}

void render(hitable_t *scene, vec3_t ori_point, vec3_t target_position, float fov_tan, int render_width,
            int render_height, int sample_rate, uint32_t *paint_table)
{
    normalize(&target_position);
    vec3_t length_in_x;
    vec3_t length_in_y = {0.f, 0.f, 1.f * fov_tan * (float)render_height / render_width};
    cross(&target_position, &length_in_y, &length_in_x);
    normalize(&length_in_x);
    self_mul(&length_in_x, 1.f * fov_tan);
    self_add(&target_position, &ori_point);
    self_sub(&target_position, &length_in_x);
    self_add(&target_position, &length_in_y);
    self_mul(&length_in_x, 2.f / render_width);
    self_mul(&length_in_y, 2.f / render_height);
    ray_t ray;
    copy(&ori_point, &ray.src);
    float const_transfer = 255.f / (sample_rate);

    print_vec3(&ori_point);
    print_vec3(&length_in_x);
    print_vec3(&length_in_y);
    putchar('\n');
    print_vec3(&target_position);
    putchar('\n');
    vec3_t temp_vec;
    for (int j = render_height - 1; j >= 0; j--)
    {
        copy(&target_position, &temp_vec);
        for (int i = 0; i < render_width; i++)
        {
            vec3_t color = {{0.f, 0.f, 0.f}};
            for (int k = 0; k < sample_rate; k++)
            {
                sub(&temp_vec, &ori_point, &ray.dir);
                copy(&ori_point, &ray.src);
                float u = random_float_val();
                vec3_t l_x;
                mul(&length_in_x, u, &l_x);
                self_add(&ray.dir, &l_x);
                u = random_float_val();
                mul(&length_in_y, u, &l_x);
                self_sub(&ray.dir, &l_x);
                normalize(&ray.dir);
                trace_ray(&ray, scene);
                self_add(&color, &ray.adder);
            }
            self_mul(&color, const_transfer);
            *(paint_table + i + j * render_width) =
                (((unsigned char)color.v[0]) << 16) |
                (((unsigned char)color.v[1]) << 8) |
                (((unsigned char)color.v[2]) << 0); // RGB
            self_add(&temp_vec, &length_in_x);
        }
        self_sub(&target_position, &length_in_y);
        printf("Render %d line over\n",j);
    }
    print_vec3(&temp_vec);
    putchar('\n');
}