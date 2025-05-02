#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 0.00000000006674301515
#define CT_RAZA 1

struct vect{
    float x, y, z;
};
typedef struct vect VECT;

struct corp{
    VECT r;
    VECT v;
    VECT a;
    float mass;
    float raza;
    FILE *file;
};
typedef struct corp BODY;

struct krk{
    VECT v, a;
};
typedef struct krk K_rk4;

void read_init_value(BODY *bod, int nr_corp);