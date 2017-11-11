//
//cache.c

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

//Contenedor de datos del cpu 0
struct cpu_0
{

    char a0;
    char a1;
    char a2;
    char a3;

};

//Contenedor de datos del cpu 1
struct cpu_1
{

    char a0;
    char a1;
    char a2;
    char a3;

};


//Contenedor de datos del cpu 2
struct cpu_2
{

    char a0;
    char a1;
    char a2;
    char a3;

};

//Contenedor de datos del cpu 3
struct cpu_3
{

    char a0;
    char a1;
    char a2;
    char a3;

};

//Contenedor de memoria cache L1
struct memory_cache_L1
{

    char a0;
    char a1;
    char a2;
    char a3;

};

//Contenedor de memoria cache L2
struct memory_cache_L2
{

    char a0;
    char a1;
    char a2;
    char a3;

};

struct cpu_0 *datos_cpu0;

struct cpu_1 *datos_cpu1;

struct cpu_2 *datos_cpu2;

struct cpu_3 *datos_cpu3;

struct memory_cache_L1 *datos_cache_L1;

struct memory_cache_L2 *datos_cache_L2;

int main()
{

    //Reservando memoria para los datos de las estructuras, CPU #
    datos_cpu0 = (struct cpu_0 *)malloc(sizeof(struct cpu_0));
    datos_cpu1 = (struct cpu_1 *)malloc(sizeof(struct cpu_1));
    datos_cpu2 = (struct cpu_2 *)malloc(sizeof(struct cpu_2));
    datos_cpu3 = (struct cpu_3 *)malloc(sizeof(struct cpu_3));

    //Reservando memoria para los datos de las estructuras, memory cache L#
    datos_cache_L1 = (struct memory_cache_L1 *)malloc(sizeof(struct memory_cache_L1));
    datos_cache_L2 = (struct memory_cache_L2 *)malloc(sizeof(struct memory_cache_L2));

    //Funcion para la comunicacion
    

   //Liberando memoria
   free(datos_cpu0);
   free(datos_cpu1);
   free(datos_cpu2);
   free(datos_cpu3);
   free(datos_cache_L1);
   free(datos_cache_L2);

}
