#include "bodies.h"
#include <string.h>

int main()
{
    int bodies, i, stepsNumber;
    float simulationStep, currentTime = 0.0f; 
    char filename[20]; 

    BODY *body;
    FILE *fileInfo = fopen("fileInfo.txt", "wt"); 
    if (fileInfo == NULL)
    {
        printf("Error while trying to open file named fileInfo.txt!\n");
        exit(1); 
    }

    printf("Introduceti numarul de corpuri: "); 
    scanf("%d", &bodies);
    fprintf(fileInfo, "%d\n", bodies); 

    printf("Introduceti pasul de simulare (in secunde): ");
    scanf("%f", &simulationStep);
    
    printf("Introduceti numarul de pasi de simulare: ");
    scanf("%d", &stepsNumber);

    body = (BODY*)malloc(bodies*sizeof(BODY));
    if (!body) {
        printf("Eroare la alocarea memoriei pentru corpuri!\n");
        return 1;
    }

    // reading initial data for each body
    for (i = 0; i < bodies; i++)
    {
        printf("\nCorpul %d:\n", i + 1);
        
        // opening a file for each body
        sprintf(filename, "corp_%d.txt", i + 1);
        (body + i)->file = fopen(filename, "w");
        fprintf(fileInfo, "%s\n", filename); 

        if (!(body + i)->file) 
        {
            printf("Eroare la deschiderea fisierului %s!\n", filename);
            // closing each file to avoid data conflicts
            for (int j = 0; j < i; j++) 
            {
                fclose((body + j)->file);
            }
            
            free(body);
            return 1;
        }
        
        // read initial values
        read_init_value((body + i), i);
        read_init_velocity((body + i), i);
        
        // acceleration initialization with 0
        (body + i)->a.x = 0.0f;
        (body + i)->a.y = 0.0f;
        (body + i)->a.z = 0.0f;
    }

    // update initial radius
    update_size(body, bodies);

    // display initial state
    printf("\nConfiguratia initiala:\n");
    for (i = 0; i < bodies; i++) {
        printf("Corpul %d: pozitie=(%f, %f, %f), viteza=(%f, %f, %f)\n", 
               i + 1, 
               (body + i)->r.x, (body + i)->r.y, (body + i)->r.z,
               (body + i)->v.x, (body + i)->v.y, (body + i)->v.z);
        
        // write initial state in the file
        write_body_state((body + i), currentTime);
    }

    // proceed to the simulation
    for (int step = 1; step <= stepsNumber; step++) {
        
        // time update
        currentTime += simulationStep;
        
        // apply a step of rk4
        rk4(body, bodies, simulationStep);
        
        // update radius
        update_size(body, bodies);
        
        // write the current state in the file
        for (i = 0; i < bodies; i++) {
            write_body_state((body + i), currentTime);
        }
    }

    printf("\nConfiguratia finala:\n");
    for (i = 0; i < bodies; i++) {
        printf("Corpul %d: pozitie=(%f, %f, %f), viteza=(%f, %f, %f)\n", 
               i + 1, 
               (body + i)->r.x, (body + i)->r.y, (body + i)->r.z,
               (body + i)->v.x, (body + i)->v.y, (body + i)->v.z);
        
        fclose((body + i)->file);
    }

    free(body);    
    return 0;
}