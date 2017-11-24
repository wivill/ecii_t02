#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
///////////////////////////////////////////////////////////////////////////////
//// Definicion de Estados  0 Modificado , 1  Share  , 2 Exlusive ,3 Invalido
//// Cada Cache tiene un struct por cada bloque, cada bloque tiene tag ,estado
//// y a cual core pertenece
//// Para evitar trabajar tanto con strings un Load sera 0 y un Store sera un 1
////
////
////
////
////
///////////////////////////////////////////////////////////////////////////////

using namespace std;

/*
    Algunas definiciones para facilitar un poco el trabajo con los estados
    y niveles de caché.
*/

#define L1 1
#define L2 2
#define MOD 0
#define EX 1
#define SH 2
#define INV 3


// Estructura de datos del caché sobre el cual se trabajará
struct cache {

    int state;
    unsigned int tag;
    int CoreId;

};

//Funcion para obtener una linea especifica


std::fstream& GotoLine(std::fstream& file,  int num) {

    file.seekg(std::ios::beg);

    for(int i = 0; i < num; ++i) {
           file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return file;

}

//instruccion para determinar el numero maximo de lineas
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

// Funcion para inicializacion de caches


void Init(vector<cache>& Cache, int size, int id){

    for (int i = 0; i < size; i++) {
        Cache.push_back(cache());
        Cache[i].state = 3;
        Cache[i].CoreId = id;
    }

}

//////////////////////////////////// Conversion de direcciones


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

//////////////////////////////////////////////Escritura y lectura de tags y estados
int getState(unsigned int address, vector<cache> & Core) {

    int stateReturn;
    unsigned int index;
    index = convertIndex(address, L1);
    stateReturn = Core[index].state;
    return stateReturn;

}

void setState(unsigned int address,vector<cache> & Core,int stateNew){

    unsigned  int index;
    index = convertIndex(address, L1);
    Core[index].state = stateNew;

}

void write(unsigned int address, vector<cache> & Core, int level) {

    unsigned int index, Tag;

    if (level == L1){
        index = convertIndex(address, L1);
        Tag = convertTag(address, L1);
        Core[index].tag = Tag;
    }
    else {
        index=convertIndex(address, L2);
        Tag=convertTag(address, L2);
        Core[index].tag=Tag;
    }

}

//Funciones para obtener la direccion en decimal y conversion de operacion a numero

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
//////////////////////////////////////////////////// Hasta aqui herramientas

void readBusRd(unsigned int index, unsigned int tag, unsigned int address,
               int stateA, vector<cache>& Core, vector<cache>& CoreA,
               vector<cache>& Share) {

    if(CoreA[index].tag==tag) {
        switch(stateA) {
            case 0:
                write(address, Share, L2);
                setState(address, CoreA, SH);
                write(address, Core, L1);
                setState(address, Core, SH);
                break;

            case 1:
                write(address, Core, L1);
                setState(address, Core, SH);
                break;

            case 2:
                setState(address, CoreA, SH);
                write(address, Core, L1);
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
        write(address, Share, L2);
    }

}

void stateCoreModifier(vector<cache>& Core, vector<cache>& CoreA,
                       vector<cache>& CoreB, vector<cache>& CoreC,
                       unsigned int address,int operation,
                       vector<cache>& Share) {

    int state, stateA, stateB, stateC;
    unsigned int tag;
    unsigned int index;
    tag = convertTag(address, L1);
    index = convertIndex(address, L1);

    state = getState(address, Core);
    stateA = getState(address, CoreA);
    stateB = getState(address, CoreB);
    stateC = getState(address, CoreC);

    if (operation == 0) {

        if(Core[index].tag != tag) {
            state = 3;
        }

        switch (state) {
            case 0:
                break;

            case 1:
                break;

            case 2:
                break;

            case 3:
                if((stateA == 3) && (stateB == 3) && (stateC == 3)) {
                    setState(address, Core, EX);
                    write(address, Core, L1);
                    write(address, Share, L2);
                }
                else if((CoreA[index].tag != tag) && (CoreB[index].tag != tag)
                        && (CoreC[index].tag != tag)) {
                    setState(address, Core, EX);
                    write(address, Core, L1);
                    write(address, Share, L2);}
                else {
                    readBusRd(index, tag,address, stateA, Core, CoreA, Share);
                    readBusRd(index, tag,address, stateB, Core, CoreB, Share);
                    readBusRd(index, tag,address, stateC, Core, CoreC, Share);
                }
                break;

            default:
                break;
        }

    }

    else {

        switch(state) {
            case 0:
                write(address, Core, L1);
                break;

            case 1:
                setState(address, Core, 0);
                write(address, Core, 1);
                invalidCaches(index, tag, address, stateA, CoreA, Share);
                invalidCaches(index, tag, address, stateB, CoreB, Share);
                invalidCaches(index, tag, address, stateC, CoreC, Share);
                break;

            case 2:
                setState(address, Core, 0);
                write(address, Core, 1);
                break;

            case 3:
                setState(address, Core, 0);
                write(address, Core, 1);
                invalidCaches(index, tag, address, stateA, CoreA, Share);
                invalidCaches(index, tag, address, stateB, CoreB, Share);
                invalidCaches(index, tag, address, stateC, CoreC, Share);
                break;

            default:
                break;
        }

    }

}


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

void instructionPrint(int coreId, vector<cache> & Core1, vector<cache> & Core2,
    vector<cache> & Core3, vector<cache> & Core4, vector<cache> & Share,
    unsigned int addressRaw, int operation, int line, std::fstream & memory,
    int indexTest) {

    cout << "Dirección Raw: " << addressRaw << endl;
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

    cout << "\n" << endl;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

    vector<cache> Core1, Core2, Core3, Core4, Share;
    Init(Core1, 1000, 1);
    Init(Core2, 1000, 2);
    Init(Core3, 1000, 3);
    Init(Core4, 1000, 4);
    Init(Share, 8000, 0);

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
        // addressRaw = getAddress(memory, Counter1[i]);
        // cout << "Dirección Raw: " << addressRaw << endl;
        // operation = getOperation(memory);
        // Controller(1, Core1, Core2, Core3, Core4, Share, addressRaw, operation);
        // indexTest = convertIndex(addressRaw, L1);
        // cout << "Índice: " << indexTest << "   " << "Operación: " << operation << endl;
        // cout << Core1[indexTest].tag << "\t" << Core1[indexTest].state << endl;
        // cout << Core2[indexTest].tag << "\t" << Core2[indexTest].state << endl;
        // cout << Core3[indexTest].tag << "\t" << Core3[indexTest].state << endl;
        // cout << Core4[indexTest].tag << "\t" << Core4[indexTest].state << endl;
        // cout << "\n" << endl;
        //
        // addressRaw = getAddress(memory, Counter2[i]);
        // cout << addressRaw << endl;
        // operation = getOperation(memory);
        // Controller(2, Core1, Core2, Core3, Core4, Share, addressRaw, operation);
        // indexTest = convertIndex(addressRaw, L1);
        // cout << indexTest << "   " << operation << endl;
        // cout << Core1[indexTest].tag << "\t" << Core1[indexTest].state << endl;
        // cout << Core2[indexTest].tag << "\t" << Core2[indexTest].state << endl;
        // cout << Core3[indexTest].tag << "\t" << Core3[indexTest].state << endl;
        // cout << Core4[indexTest].tag << "\t" << Core4[indexTest].state << endl;
        // cout << "\n" << endl;
        //
        // addressRaw = getAddress(memory, Counter3[i]);
        // cout << addressRaw << endl;
        // operation = getOperation(memory);
        // Controller(3, Core1, Core2, Core3, Core4, Share, addressRaw, operation);
        // indexTest = convertIndex(addressRaw, L1);
        // cout << indexTest << "   " << operation << endl;
        // cout << Core1[indexTest].tag << "\t" << Core1[indexTest].state << endl;
        // cout << Core2[indexTest].tag << "\t" << Core2[indexTest].state << endl;
        // cout << Core3[indexTest].tag << "\t" << Core3[indexTest].state << endl;
        // cout << Core4[indexTest].tag << "\t" << Core4[indexTest].state << endl;
        // cout << "\n" << endl;
        //
        // addressRaw = getAddress(memory, Counter4[i]);
        // cout << addressRaw << endl;
        // operation = getOperation(memory);
        // Controller(4, Core1, Core2, Core3, Core4, Share, addressRaw, operation);
        // indexTest = convertIndex(addressRaw, L1);
        // cout << indexTest << "   " << operation << endl;
        // cout << Core1[indexTest].tag << "\t" << Core1[indexTest].state << endl;
        // cout << Core2[indexTest].tag << "\t" << Core2[indexTest].state << endl;
        // cout << Core3[indexTest].tag << "\t" << Core3[indexTest].state << endl;
        // cout << Core4[indexTest].tag << "\t" << Core4[indexTest].state << endl;
        // cout << "\n" << endl;
    }

    return 0;
}
