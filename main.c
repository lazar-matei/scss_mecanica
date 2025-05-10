#include "bodies.h"

int main()
{
    int n, i;
    char fisier[10] = "out"; //3
    BODY *corp;

    printf("Nr. corpuri: ");
    scanf("%d", &n);
    corp = (BODY*)malloc(n*sizeof(BODY));
    for(i=0; i<n; i++)
    {
        fisier[3]=i;
        read_init_value((corp+i), i);
        (corp+i)->file = fopen(fisier, "wb");
    }

    printf("heil heil heil"); 
    return 0;
}