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

//Se incluyó debido a que se usa
//la funcion strcpy
#include <string.h>

//Header de la biblioteca estandar de C
//para definir un dato de tipo booleano
#include <stdbool.h>



//Estructuras definidas para contener los datos la cache y la memoria

//Contenedor de datos
struct datos
{

    char dato[1];
    char dir[1];
    int modified;
    int exclusive;
    int share;
    int invalid;
    int owen;
    int dirty;
    int clear;
    bool L;
    bool S;
    unsigned int tag;
    int core;
};

struct datos *datos;

int numeroDeDatosLeidos=0;

void LeerDatos() {

    FILE *archivo;
    char d1[256], d2[256], d3[256], d4[256];
    
    printf("Leyendo datos de memoria\n");

    //Abriendo archivo de texto en modo de lectura
    char nombreDeArchivo[256]="memory-trace-gcc.txt";
    archivo = fopen(nombreDeArchivo, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo: memory-trace-gcc.txt";
        exit(1);
}

    //Leyendo datos de memoria linea por linea
    fscanf(archivo, "%s \n", d1);
    fscanf(archivo, "%s \n", d2);
    fscanf(archivo, "%s \n", d3);
    fscanf(archivo, "%s \n", d4);
    int numeroDeDatosLeidos++;


    printf("  Numero de datos leidos: %d\n", numeroDeDatosLeidos);

    //Cerrando archivo
    fclose(archivo);

}

/*
void leernumerodelineas() {
    int numlineas = 0;
}*/

int main()
{

    //Reservando memoria para los datos de la estructura
    datos = (struct datos *)malloc(sizeof(struct datos));


    //Leer datos del archivo con direcciones y datos


    //Funciones para la comunicacion
    

    //Liberando memoria
   free(datos);

}


