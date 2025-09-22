//bibliotecas utilizadas para la creacion del programa ugit

#include <stdio.h> //Para que funcionen algunos comandos basicos como printf, scanf, if, etc...
#include <stdlib.h>
#include <string.h>//biblioteca la cual permite comparar cadenas de caracteres
#include <time.h>
#include <stdbool.h> //biblioteca que nos permite trabajar con variables booleanas

#define MAX_COMMITS 100 //Numero maximo de commits que se pueden crear
#define HASH_LEN 9   // 8 chars + '\0'
/**
*Diego: Init, log
*Catalina: commit
*Alejandro:Add,checkout
*/

// Estructura para commits
typedef struct {
    char id[HASH_LEN];       // hash corto tipo "abcd1234"
    char fecha[64];          // fecha formateada
    char mensaje[256];       // mensaje del commit
    char archivos[20][256];  // archivos incluidos
    int total_archivos;
} Commit;

Commit commits[MAX_COMMITS];
int total_commits = 0;

// Prototipos
int repo_existe();
int crear_carpeta_repo();
void crear_archivos_control();
void ugit_init();

FILE *abrir_staging_para_lectura(void);
void vaciar_staging(void);

void generar_hash(char *dest, size_t len);
void ugit_add(const char *archivo_commit);
void ugit_commit(const char *mensaje);
void cargar_commits_desde_archivo(void); 
void ugit_log(void);
void ugit_checkout(const char *commit_id);

// ---------------- MAIN ----------------
int main(int argc, char *argv[])
{
    if (repo_existe()) {
        cargar_commits_desde_archivo(); //si existe el repositorio se ejecuta la funcion cargar_commits_desde_archivo
    }
    if (argc < 2) {
        printf("Uso: ugit <comando>\n");
        return 1;
    }
    if (argc < 2) {
        printf("Uso: ugit <comando>\n");
        return 1;
    } // posibles errores, si el conteo de argumentos detecta menos de 2 ingresados, se le indica al usuario como correr el programa correctamente

    if (strcmp(argv[1], "init") == 0) {
        ugit_init();
    }
    else if (strcmp(argv[1], "add") == 0) { // sentencia if que compara a modo de string lo que el usurio ingreso, con *add*, necesario incluir la bibloteca string.h,
					    // si las cadenas son iguales, devuelve 0
        if (argc < 3)				
            printf("Uso: ugit add <archivo>\n");
        else
            ugit_add(argv[2]); 
    }
    
    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 4 || strcmp(argv[2], "-m") != 0) // sentencia que comprueba que hayan al menos 4 argumentos ingresados, o que el segundo no sea -m 
            printf("Uso: ugit commit -m \"mensaje\"\n"); // si una de las condiciones no se cumple se le muestra al usuario como ejecutar correctamente ugit commit 
        else
            ugit_commit(argv[3]); 
    }
    else if (strcmp(argv[1], "log") == 0) { //El primer argumento es log
        ugit_log(); // se ejecuta la funcion ugit_log()
    }
    else if (strcmp(argv[1], "checkout") == 0) { //El primer argumento es checkout
        if (argc < 3)
            printf("Uso: ugit checkout <id>\n");
        else
            ugit_checkout(argv[2]); //se ejecuta la funcion ugit_checkout
    }
    else {
        printf("Comando no reconocido: %s\n", argv[1]);
    }

    return 0;
}
// ---------------- FUNCIONES BÁSICAS ----------------
int repo_existe() {
    
    FILE *f = fopen(".ugit/commits.txt", "r"); //se abre el archivo, solo en modo lectura
    if (f) {
        fclose(f);
        return 1; // si se devuelve algo distinto de 0 o NULL el archivo existe y lo cerramos
    }
    return 0; //el archivo no existe
}

int crear_carpeta_repo() {
    return system("mkdir -p .ugit"); //return 0
}

void crear_archivos_control() {
    FILE *f;
    f = fopen(".ugit/commits.txt", "w"); if (f) fclose(f); 
    f = fopen(".ugit/staging_area.txt", "w"); if (f) fclose(f);
    //funcion que crea o trunca los archivos *commits.txt o stanging_area.txt*
}

void ugit_init() {
    if (repo_existe()) { //si existe un repositorio en el directorio, no se se crea y se termina la funcion
        printf("Ya existe un repositorio uGit en este directorio\n");
        return;
    }
    if (crear_carpeta_repo() != 0) { //si la funcion crear_carpeta_repo devolvio un valor distinto de 0, quiere decir que hubo un error al crear la carpeta
        printf("Error al crear la carpeta del repositorio\n");
        return;
    }
    crear_archivos_control(); //si todo sale bien se llama a la funcion crear_archivos_control, la cual se asegura de entregar los archivos vacios 
    printf("Inicializado repositorio uGit vacío\n");
}

FILE *abrir_staging_para_lectura(void) {
    return fopen(".ugit/staging_area.txt", "r");
}

void vaciar_staging(void) {
    FILE *clear = fopen(".ugit/staging_area.txt", "w");
    if (clear) fclose(clear);
}
// ---------------- HASH ----------------
void generar_hash(char *dest, size_t len) { // len = longitud maxima de caracteres, dest = buffer donde se guarda el hash
    const char *chars = "abcdef0123456789"; 
    srand(time(NULL) ^ rand());
    for (size_t i = 0; i < len - 1; i++) //ciclo for con i incializado en 0. al ser size_t solo contempla numeros positivos
        dest[i] = chars[rand() % 16]; //arreglo que va conformando el id del commit, desde con caracteres de 0-f (hexadecimal)
    dest[len - 1] = '\0'; 
}
// ---------------- ADD ----------------
void ugit_add(const char *archivo_commit) {
    
    FILE *staging = fopen(".ugit/staging_area.txt", "a"); //puntero que abre el archivo stanging_area.txt en modo de escritura
    
    if (!staging) { //sentencia que comprueba que el archivo no se pudo abrir
        printf("ERROR: no se pudo abrir el staging area\n");
        return;
    }
    
    fprintf(staging, "%s\n", archivo_commit); //sentencia que imprime el nombre del archivo en la seccion staging
    fclose(staging); //se cierra el archivo staging.txt para guardar cambios
    printf("Archivo '%s' agregado al staging\n", archivo_commit); //Se le indica al usuario que el archivo se ha guardado en el area staging

}
// ---------------- COMMIT ----------------
void ugit_commit(const char *mensaje) {
    if (total_commits >= MAX_COMMITS) {
        printf("Error: máximo de commits alcanzado.\n");
        return; // Se han creado mas de 100 commits (MAX_COMMITS)
    }
    FILE *staging = abrir_staging_para_lectura();
    if (!staging) {
        printf("No existe staging area. Ejecuta ugit init.\n");
        return;
    }
    // Verificar si staging está vacío
    fseek(staging, 0, SEEK_END);
    if (ftell(staging) == 0) {
        printf("No hay archivos en el staging area.\n");
        fclose(staging);
        return;
    }
    rewind(staging);
    Commit nuevo;
    generar_hash(nuevo.id, HASH_LEN);
    // Fecha
    time_t now = time(NULL);
    strftime(nuevo.fecha, sizeof(nuevo.fecha), "%Y-%m-%d %H:%M:%S", localtime(&now));
    strncpy(nuevo.mensaje, mensaje, sizeof(nuevo.mensaje));
    // Archivos del staging
    nuevo.total_archivos = 0;
    while (fgets(nuevo.archivos[nuevo.total_archivos], sizeof(nuevo.archivos[0]), staging)) {
        nuevo.archivos[nuevo.total_archivos][strcspn(nuevo.archivos[nuevo.total_archivos], "\n")] = '\0';
        nuevo.total_archivos++;
    }
    fclose(staging);
    // Guardar en memoria
    commits[total_commits++] = nuevo;
    // Guardar en disco
    FILE *f = fopen(".ugit/commits.txt", "a");
    if (f) {
        fprintf(f, "commit %s\n", nuevo.id);
        fprintf(f, "Date: %s\n", nuevo.fecha);
        fprintf(f, "Message: %s\n", nuevo.mensaje);
        fprintf(f, "Files:\n");
        for (int i = 0; i < nuevo.total_archivos; i++)
            fprintf(f, "%s\n", nuevo.archivos[i]);
        fprintf(f, "---\n");
        fclose(f);
    }
    vaciar_staging();
    printf("[master] commit %s - %s\n", nuevo.id, mensaje);
}

void cargar_commits_desde_archivo(void) {
    
    FILE *f = fopen(".ugit/commits.txt", "r");
    if (!f) return;

    char linea[256];
    Commit temp;
    memset(&temp, 0, sizeof(Commit));
    while (fgets(linea, sizeof(linea), f)) {
        if (strncmp(linea, "commit", 6) == 0) {
            if (temp.total_archivos > 0 || strlen(temp.id) > 0) {
                // Guardar el commit anterior antes de reiniciar
                if (total_commits < MAX_COMMITS)
                    commits[total_commits++] = temp;
                memset(&temp, 0, sizeof(Commit));
            }
            sscanf(linea, "commit %8s", temp.id);
        }
        else if (strncmp(linea, "Date:", 5) == 0) {
            sscanf(linea, "Date: %[^\n]", temp.fecha);
        }
        else if (strncmp(linea, "Message:", 8) == 0) {
            sscanf(linea, "Message: %[^\n]", temp.mensaje);
        }
        else if (strncmp(linea, "Files:", 6) == 0) {
            // No hacer nada, los archivos vienen en las líneas siguientes
        }
        else if (strncmp(linea, "---", 3) == 0) {
            if (total_commits < MAX_COMMITS)
                commits[total_commits++] = temp;
            memset(&temp, 0, sizeof(Commit));
        }
        else if (strlen(linea) > 1) {
            // Guardar archivos
            linea[strcspn(linea, "\n")] = '\0'; // quitar \n
            strncpy(temp.archivos[temp.total_archivos], linea, 255);
            temp.total_archivos++;
        }
    }
    // Guardar el último commit si quedó cargado
    if (temp.total_archivos > 0 || strlen(temp.id) > 0) {
        if (total_commits < MAX_COMMITS)
            commits[total_commits++] = temp;
    }
    fclose(f);
}
// ---------------- LOG ----------------
void ugit_log(void) {
    if (total_commits == 0) {
        printf("No hay commits.\n");
        return; 
    }
    for (int i = total_commits - 1; i >= 0; i--) {
        printf("commit %s%s\n", commits[i].id,
               (i == total_commits - 1) ? "(HEAD -> master)" : ""); // Condicion que comprueba si se itero al commit mas reciente, de ser cierto imprime head master, de lo contrario nada
        printf("Author: Tu Nombre <tu.email@example.com>\n");
        printf("Date: %s\n", commits[i].fecha);
        printf("%s\n\n", commits[i].mensaje);
    } //Ciclo que itera sobre los commits creados en orden inverso, mostrando id, autor, fecha, y mensaje del commit iterado
}
// ---------------- CHECKOUT ----------------
void ugit_checkout(const char *commit_id) {
    
    FILE *commits_file = fopen(".ugit/commits.txt", "r"); //Puntero que abre el archivo commits.txt en modo lectura.
    if (!commits_file) {
        printf("Error al abrir commits.txt.\n");
        return; //Hubo un error al querer abrir commits.txt
    }
    char linea[256];
    
    bool mostrando = false, encontrado = false; //variables booleanas inicializadas en FALSE
    
    while (fgets(linea, sizeof(linea), commits_file)) { //ciclo que recorre el archivo commits.txt en busqueda del commit deseado
        if (strncmp(linea, "commit", 6) == 0) {
            char id[HASH_LEN];
            if (sscanf(linea, "commit %8s", id) == 1) {
                if (strcmp(id, commit_id) == 0) { //se ha encontrado el commit
                    mostrando = true; 
                    encontrado = true; //las variables bool pasan a TRUE
                    printf("%s", linea);
                    continue;
                } else {
                    mostrando = false; //si la id almacenada no coincide con commit_id seguira en estado FALSE
                }
            }
        }
        if (mostrando) { //si la variable esta activada (TRUE), entonces se imprime el commit hasta las lineas * --- *.
            if (strncmp(linea, "---", 3) == 0) break;
            printf("%s", linea);
        }
    }
    if (!encontrado)
        printf("No se encontró un commit con id %s\n", commit_id); //No se ha encontrado el commit
    fclose(commits_file); // se cierra el archivo commits.txt
}
