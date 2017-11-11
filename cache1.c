//
//cache1.c

//Tarea programada II
//Estructuras de computadoras II
//Carolina Obando Gonzalez
//Willy Villalobos Marrero

//headers...
//Header de la biblioteca estandar de C
//para operaciones de entrada y salida
#include <stdio.h>

//Header de la biblioteca estandar de C
//para gestion de memoria dinamica,
//control de procesos y otras
#include <stdlib.h>


//Estructuras definidas para contener los datos la cache y la memoria

//Contenedor de datos
struct datos
{

    char dato[1];
    char dir[1];
    int mo;
    int ex;
    int sh;
    int iv;
    int ow;
    int dirty;
    int L;
    int S;
    
};

struct datos *datos;

int main()
{

    //Reservando memoria para los datos de la estructura
    datos = (struct datos *)malloc(sizeof(struct datos));

    //Definiendo 
    struct cache_datos dato0,dato1,dato2,dato3;

    //Leer datos del archivo con direcciones y datos


    //Funciones para la comunicacion
    

   //Liberando memoria
   free(datos);

}
