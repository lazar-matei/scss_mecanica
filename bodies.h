#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G 1   
#define CT_RAZA 1e9      

typedef struct {
    float x;
    float y;
    float z;
} VECT;

typedef struct {
    float mass;      
    VECT r;          
    VECT v;          
    VECT a;          
    float raza;      
    FILE *file;      
} BODY;

typedef struct {
    VECT v;         
    VECT a;         
} K_rk4;

// Funcții pentru manevrarea corpurilor
void read_init_value(BODY *bod, int nr_corp);
void read_init_velocity(BODY *bod, int nr_corp);
float putere(float x, int a);
float dist32(BODY a, BODY b, float pas, VECT k_a, VECT k_b);
void init_acc(BODY *corp, int nr_total);
void rez_acc_vit(BODY *corp, int nr_total, float h, K_rk4 *k);
void get_k(BODY *corp, int nr_total, K_rk4 **k);
void cpy_corp(BODY *corp, int nr_total, BODY **corp_cpy);
void rk4(BODY *corp, int nr_total, float h);
float dist_OB(BODY a);
void update_size(BODY *corp, int nr_total);
void write_body_state(BODY *corp, float time);
void check4collision(BODY *corp, int nr_total); 
void collision(BODY *corp1, BODY *corp2);
float distanta(BODY a, BODY b); 

