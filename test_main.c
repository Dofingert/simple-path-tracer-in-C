#include <stdio.h>
#include "scene.h"

hitable_t hitable_memory[MAX_SCEEN_SIZE];
uint32_t now_id;

hitable_t *alloc()
{
    hitable_t *ret = hitable_memory + now_id;
    now_id++;
    return ret;
}

uint32_t get_id(hitable_t *target)
{
    if (target == NULL)
    {
        return 0xffffffff;
    }
    return target - hitable_memory;
}

hitable_t *get_hitable(uint32_t id)
{
    if (id == 0xffffffff)
    {
        return NULL;
    }
    return hitable_memory + id;
}

void print_bvh_tree(hitable_t *tree, int level)
{
    if (tree == NULL)
    {
        return;
    }
    if (tree->type == TRI_OBJECT)
    {
        for (int i = 0; i < level; i++)
        {
            putchar('\t');
        }
        printf("Level %d : triangle\n", level);
        return;
    }
    for (int i = 0; i < level; i++)
    {
        putchar('\t');
    }
    printf("Level %d has a object\n", level);
    print_bvh_tree(get_hitable(tree->bvh.son[0]), level + 1);
    print_bvh_tree(get_hitable(tree->bvh.son[1]), level + 1);
}

#define res_x 4096
#define res_y 4096
#define sample_rate 128
#define fov_tan 0.75
uint32_t paint_board[res_y][res_x];

void write_to_file(char *file_name)
{
    FILE *fp = fopen(file_name, "w");
    fprintf(fp, "P3\n%d %d\n255\n", res_x, res_y);
    for (int y = res_y - 1; y >= 0; y--)
    {
        for (int x = 0; x < res_x; x++)
        {
            for (int i = 16; i >= 0; i -= 8)
            {
                fprintf(fp, "%d ", (paint_board[y][x] >> i) & 0xff);
            }
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}

int main()
{
    printf("Hello the world\n");
    ray_t test_ray = {
        {2.f, -5.f, 5.f},
        {0.f, 1.f, 0.f},
        {0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f}};
    // tri_t test_triangle;
    // vec3_t p[3] = {{0.f, 0.f, 0.f},
    //                {1.f, 0.f, 0.f},
    //                {0.f, 1.f, 1.f}};
    // vec3_t u[3] = {{0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f}};
    // vec3_t v[3] = {{0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f}};
    // init_a_triangle(&test_triangle, p, u, v, 0);
    // printf("result is %f\n",interact_triangle(&test_triangle,NULL,&test_ray));
    // It's working !!!
    int f = get_scene("test.scene");
    printf("We got %d frac\n", f);
    hitable_t *scene_root = build_scene(hitable_memory, f);
    print_bvh_tree(scene_root, 0);
    trace_ray(&test_ray, scene_root);
    printf("result is %f %f %f\n\n", test_ray.adder.v[0], test_ray.adder.v[1], test_ray.adder.v[2]);

    // tri_t test_triangle;
    // vec3_t p[3] = {{0.f, 0.f, 0.f},
    //                {1.f, 0.f, 0.f},
    //                {0.f, 1.f, 0.f}};
    // vec3_t u[3] = {{0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f}};
    // vec3_t v[3] = {{0.2f, 0.2f, 0.f},
    //                {0.f, 0.f, 0.f},
    //                {0.f, 0.f, 0.f}};
    // init_a_triangle(&test_triangle, p, u, v, 0);
    // printf("result is %f\n",is_in_tri(&test_triangle,v));
    vec3_t cam_point = {{5, -10, 5}};
    vec3_t target_pos = {{0, 1, 0}};
    render(scene_root, cam_point, target_pos, fov_tan, res_x, res_y, sample_rate, paint_board);
    write_to_file("test_output.ppm");
}