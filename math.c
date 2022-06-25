#include "math.h"
uint32_t seed = 0xdeadbedf;
uint32_t random_val()
{
    seed = (seed * 1664525) + 1013904223;
    return seed;
}

float inv_sqrt(float x) // Carm Invert Sqrt
{

    float xhalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5f375a86 - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

float user_sqrt(float x) // Carm Sqrt
{
    union
    {
        int intPart;
        float floatPart;
    } convertor;

    union
    {
        int intPart;
        float floatPart;
    } convertor2;

    convertor.floatPart = x;
    convertor2.floatPart = x;
    convertor.intPart = 0x1FBCF800 + (convertor.intPart >> 1);
    convertor2.intPart = 0x5f3759df - (convertor2.intPart >> 1);
    return 0.5f * (convertor.floatPart + (x * convertor2.floatPart));
}

float float_abs(float input)
{
    return (input > 0.f) ? input : -input;
}