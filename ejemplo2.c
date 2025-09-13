#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <arg1> <arg2>\n", argv[0]);
        return 1;
    }

    char *a = argv[1];
    printf("%s ", a);

    char *b = argv[2];
    printf("%s\n", b);

    if (strcmp(argv[2], "hola") == 0) {
        printf("son iguales\n");
    } else {
        printf("no son iguales\n");
    }
    return 0;
}