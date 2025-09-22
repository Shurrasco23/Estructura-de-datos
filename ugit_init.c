#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_COMMITS 100
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
        cargar_commits_desde_archivo();
    }
    if (argc < 2) {
        printf("Uso: ugit <comando>\n");
        return 1;
    }
    if (argc < 2) {
        printf("Uso: ugit <comando>\n");
        return 1;
    }
    if (strcmp(argv[1], "init") == 0) {
        ugit_init();
    }
    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3)
            printf("Uso: ugit add <archivo>\n");
        else
            ugit_add(argv[2]);
    }
    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 4 || strcmp(argv[2], "-m") != 0)
            printf("Uso: ugit commit -m \"mensaje\"\n");
        else
            ugit_commit(argv[3]);
    }
    else if (strcmp(argv[1], "log") == 0) {
        ugit_log();
    }
    else if (strcmp(argv[1], "checkout") == 0) {
        if (argc < 3)
            printf("Uso: ugit checkout <id>\n");
        else
            ugit_checkout(argv[2]);
    }
    else {
        printf("Comando no reconocido: %s\n", argv[1]);
    }

    return 0;
}
// ---------------- FUNCIONES BÁSICAS ----------------
int repo_existe() {
    FILE *f = fopen(".ugit/commits.txt", "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

int crear_carpeta_repo() {
    return system("mkdir -p .ugit");
}

void crear_archivos_control() {
    FILE *f;
    f = fopen(".ugit/commits.txt", "w"); if (f) fclose(f);
    f = fopen(".ugit/staging_area.txt", "w"); if (f) fclose(f);
}

void ugit_init() {
    if (repo_existe()) {
        printf("Ya existe un repositorio uGit en este directorio\n");
        return;
    }
    if (crear_carpeta_repo() != 0) {
        printf("Error al crear la carpeta del repositorio\n");
        return;
    }
    crear_archivos_control();
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
void generar_hash(char *dest, size_t len) {
    const char *chars = "abcdef0123456789";
    srand(time(NULL) ^ rand());
    for (size_t i = 0; i < len - 1; i++)
        dest[i] = chars[rand() % 16];
    dest[len - 1] = '\0';
}
// ---------------- ADD ----------------
void ugit_add(const char *archivo_commit) {
    FILE *staging = fopen(".ugit/staging_area.txt", "a");
    if (!staging) {
        printf("ERROR: no se pudo abrir el staging area\n");
        return;
    }
    fprintf(staging, "%s\n", archivo_commit);
    fclose(staging);
    printf("Archivo '%s' agregado al staging\n", archivo_commit);
}
// ---------------- COMMIT ----------------
void ugit_commit(const char *mensaje) {
    if (total_commits >= MAX_COMMITS) {
        printf("Error: máximo de commits alcanzado.\n");
        return;
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
               (i == total_commits - 1) ? " (HEAD -> master)" : "");
        printf("Author: Tu Nombre <tu.email@example.com>\n");
        printf("Date: %s\n", commits[i].fecha);
        printf("%s\n\n", commits[i].mensaje);
    }
}
// ---------------- CHECKOUT ----------------
void ugit_checkout(const char *commit_id) {
    FILE *commits_file = fopen(".ugit/commits.txt", "r");
    if (!commits_file) {
        printf("Error al abrir commits.txt.\n");
        return;
    }
    char linea[256];
    bool mostrando = false, encontrado = false;
    while (fgets(linea, sizeof(linea), commits_file)) {
        if (strncmp(linea, "commit", 6) == 0) {
            char id[HASH_LEN];
            if (sscanf(linea, "commit %8s", id) == 1) {
                if (strcmp(id, commit_id) == 0) {
                    mostrando = true;
                    encontrado = true;
                    printf("%s", linea);
                    continue;
                } else {
                    mostrando = false;
                }
            }
        }
        if (mostrando) {
            if (strncmp(linea, "---", 3) == 0) break;
            printf("%s", linea);
        }
    }
    if (!encontrado)
        printf("No se encontró un commit con id %s\n", commit_id);
    fclose(commits_file);
}
