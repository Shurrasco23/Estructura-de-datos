#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

void ugit_add(const char *archivo_commit){

        FILE *staging = fopen(".ugit/staging_area.txt", "a");

        if (staging == NULL){

                printf ("ERROR, no se pudo abrir el staging area");
                return;
        } //parte del codigo que abre la ruta del archivo en el cual se a;aden los commits

        fprintf (staging, "%s\n", archivo_commit);

        fclose(staging);

        printf ("archivo %s\n agregado al staging", archivo_commit);

        }

void ugit_checkout(int commit_id){

        FILE *commits = fopen(".ugit/commits.txt", "r");

        if (commits == NULL){

                printf ("Error al abrir commits.txt, asegurese de haber creado un commit");

                return;
        }

        char linea[256];
        int id_actual; //variable que recorre las id de los commits, hasta llegar al id del deseado
        bool id_encontrada = false; //variable booleana que indica si se encontro el commit por su id, inicialmente en false

        while (fgets(linea, sizeof(linea), commits)){ //

                if(strncmp(linea, "commit", 6)){

                        id_actual = id_actual + 1;

                }

                if(id_actual == commit_id){

                        id_encontrada = true;

                        printf ("commit %d", commit_id);
                }

                if(id_encontrada){
        
           if(strncmp (linea, "----", 4) == 0){
                                break;
                        }

                        printf ("%s", linea);
                }


        }

        if (!id_encontrada){

                printf ("no se ha encontrado un commit con esa id");

        }

        fclose(commits);
}

// Despues sigo comentando el codigo
