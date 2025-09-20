#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/****DIEGO******/
int repo_existe();
int crear_carpeta_repo();
void crear_archivos_control();
void ugit_init();
void ugit_commit(const char *mensaje);

int main(int argc, char *argv[]) 
{
    // Indica al usuario como usar el programa si no se utilizan argumentos al ejecutarlo
    if (argc < 2) 
    {
        printf("Uso: ugit <comando>\n");
        return 1;
    }

    // Si el argumento es init, el repositorio se inicializa
    if (strcmp(argv[1], "init") == 0) 
    {
        ugit_init();
    } 

    //commit:uso:ugit commit -m "mensaje"
    else if (strcmp(argv[1], "commit") == 0){
        if(argc < 4 || strcmp(argv[2], "-m") != 0){
            printf("ugit commit");
        }else{
            ugit_commit(argv[3]);
        }
    }

    // Si se escribe un argumento que no se reconoce, se le indica al usuario
    else 
    {
        printf("Comando no reconocido: %s\n", argv[1]);
    }

    return 0;
}


// Verificar si ya existe un repositorio
int repo_existe() 
{
    FILE *f = fopen(".ugit/commits.txt", "r");

    if (f != NULL) 
    {
        fclose(f);
        return 1; // Existe
    }

    return 0; // No existe
}


// Crear la carpeta base .ugit
int crear_carpeta_repo() 
{
    return system("mkdir .ugit");
}


// Crear archivos de control dentro de .ugit
void crear_archivos_control() 
{
    FILE *f;

    f = fopen(".ugit/commits.txt", "w");

    if (f != NULL) 
    {
        fclose(f);
    }

    f = fopen(".ugit/staging_area.txt", "w");

    if (f != NULL) 
    {
        fclose(f);
    }
}


// Inicializar repositorio
void ugit_init() 
{
    if (repo_existe()) 
    {
        printf("Ya existe un repositorio uGit en este directorio\n");
        return;
    }

    if (crear_carpeta_repo() != 0) 
    {
        printf("Error al crear la carpeta del repositorio\n");
        return;
    }

    crear_archivos_control();
    printf("Inicializado repositorio uGit vacío\n");
}
/***DIEGO****/
 /*******CATALINA*******/
 void ugit_commit(const char *mensaje){
    FILE *staging = fopen(".ugit/staging_area.txt", "r");
    if (staging == NULL){
        printf("No existe un staging. Ejecuta ugit init.");
        return;
    }

    fseek(staging, 0, SEEK_END);
    long size = ftell(staging);
    if (size == 0){
        printf("No hay archivos en el staging area para commitear.\n");
        fclose(staging);
        return;
    }
    rewind(staging);
    //Abrir commits.txt para agregar el nuevo commit
    FILE *commits = fopen(".ugit/commits.txt", "a");
    if (commits == NULL){
        printf("Error al abrir commits.txt\n");
        fclose(staging);
        return;
    }
    //contar commits previos para asignar ID
    FILE *commits_read = fopen(".ugit/commits.txt", "r");
    int commit_id = 1;
    if (commits_read){
        char linea[256];
        while(fgets(linea,sizeof(linea),commits_read)){
            if(strncmp(linea,"commit", 7) == 0){
                commit_id++;
            }
        }
        fclose(commits_read);
    }
    //fecha y hora
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char fecha[64];
    strftime(fecha,sizeof(fecha), "%Y-%m-%d %H:%M:%S", t);
    //encabezado del commit
    fprintf(commits, "commit %d\n", commit_id);
    fprintf(commits, "Date: %s\n", fecha);
    fprintf(commits, "Message: %s\n", mensaje);
    fprintf(commits, "Files:\n");
    //Escribe cada archivo que estaba en staging 
    char archivo[256];
    while(fgets(archivo,sizeof(archivo),staging)){
        //Eliminar salto de linea final
        archivo[strcspn(archivo,"\n")] = 0;
        if(strlen(archivo) == 0) continue; //salta lineas vacias
        fprintf(commits,"%s\n", archivo);

    }
    //separador para facilitar lectura
    fprintf(commits, "---\n");

    fclose(staging);
    fclose(commits);

    //vaciar staging_area.txt
    FILE *clear = fopen(".ugit/staging_area.txt", "w");
    if(clear) fclose(clear);
    printf("[master] commit %d - %s\n",commit_id,mensaje);

 }