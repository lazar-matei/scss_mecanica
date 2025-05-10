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
float putere(float x, int a); 
float dist32(BODY a, BODY b, float pas, VECT k_a, VECT k_b); 
void init_acc(BODY *corp, int nr_total);
void rez_acc_vit(BODY *corp,int nr_total, float h, K_rk4 *k); 
void get_k(BODY *corp, int nr_total, K_rk4 **k); 
void cpy_corp(BODY *corp, int nr_total, BODY **corp_cpy); 
void rk4(BODY *corp, int nr_total, float h); 
float dist_OB(BODY a); 
void update_size(BODY *corp, int nr_total); 