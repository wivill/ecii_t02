# Tarea programada 02 Estructuras de Computadoras Digitales II
# Estudiantes:
# Carolina Obando González
# Willy Gerardo Villalobos Marrero
#
# Profesora: Katherine Nuñez
#
# Pequeño Makefile básico para la compilación del código y la limpieza
# del entorno.

src=mesi
CXXFLAGS="-Wall -Wpedantic -std=c++14"
all:
	g++ ${CXXFLAGS} ${src}.cpp -o ${src}.o

clean:
	rm -rf ${src}.o

test: all
	qsub ${src}.pbs
