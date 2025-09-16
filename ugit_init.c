#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int repo_existe();
int crear_carpeta_repo();
void crear_archivos_control();
void ugit_init();

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