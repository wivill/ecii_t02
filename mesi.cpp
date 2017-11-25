/*Se realizan los ncludes necesarios para la correcta manipulación de los
  archivos, entre otras funcionaes básicas.*/

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <sstream>

using namespace std;

/*Algunas definiciones para facilitar un poco el trabajo con los estados
  y niveles de caché.*/

#define L1 1
#define L2 2
#define MOD 0   // M
#define EX 1    // E
#define SH 2    // S
#define INV 3   // I

/*Definición de caché como estructura de datos*/
struct cache {

    int state;
    unsigned int tag;
    int CoreId;

};

/*Obtención de línea específica para indicar cual se debe procesar.*/
std::fstream& GotoLine(std::fstream& file,  int num) {

    file.seekg(std::ios::beg);

    for(int i = 0; i < num; ++i) {
           file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return file;

}

/*Conteo de líneas para usar como paŕametro y distribuir las instrucciones
  entre los cores.*/
int countLines(char* testFile) {

    string lineC;
    int maxLines = 0;

    ifstream memory (testFile);

    while(getline(memory, lineC)) {
        ++maxLines;
    }

    memory.close();
    return maxLines;

}

/*Inicializa los bloques de caché con tamaño e identificador*/
void init_cache(vector<cache>& Cache, int size, int id) {

    for (int i = 0; i < size; i++) {
        Cache.push_back(cache());
        Cache[i].state = INV;
        Cache[i].CoreId = id;
    }

}

/*Convierte index y tag de acuerdo con el nivel de cache y la instrucción */
unsigned int convertIndex(unsigned int Value, int level){

    unsigned int temp, Index;

    if (level == L1) {
        temp = Value << 20;
        Index = temp >> 22;
        return Index;
    }
    else {
        temp = Value << 17;
        Index = temp >> 19;
        return Index;
    }

}

unsigned int convertTag(unsigned int Value, int level) {

    unsigned int Tag;

    if (level == L1) {
        Tag = Value >> 12;
        return Tag;
    }
    else {
        Tag = Value >> 15;
        return Tag;
    }

}

/*Obtención y actualización de estado, así como lectura en cache*/
int getState(unsigned int address, vector<cache> & Core) {

    int stateReturn;
    unsigned int index;
    index = convertIndex(address, L1);
    stateReturn = Core[index].state;
    return stateReturn;

}

void setState(unsigned int address, vector<cache> & Core, int stateNew){

    unsigned int index;
    index = convertIndex(address, L1);
    Core[index].state = stateNew;

}

void write_cache(unsigned int address, vector<cache> & Core, int level) {

    unsigned int index, Tag;

    if (level == L1){
        index = convertIndex(address, L1);
        Tag = convertTag(address, L1);
        Core[index].tag = Tag;
    }
    else {
        index = convertIndex(address, L2);
        Tag = convertTag(address, L2);
        Core[index].tag = Tag;
    }

}

/*Funciones para obtener direcciones, operaciones y establecer estados*/
unsigned int getAddress(std::fstream & address, int lineNumber) {

    string instruction;
    unsigned int value;
    stringstream hexConvert;

    GotoLine(address, lineNumber);
    address >> instruction;
    cout << instruction << endl;
    hexConvert << instruction;
    hexConvert >> std::hex >> value;
    return value;

}

int getOperation(std::fstream & operation) {

    string line;
    getline(operation, line);
    std::size_t found = line.find("L");

    if (found != std::string::npos) {
        return 0;
    }
    else {
        return 1;
    }

}

void readBusRd(unsigned int index, unsigned int tag, unsigned int address,
               int stateA, vector<cache>& Core, vector<cache>& CoreA,
               vector<cache>& Share) {

    if(CoreA[index].tag == tag) {
        switch(stateA) {
            case MOD:
                write_cache(address, Share, L2);
                setState(address, CoreA, SH);
                write_cache(address, Core, L1);
                setState(address, Core, SH);
                break;

            case EX:
                setState(address, CoreA, SH);
                write_cache(address, Core, L1);
                setState(address, Core, SH);
                break;

            case SH:
                write_cache(address, Core, L1);
                setState(address, Core, SH);
                break;

            default:
                break;
        }
    }
}

void invalidCaches(int index, unsigned int tag, unsigned int address,
                   int stateA, vector<cache>& Core, vector<cache>& Share) {

    if(Core[index].tag == tag) {
        setState(address, Core, INV);
    }

    if(stateA == 0) {
        write_cache(address, Share, L2);
    }

}

/*Se realiza la lógica de actualización de estados de caché.*/
void stateCoreModifier(vector<cache>& CoreA, vector<cache>& CoreB,
    vector<cache>& CoreC, vector<cache>& CoreD, unsigned int address,
    int operation, vector<cache>& Share) {

    int stateA, stateB, stateC, stateD;
    unsigned int tag;
    unsigned int index;
    tag = convertTag(address, L1);
    index = convertIndex(address, L1);

    stateA = getState(address, CoreA);
    stateB = getState(address, CoreB);
    stateC = getState(address, CoreC);
    stateD = getState(address, CoreD);

    if (operation == 0) {

        if(CoreA[index].tag != tag) {
            stateA = INV;
        }

        switch (stateA) {
        case MOD:
                break;

            case EX:
                break;

            case SH:
                break;

            case INV:
                if((stateB == INV) && (stateC == INV) && (stateD == INV)) {
                    setState(address, CoreA, EX);
                    write_cache(address, CoreA, L1);
                    write_cache(address, Share, L2);
                }
                else if((CoreB[index].tag != tag) && (CoreC[index].tag != tag)
                    && (CoreD[index].tag != tag)) {

                    setState(address, CoreA, EX);
                    write_cache(address, CoreA, L1);
                    write_cache(address, Share, L2);}

                else {
                    readBusRd(index, tag, address, stateB, CoreA, CoreB, Share);
                    readBusRd(index, tag, address, stateC, CoreA, CoreC, Share);
                    readBusRd(index, tag, address, stateD, CoreA, CoreD, Share);
                }
                break;

            default:
                break;
        }

    }

    else {

        switch(stateA) {
            case MOD:
                write_cache(address, CoreA, L1);
                break;

            case EX:
                setState(address, CoreA, MOD);
                write_cache(address, CoreA, L1);
                break;

            case SH:
                setState(address, CoreA, MOD);
                write_cache(address, CoreA, L1);
                invalidCaches(index, tag, address, stateB, CoreB, Share);
                invalidCaches(index, tag, address, stateC, CoreC, Share);
                invalidCaches(index, tag, address, stateD, CoreD, Share);
                break;

            case INV:
                setState(address, CoreA, MOD);
                write_cache(address, CoreA, L1);
                invalidCaches(index, tag, address, stateB, CoreB, Share);
                invalidCaches(index, tag, address, stateC, CoreC, Share);
                invalidCaches(index, tag, address, stateD, CoreD, Share);
                break;

            default:
                break;
        }

    }

}

/*Actualiza los estados según los valores proporcionados por el core y
  la instrucción ejecutada*/
void Controller(int coreId, vector<cache> & Core1, vector<cache> & Core2,
                vector<cache> & Core3, vector<cache> & Core4,
                vector<cache> & Share, unsigned int address, int operation) {

    switch(coreId) {
        case 1:
            stateCoreModifier(Core1, Core2, Core3, Core4, address, operation,
                              Share);
            break;

        case 2:
            stateCoreModifier(Core1, Core2, Core3, Core4, address, operation,
                              Share);
            break;

        case 3:
            stateCoreModifier(Core1, Core2, Core3, Core4, address, operation,
                              Share);
            break;

        case 4:
            stateCoreModifier(Core1, Core2, Core3, Core4, address, operation,
                              Share);
            break;

        default:
            break;
    }

}

/* Función para imprimir en pantalla o archivo los estados de caché.*/

void instructionPrint(int coreId, vector<cache> & Core1, vector<cache> & Core2,
    vector<cache> & Core3, vector<cache> & Core4, vector<cache> & Share,
    unsigned int addressRaw, int operation, int line, std::fstream & memory,
    int indexTest) {

    cout << "Dirección: " << addressRaw << endl;
    Controller(coreId, Core1, Core2, Core3, Core4, Share, addressRaw,
        operation);
    cout << "Índice: " << indexTest << "   " << "Operación: " << operation <<
        endl;

    cout << "Tag Core 1: " << Core1[indexTest].tag << "\t" << "Estado Core 1: "
        << Core1[indexTest].state << endl;

    cout << "Tag Core 2: " << Core2[indexTest].tag << "\t" << "Estado Core 2: "
        << Core2[indexTest].state << endl;

    cout << "Tag Core 3: " << Core3[indexTest].tag << "\t" << "Estado Core 3: "
        << Core3[indexTest].state << endl;

    cout << "Tag Core 4: " << Core4[indexTest].tag << "\t" << "Estado Core 4: "
        << Core4[indexTest].state << endl;

    cout << "Tag L2: " << Share[indexTest].tag << "\t" << "Estado L2: "
        << Share[indexTest].state << endl;

    cout << "\n" << endl;

}

/*Main*/
int main(int argc, char *argv[]) {

    vector<cache> Core1, Core2, Core3, Core4, Share;
    init_cache(Core1, 1000, 1);
    init_cache(Core2, 1000, 2);
    init_cache(Core3, 1000, 3);
    init_cache(Core4, 1000, 4);
    init_cache(Share, 8000, 0);

    string line;
    int maxCount = 0;
    maxCount = countLines(argv[1]);
    int * Counter1 = new int[maxCount/4];
    int * Counter2 = new int[maxCount/4];
    int * Counter3 = new int[maxCount/4];
    int * Counter4 = new int[maxCount/4];

    //Asigna las instrucciones que cada core debera ejecutar
    for (int i = 0; i < maxCount/4; i++) {

        Counter1[i] = 0 + i*4;
        Counter2[i] = 1 + i*4;
        Counter3[i] = 2 + i*4;
        Counter4[i] = 3 + i*4;

    }

    fstream  memory (argv[1]);
    unsigned int addressRaw;
    int operation;

    for(int i = 0; i < maxCount/4; i++) {

        int indexTest;

        addressRaw = getAddress(memory, Counter1[i]);
        operation = getOperation(memory);
        indexTest = convertIndex(addressRaw, L1);
        instructionPrint(1, Core1, Core2, Core3, Core4, Share, addressRaw,
            operation, Counter1[i], memory, indexTest);

        addressRaw = getAddress(memory, Counter2[i]);
        operation = getOperation(memory);
        indexTest = convertIndex(addressRaw, L1);
        instructionPrint(2, Core1, Core2, Core3, Core4, Share, addressRaw,
            operation, Counter2[i], memory, indexTest);

        addressRaw = getAddress(memory, Counter3[i]);
        operation = getOperation(memory);
        indexTest = convertIndex(addressRaw, L1);
        instructionPrint(3, Core1, Core2, Core3, Core4, Share, addressRaw,
            operation, Counter3[i], memory, indexTest);

        addressRaw = getAddress(memory, Counter4[i]);
        operation = getOperation(memory);
        indexTest = convertIndex(addressRaw, L1);
        instructionPrint(4, Core1, Core2, Core3, Core4, Share, addressRaw,
            operation, Counter4[i], memory, indexTest);

    }

    return 0;
}
