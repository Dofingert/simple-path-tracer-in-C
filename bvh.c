#include "scene.h"
#include "math.h"

void get_tri_range(hitable_t *hitable, vec3_t *up, vec3_t *down)
{
    copy(hitable->tri.p, up);
    copy(hitable->tri.p, down);
    for (int i = 1; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (hitable->tri.p[i].v[j] > up->v[j])
            {
                up->v[j] = hitable->tri.p[i].v[j];
            }
            if (hitable->tri.p[i].v[j] < down->v[j])
            {
                down->v[j] = hitable->tri.p[i].v[j];
            }
        }
    }
}

void get_range(hitable_t *hitable, vec3_t *up, vec3_t *down)
{
    if (hitable->type == TRI_OBJECT)
    {
        get_tri_range(hitable, up, down);
    }
    else if (hitable->type == BVH_OBJECT)
    {
        copy(hitable->bvh.bound, up);
        copy(hitable->bvh.bound + 1, down);
    }
}

void get_range_multi(hitable_t *hitable_a, hitable_t *hitable_b, vec3_t *up, vec3_t *down)
{
    vec3_t up_a;
    vec3_t down_a;
    vec3_t up_b;
    vec3_t down_b;
    get_range(hitable_a, &up_a, &down_a);
    get_range(hitable_b, &up_b, &down_b);
    for (int i = 0; i < 3; i++)
    {
        up->v[i] = up_a.v[i] > up_b.v[i] ? up_a.v[i] : up_b.v[i];
        down->v[i] = down_a.v[i] < down_b.v[i] ? down_a.v[i] : down_b.v[i];
    }
}

bvh_t *build_bvh(hitable_t *l_hitable, hitable_t *r_hitable, bvh_t *target)
{
    target->type = BVH_OBJECT;
    target->son[0] = get_id(l_hitable);
    target->son[1] = get_id(r_hitable);
    get_range_multi(l_hitable, r_hitable, target->bound, target->bound + 1);
    return target;
}

int get_morton_code(vec3_t *point, vec3_t *minimal, vec3_t *range)
{
    int ret = 0;
    float normalize[3];
    int normalizeInt[3];
    for (int i = 0; i < 3; i++)
    {
        normalize[i] = (point->v[i] - minimal->v[i]);
        normalize[i] = normalize[i] * 1023 / range->v[i];
        normalizeInt[i] = (int)normalize[i];
    }

    for (int i = 0; i < 9; i++)
    {
        ret |= (((normalizeInt[0] & 0x1) << 2) |
                ((normalizeInt[1] & 0x1) << 1) |
                (normalizeInt[2] & 0x1))
               << 27;
        ret = ret >> 3;
        normalizeInt[0] >>= 1;
        normalizeInt[1] >>= 1;
        normalizeInt[2] >>= 1;
    }
    ret |= ((normalizeInt[0] << 2) |
            (normalizeInt[1] << 1) |
            normalizeInt[2])
           << 27;

    return ret;
}

void get_tri_range_global(hitable_t *hitable, vec3_t *up, vec3_t *down)
{
    for (int i = 1; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (hitable->tri.p[i].v[j] > up->v[j])
            {
                up->v[j] = hitable->tri.p[i].v[j];
            }
            if (hitable->tri.p[i].v[j] < down->v[j])
            {
                down->v[j] = hitable->tri.p[i].v[j];
            }
        }
    }
}

int morton_cmp(void *a, void *b)
{
    hitable_t *pa = a;
    hitable_t *pb = b;
    return pa->aabb_id - pb->aabb_id;
}

#include <stdlib.h>
uint32_t id_list[MAX_SCEEN_SIZE];
hitable_t *build_scene(hitable_t *list, uint32_t sceen_size)
{
    vec3_t up, down;
    // First run, we get all size info;
    copy(list->tri.p, &up);
    copy(list->tri.p, &down);
    for (int i = 1; i < sceen_size; i++)
    {
        get_tri_range_global(list + i, &up, &down);
    }
    // Second run, we build morton code;
    self_sub(&up, &down);
    for (int i = 0; i < sceen_size; i++)
    {
        list[i].aabb_id = get_morton_code(&list[i].tri.s, &down, &up);
    }
    // Third run, we do a sort and initialize the type
    qsort(list, sceen_size, sizeof(hitable_t), morton_cmp);
    for (int i = 0; i < sceen_size; i++)
    {
        list[i].type = TRI_OBJECT;
        id_list[i] = i;
    }
    // Forth run, we build bvh.
    int levelSize = sceen_size;
    int level = 0;
    hitable_t* new_bvh;
    while (levelSize >= 2 || level == 0)
    {
        for (int i = 0; i < levelSize - 1; i += 2)
        {
            int index1, index2;
            index1 = id_list[i];
            index2 = id_list[i + 1];
            new_bvh = alloc();
            hitable_t* hitable_l = get_hitable(index1);
            hitable_t* hitable_r = get_hitable(index2);
            build_bvh(hitable_l,hitable_r,&new_bvh->bvh);
            id_list[i / 2] = get_id(new_bvh);
        }
        if (levelSize % 2 == 1)
        {
            new_bvh = get_hitable(id_list[levelSize - 1]);
            id_list[levelSize / 2] = id_list[levelSize - 1];
        }
        // System.out.printf("%dlevel: %dBvh\n",level,levelSize);
        levelSize = (levelSize + 1) / 2;
        level++;
        // System.out.printf("%dAflevel: %dBvh\n",level,levelSize);
    }
    return get_hitable(id_list[0]);
}