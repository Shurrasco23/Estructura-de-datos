#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])

{   
	//el programa solo aceptara un maximo de 2 parametros 
    if (argc > 4) {
        printf("Usage %s: <arg1><arg2> \n", argv[0]);
        return 1;
    }
    
    // se utiliza atoi para cambiar los parametros a enteros antes se debe incluir stdlib

    int num = atoi(argv[1]);
    int numB = atoi(argv[2]);
    int numC = atoi(argv[3]);

    int suma = num + numB + numC;
    int resta = num - numB + numC;
    int multiplicacion = num * numB * numC;
    
    printf("suma: %d\n", suma);
    printf("resta: %d\n", resta);
    printf("multiplicacion %d\n", multiplicacion);
	
    //division
	
    	
    	if (numB == 0 || numC == 0)
		printf ("ERROR: no se puede dividir por 0"); //si el segundo o tercer parametro es 0 imprime ERROR
						     
	
    	else {
		float division= num / numB / numC;
		printf ("division: %5.f\n", division);
	}

    return 0;
}
