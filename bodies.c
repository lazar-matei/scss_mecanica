#include "bodies.h"

float h;

void read_init_value(BODY *bod, int nr_corp)
{
    printf("m_%d: ", nr_corp);
    scanf("%f", &bod->mass);
    printf("x_0_%d: ", nr_corp);
    scanf("%f", &bod->r.x);
    printf("y_0_%d: ", nr_corp);
    scanf("%f", &bod->r.y);
    printf("z_0_%d: ", nr_corp);
    scanf("%f", &bod->r.z);

    printf("m_%d: ", nr_corp);
    scanf("%f", &bod->mass);
    printf("v_x_0_%d: ", nr_corp);
    scanf("%f", &bod->v.x);
    printf("v_y_0_%d: ", nr_corp);
    scanf("%f", &bod->v.y);
    printf("v_z_0_%d: ", nr_corp);
    scanf("%f", &bod->v.z);
}

float putere(float x, int a)
{
    if(a==0) return 1;
    return x*putere(x, a-1);
}

float dist32(BODY a, BODY b, float pas, VECT k_a, VECT k_b) //face prod scalar ^3 cu defazaju k
{
    return sqrtf(putere(putere((a.r.x+pas*k_a.x - b.r.x+pas*k_b.x), 2) + putere((a.r.y+pas*k_a.y - b.r.y+pas*k_b.y), 2) + putere((a.r.z+pas*k_a.z - b.r.z+pas*k_b.z), 2), 3));
}

void init_acc(BODY *corp, int nr_totoal)
{
    int i;
    for(i=0; i<nr_totoal; i++)
    {
        (corp+i)->a.x = 0;
        (corp+i)->a.y = 0;
        (corp+i)->a.z = 0;
    }
}

void rez_acc_vit(BODY *corp,int nr_total, float h, K_rk4 *k)
{
    int i, j;

    init_acc(corp, nr_total);

    for(i=0; i<nr_total; i++)
    {    
        for(j=0; j<nr_total; j++)
        {
            if(i!=j)
            {
                float dist = (dist32(*((corp)+i), *((corp)+j), h,(k+i)->v,(k+j)->v));
                ((corp) + i)->a.x += ((corp)+j)->mass/dist*(((corp)+j)->r.x+(k+j)->v.x - ((corp)+i)->r.x+(k+i)->v.x); 
                ((corp) + i)->a.y += ((corp)+j)->mass/dist*(((corp)+j)->r.y+(k+j)->v.y - ((corp)+i)->r.y+(k+i)->v.y); 
                ((corp) + i)->a.z += ((corp)+j)->mass/dist*(((corp)+j)->r.z+(k+j)->v.z - ((corp)+i)->r.z+(k+i)->v.z); 

            }

        }
        ((corp) + i)->a.x *= G;
        ((corp) + i)->a.y *= G;
        ((corp) + i)->a.z *= G;

        ((corp) + i)->v.x += h*(k+i)->a.x;
        ((corp) + i)->v.y += h*(k+i)->a.y;
        ((corp) + i)->v.z += h*(k+i)->a.z;

    }
}

void get_k(BODY *corp, int nr_total, K_rk4 **k)
{
    int i;
    (*k)=(K_rk4*)malloc(sizeof(K_rk4)*nr_total);
    for(i=0; i<nr_total; i++)
    {
        ((*k)+i)->v.x = (corp+i)->v.x;
        ((*k)+i)->v.y = (corp+i)->v.y;
        ((*k)+i)->v.z = (corp+i)->v.z;

        ((*k)+i)->a.x = (corp+i)->a.x;
        ((*k)+i)->a.y = (corp+i)->a.y;
        ((*k)+i)->a.z = (corp+i)->a.z;
    }
}

void cpy_corp(BODY *corp, int nr_total, BODY **corp_cpy)
{
    (*corp_cpy)=(BODY*)malloc(sizeof(BODY)*nr_total);
    int i;
    for(i=0; i<nr_total; i++)
    {
        ((*corp_cpy)+i)->a.x = (corp+i)->a.x;
        ((*corp_cpy)+i)->a.y = (corp+i)->a.y;
        ((*corp_cpy)+i)->a.z = (corp+i)->a.z;

        ((*corp_cpy)+i)->v.x = (corp+i)->v.x;
        ((*corp_cpy)+i)->v.y = (corp+i)->v.y;
        ((*corp_cpy)+i)->v.z = (corp+i)->v.z;

        ((*corp_cpy)+i)->r.x = (corp+i)->r.x;
        ((*corp_cpy)+i)->r.y = (corp+i)->r.y;
        ((*corp_cpy)+i)->r.z = (corp+i)->r.z;

        ((*corp_cpy)+i)->mass = (corp+i)->mass;
        ((*corp_cpy)+i)->raza = (corp+i)->raza;
        ((*corp_cpy)+i)->file = (corp+i)->file;

    }
}

void rk4(BODY *corp, int nr_total, float h)
{
    K_rk4 *k1, *k2, *k3, *k4;
    BODY *copr_cpy;

    cpy_corp(corp, nr_total, &copr_cpy);
    rez_acc_vit(copr_cpy, nr_total, 0, NULL);
    get_k(copr_cpy, nr_total, &k1);
    free(copr_cpy);

    cpy_corp(corp, nr_total, &copr_cpy);
    rez_acc_vit(copr_cpy, nr_total, h/2, k1);
    get_k(copr_cpy, nr_total, &k2);
    free(copr_cpy);

    cpy_corp(corp, nr_total, &copr_cpy);
    rez_acc_vit(copr_cpy, nr_total, h/2, k2);
    get_k(copr_cpy, nr_total, &k3);
    free(copr_cpy);

    cpy_corp(corp, nr_total, &copr_cpy);
    rez_acc_vit(copr_cpy, nr_total, h, k3);
    get_k(copr_cpy, nr_total, &k4);
    free(copr_cpy);

    int i;
    for(i=0; i<nr_total; i++)
    {
        (corp+i)->r.x += h/6*((k1+i)->v.x + 2*(k2+i)->v.x + 2*(k3+i)->v.x + (k4+i)->v.x);
        (corp+i)->r.y += h/6*((k1+i)->v.y + 2*(k2+i)->v.y + 2*(k3+i)->v.y + (k4+i)->v.y);
        (corp+i)->r.z += h/6*((k1+i)->v.z + 2*(k2+i)->v.z + 2*(k3+i)->v.z + (k4+i)->v.z);

        (corp+i)->v.x += h/6*((k1+i)->a.x + 2*(k2+i)->a.x + 2*(k3+i)->a.x + (k4+i)->a.x);
        (corp+i)->v.y += h/6*((k1+i)->a.y + 2*(k2+i)->a.y + 2*(k3+i)->a.y + (k4+i)->a.y);
        (corp+i)->v.z += h/6*((k1+i)->a.z + 2*(k2+i)->a.z + 2*(k3+i)->a.z + (k4+i)->a.z);

    }

}

float dist_OB(BODY a)
{
    return sqrtf(putere(a.r.x, 2)+putere(a.r.y, 2)+putere(a.r.z, 2));
}

void update_size(BODY *corp, int nr_total)
{
    int i;
    for(i=0; i<nr_total; i++)
        (corp+i)->raza = CT_RAZA/dist_OB(*(corp+i));
}

