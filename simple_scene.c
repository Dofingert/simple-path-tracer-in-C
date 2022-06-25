#include <stdio.h>
#include "scene.h"

int get_scene(char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    vec3_t p[3];
    int cnt = 0;
    int material;
    while (fscanf(fp, "%x", &material) != EOF)
    {
        cnt++;
        tri_t *new_tri = alloc();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                fscanf(fp, "%f", &new_tri->p[i].v[j]);
            }
        }
        init_a_triangle(new_tri,new_tri->p,new_tri->p,new_tri->v,material);
    }
    fclose(fp);
    return cnt;
}