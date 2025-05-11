#include "bodies.h"

float h;

void read_init_value(BODY *bod, int nr_corp)
{
    float y; 
    printf("m_%d: ", nr_corp + 1);
    scanf("%f", &bod->mass);
    printf("x_0_%d: ", nr_corp + 1);
    scanf("%f", &bod->r.x);
    printf("y_0_%d: ", nr_corp + 1);
    scanf("%f", &y);
    bod->r.y = y; //  * (-1); 
    printf("z_0_%d: ", nr_corp + 1);
    scanf("%f", &bod->r.z);
}

void read_init_velocity(BODY *bod, int nr_corp)
{
    printf("vx_0_%d: ", nr_corp + 1);
    scanf("%f", &bod->v.x);
    printf("vy_0_%d: ", nr_corp + 1);
    scanf("%f", &bod->v.y);
    printf("vz_0_%d: ", nr_corp + 1);
    scanf("%f", &bod->v.z);
}

float putere(float x, int a)
{
    if(a==0) return 1;
    return x*putere(x, a-1);
}

float dist32(BODY a, BODY b, float pas, VECT k_a, VECT k_b) //face prod scalar ^3 cu defazajul k
{
    float dx = (a.r.x + pas*k_a.x) - (b.r.x + pas*k_b.x);
    float dy = (a.r.y + pas*k_a.y) - (b.r.y + pas*k_b.y);
    float dz = (a.r.z + pas*k_a.z) - (b.r.z + pas*k_b.z);
    
    float dist2 = dx*dx + dy*dy + dz*dz;
    if (dist2 < 1e-10) return 1e-10; 
    
    return sqrt(pow(dist2, 1.5));
}

void init_acc(BODY *corp, int nr_total)
{
    int i;
    for(i=0; i<nr_total; i++)
    {
        (corp+i)->a.x = 0;
        (corp+i)->a.y = 0;
        (corp+i)->a.z = 0;
    }
}

void rez_acc_vit(BODY *corp, int nr_total, float h, K_rk4 *k)
{
    int i, j;

    // Inițializare accelerație
    init_acc(corp, nr_total);

    for(i = 0; i < nr_total; i++) {    
        for(j = 0; j < nr_total; j++) {
            if(i != j) {
                float dist;
                float dx, dy, dz;
                
                if (k != NULL) {
                    // Verificăm dacă k este valid înainte de a-l folosi
                    if (i >= nr_total || j >= nr_total) {
                        fprintf(stderr, "Index out of bounds (i: %d, j: %d)\n", i, j);
                        return;
                    }

                    dist = dist32(*(corp + i), *(corp + j), h, (k + i)->v, (k + j)->v);
                    dx = ((corp + j)->r.x + h * (k + j)->v.x) - ((corp + i)->r.x + h * (k + i)->v.x);
                    dy = ((corp + j)->r.y + h * (k + j)->v.y) - ((corp + i)->r.y + h * (k + i)->v.y);
                    dz = ((corp + j)->r.z + h * (k + j)->v.z) - ((corp + i)->r.z + h * (k + i)->v.z);
                } else {
                    dist = dist32(*(corp + i), *(corp + j), 0, (VECT){0, 0, 0}, (VECT){0, 0, 0});
                    dx = (corp + j)->r.x - (corp + i)->r.x;
                    dy = (corp + j)->r.y - (corp + i)->r.y;
                    dz = (corp + j)->r.z - (corp + i)->r.z;
                }

                // Calculăm accelerațiile
                (corp + i)->a.x += ((corp + j)->mass * dx) / dist;
                (corp + i)->a.y += ((corp + j)->mass * dy) / dist;
                (corp + i)->a.z += ((corp + j)->mass * dz) / dist;
            }
        }
        
        // Aplicați accelerația gravitațională
        (corp + i)->a.x *= G;
        (corp + i)->a.y *= G;
        (corp + i)->a.z *= G;

        // Actualizarea vitezelor
        if (k != NULL) {
            ((corp) + i)->v.x += h * ((k + i)->a.x);
            ((corp) + i)->v.y += h * ((k + i)->a.y);
            ((corp) + i)->v.z += h * ((k + i)->a.z);
        }
    }
}


void get_k(BODY *corp, int nr_total, K_rk4 **k)
{
    int i;
    (*k) = (K_rk4*)malloc(sizeof(K_rk4)*nr_total);
    if (!(*k)) {
        printf("Eroare la alocarea memoriei pentru k!\n");
        exit(1);
    }
    
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
    (*corp_cpy) = (BODY*)malloc(sizeof(BODY)*nr_total);
    if (!(*corp_cpy)) {
        printf("Eroare la alocarea memoriei pentru copie corp!\n");
        exit(1);
    }
    
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
    BODY *corp_cpy;

    // calculating k1
    cpy_corp(corp, nr_total, &corp_cpy);
    rez_acc_vit(corp_cpy, nr_total, 0, NULL);
    get_k(corp_cpy, nr_total, &k1);
    free(corp_cpy);

    // calculating k2
    cpy_corp(corp, nr_total, &corp_cpy);
    rez_acc_vit(corp_cpy, nr_total, h/2, k1);
    get_k(corp_cpy, nr_total, &k2);
    free(corp_cpy);

    // calculating k3
    cpy_corp(corp, nr_total, &corp_cpy);
    rez_acc_vit(corp_cpy, nr_total, h/2, k2);
    get_k(corp_cpy, nr_total, &k3);
    free(corp_cpy);

    // calculating k4
    cpy_corp(corp, nr_total, &corp_cpy);
    rez_acc_vit(corp_cpy, nr_total, h, k3);
    get_k(corp_cpy, nr_total, &k4);
    free(corp_cpy);

    int i;
    for(i=0; i<nr_total; i++)
    {
        // update position
        (corp+i)->r.x += h/6.0f*((k1+i)->v.x + 2.0f*(k2+i)->v.x + 2.0f*(k3+i)->v.x + (k4+i)->v.x);
        (corp+i)->r.y += h/6.0f*((k1+i)->v.y + 2.0f*(k2+i)->v.y + 2.0f*(k3+i)->v.y + (k4+i)->v.y);
        (corp+i)->r.z += h/6.0f*((k1+i)->v.z + 2.0f*(k2+i)->v.z + 2.0f*(k3+i)->v.z + (k4+i)->v.z);

        // update speed
        (corp+i)->v.x += h/6.0f*((k1+i)->a.x + 2.0f*(k2+i)->a.x + 2.0f*(k3+i)->a.x + (k4+i)->a.x);
        (corp+i)->v.y += h/6.0f*((k1+i)->a.y + 2.0f*(k2+i)->a.y + 2.0f*(k3+i)->a.y + (k4+i)->a.y);
        (corp+i)->v.z += h/6.0f*((k1+i)->a.z + 2.0f*(k2+i)->a.z + 2.0f*(k3+i)->a.z + (k4+i)->a.z);
    }

    // Eliberează memoria
    free(k1);
    free(k2);
    free(k3);
    free(k4);
}

float dist_OB(BODY a)
{
    return sqrt(putere(a.r.x, 2) + putere(a.r.y, 2) + putere(a.r.z, 2));
}

void update_size(BODY *corp, int nr_total)
{
    int i;
    for(i=0; i<nr_total; i++) 
    {
        float dist = dist_OB(*(corp+i));
        if (dist < 1.0f) dist = 1.0f;  // avoid dividing by 0
        (corp+i)->raza = CT_RAZA/dist;
    }
}

void write_body_state(BODY *corp, float time)
{
    fprintf(corp->file, "%.2f %.2f\n", corp->r.x, corp->r.y);
}

float distanta(BODY a, BODY b)
{
    return sqrtf(powf(a.r.x-b.r.x, 2)+powf(a.r.y-b.r.y, 2)+powf(a.r.z-b.r.z,2));
}

void collision(BODY *corp1, BODY *corp2)
{
    VECT newVelocity;
    float denominator = corp1 -> mass + corp2 -> mass;

    float fraction1 = 2 * corp1 -> mass / denominator;
    float fraction2 = 2 * corp2 -> mass / denominator; 

    newVelocity.x = fraction1 * corp1->v.x + fraction2 * corp2->v.x;
    newVelocity.y = fraction1 * corp1->v.y + fraction2 * corp2->v.y;
    newVelocity.z = fraction1 * corp1->v.z + fraction2 * corp2->v.z; 

    VECT newVelocity1, newVelocity2; 
    corp1->v.x = newVelocity.x - corp1->v.x; 
    corp1->v.y = newVelocity.y - corp1->v.y; 
    corp1->v.z = newVelocity.z - corp1->v.z;
    
    corp2->v.x = newVelocity.x - corp2->v.x; 
    corp2->v.y = newVelocity.y - corp2->v.y; 
    corp2->v.z = newVelocity.z - corp2->v.z;        
}

void check4collision(BODY *corp, int nr_total)
{
    int i, j;
    for(i=0; i<nr_total-1; i++)
    {
        for(j=i+1; j<nr_total; j++)
        {
            if(distanta(*(corp+i), *(corp+j))<(corp+i)->raza+(corp+j)->raza)
            {
                collision(corp+i, corp+j);
            }
        }
    }
}
