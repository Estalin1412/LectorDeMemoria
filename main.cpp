//
// Created by jemd2 on 6/11/2024.
//
#include <iostream>
#include <windows.h>
#include <vector>
/*Para trabajar con las instancias de procesos*/
#include <cmath>
#include <tlhelp32.h>
#include <memory>
#include <cstring>

using std::endl;
using std::cout;
using std::cin;
using std::string;

#define MENU_DE_OPCIONES  0
#define INGRESE_OPCION  1
#define OP1CAMBIAR_VALOR_COMUN  2
#define OP2CAMBIAR_VALOR_PUNTERO  3
#define OP3CAMBIAR_PUNTERO_CAMBIANTE  4
#define OP4OBTENER_PROCESOS  5

/*Encabezado de funciones*/
int Menu();
int Menu(int &a);
//Para tipo int
void ListAllProcesses();
void funEscanearMemoria(HANDLE gestionar, int targetValue, std::vector<void*>& direcciones);
DWORD getProcessID(char * processName);
//Para float
void funEscanearMemoria(HANDLE gestionar, float targetValue, std::vector<void*>& direcciones);
bool funCambiarValor(HANDLE proceso, LPVOID direccion, float valor, DWORD &pid);
string diccionary[]={
    "Menu de opciones.",
    "Ingrese la opcion que desea: ",
    "1) Cambiar datos de la memoria.",
    "2) Leer variable que cambia.",
    "3) Leer puntero varible.",
    "5) Leer procesos."
};


int main() {

    int op = Menu(op);
    //Lista todos lo procesos que se estan ejecutando en windows
    ListAllProcesses();
    //Creo variable con el valor del proceso
    const int sise = 30;
    char *proceso = new char[sise];
    cout<<"Ingrese el procesos que quiere leer: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.getline(proceso, 30);
    //Para encontrar el ID del proceso
    DWORD lectura = getProcessID(proceso);
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, lectura);
    //Escaneo variables con valor numerico
    float targetValue = 0.0;
    cout<< "\nIngrese el valor que desea buscar: " ;
    cin >> targetValue;
    std::vector<void*> direcciones;
    funEscanearMemoria(processHandle, targetValue, direcciones);
    cout << "Direcciones encontradas: " << endl;

    for (auto direccion : direcciones) {
        cout << direccion << endl;
    }

    cout<< "\nIngrese el valor que desea filtrar: " ;
    cin >> targetValue;

    std::vector<void*> direccionesFiltradas;

    for (void* direccion : direcciones) {
        float nuevoValor;
        SIZE_T bytesLeidos;
        if (ReadProcessMemory(processHandle, direccion, &nuevoValor, sizeof(int), &bytesLeidos) && nuevoValor == targetValue) {
            direccionesFiltradas.push_back(direccion);
            cout << "Valor: " << nuevoValor
                 << " en dirección: " << direccion << endl;
        }
    }
    //Veamos para cambiar valor


    if (!direccionesFiltradas.empty()) {
        float nuevoValor;
        cout << "\nIngrese el nuevo valor que desea escribir: ";
        cin >> nuevoValor;
    //Cambiar valor
        for (auto direccion : direccionesFiltradas) {
            if (funCambiarValor(processHandle, direccion, nuevoValor, lectura)) {
                cout << "Valor cambiado exitosamente en la dirección: " << direccion << endl;
            } else {
                cout << "Error al cambiar el valor en la dirección: " << direccion << endl;
            }
        }
    } else {
        cout << "No se encontraron direcciones que coincidan con el valor filtrado." << endl;
    }

    return 0;
}

/*DEFINICION DE FUNCIONES---------------------------------------------------------------------------------------------------------------*/
/* ListAllProcesses():
 * Crea un objeto PROCESSENTRY32 representa procesos de 32 butyes
 * Creamos una instancia que lea todos lo prcesoses que se estan ejecutando
 * Imprimimos los procesos leidos y su ID
 */
void ListAllProcesses() {
    PROCESSENTRY32 entradaProcesos;
    entradaProcesos.dwSize = sizeof(PROCESSENTRY32);
    // Tomamos una instantánea de todos los procesos en ejecución
    HANDLE instancia = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (instancia == INVALID_HANDLE_VALUE) {
        cout << "No carga instancia" << GetLastError() << endl;
        return;
    }
    // Iteramos por los procesos encontrados
    if (Process32First(instancia, &entradaProcesos)) {
        do {
            cout << "Proceso: " << entradaProcesos.szExeFile
                 << "\tID:  " << entradaProcesos.th32ProcessID << endl;
        } while (Process32Next(instancia, &entradaProcesos));
    } else {
        cout << "Error en la primera entrada" << GetLastError() << endl;
    }
    // Cerramos el handle de la instantánea
    CloseHandle(instancia);
}


/*FUNCIONES----------------------------------------------------------------------------------------------------------------------------*/
//Función para menu de opciones
/*
 * Te muestre el menu de opciones
 * Te pide que ingrese una opcion
 * Te devuelve la opcion ingresada
 */
int Menu() {
    cout<<diccionary[MENU_DE_OPCIONES]<<endl;
    cout<<diccionary[OP1CAMBIAR_VALOR_COMUN]<<endl;
    cout<<diccionary[OP2CAMBIAR_VALOR_PUNTERO]<<endl;
    cout<<diccionary[OP3CAMBIAR_PUNTERO_CAMBIANTE]<<endl;
    cout<<diccionary[OP4OBTENER_PROCESOS]<<endl;
    int a = 0;
    cout<<"Ingrese la opcion deseada: "<<endl;
    cin>>a;
    return a;
}
/*
 * Entrada: Ingresa un valor donde quieres guardar el valor
 * Te muestre el menu de opciones
 * Te pide que ingrese una opcion
 * Guarda la opcion ingresada
 * Te devuelve la opcion ingresada
 */
int Menu(int & a){
    cout<<diccionary[MENU_DE_OPCIONES]<<endl;
    cout<<diccionary[OP1CAMBIAR_VALOR_COMUN]<<endl;
    cout<<diccionary[OP2CAMBIAR_VALOR_PUNTERO]<<endl;
    cout<<diccionary[OP3CAMBIAR_PUNTERO_CAMBIANTE]<<endl;
    cout<<diccionary[OP4OBTENER_PROCESOS]<<endl;
    cout<<"Ingrese la opcion deseada: "<<endl;
    cin>>a;
    return a;
}

/*----------------------------------------------------------------------------------------------------------------------------------*/
DWORD getProcessID(char * processName) {
    DWORD procesoId = 0;
    PROCESSENTRY32 entrada;
    entrada.dwSize = sizeof(PROCESSENTRY32);
    HANDLE instancia = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(Process32First(instancia, &entrada)) {
        do {
            if(strcmp(entrada.szExeFile, processName) == 0) {
                procesoId = entrada.th32ProcessID;
                break;
            }
        } while (Process32Next(instancia, &entrada));
    }
    CloseHandle(instancia);
    return procesoId;
}

void funEscanearMemoria(HANDLE gestionar, int targetValue, std::vector<void*>& direcciones) {
    SYSTEM_INFO inforSistema;
    GetSystemInfo(&inforSistema);

    LPCVOID inicio = inforSistema.lpMinimumApplicationAddress;
    LPCVOID fin= inforSistema.lpMaximumApplicationAddress;
    MEMORY_BASIC_INFORMATION inforMemoria;

    for(LPCVOID address = inicio; address < fin; ) {
        if(VirtualQueryEx(gestionar, address,&inforMemoria, sizeof(inforMemoria)) == 0) {
            break;
        }

        //Verificamos
        if(inforMemoria.State == MEM_COMMIT && (inforMemoria.Protect == PAGE_READWRITE || inforMemoria.Protect == PAGE_EXECUTE_READWRITE) ) {
            BYTE* buffer = new BYTE[inforMemoria.RegionSize];
            SIZE_T byteLeido;

            if(ReadProcessMemory(gestionar, address, buffer,inforMemoria.RegionSize, &byteLeido)) {
                for(SIZE_T i = 0; i < byteLeido - sizeof(int); i++) {
                    int value = *(int*)(buffer+i);
                    if(value == targetValue) {
                        void * direccionEncontrada = static_cast<void*>((BYTE*)address + i);
                        direcciones.push_back(direccionEncontrada);
                        cout<<"Se encuentra: "<<value<<" en : "<<static_cast<void*>((BYTE*)address+i)<<endl;
                    }
                }
            }
            delete[] buffer;
        }
        address = (BYTE*)inforMemoria.BaseAddress + inforMemoria.RegionSize;
    }
}
/*FUNCION CAMBIAR VALOR DE MEMORIA----------------------------------------------------------------------------------------------------*/
bool funCambiarValor(HANDLE proceso, LPVOID direccion, int valor) {
    SIZE_T byteLeido;
    if(WriteProcessMemory(proceso, direccion, &valor, sizeof(valor), &byteLeido)){
        cout<<"Nuevo valor a: "<<valor<<endl;
        return true;
    }
    return false;
}
/****FLOAT*******************************************************************************************************************/
void funEscanearMemoria(HANDLE gestionar, float targetValue, std::vector<void*>& direcciones) {
    SYSTEM_INFO inforSistema;
    GetSystemInfo(&inforSistema);

    LPCVOID inicio = inforSistema.lpMinimumApplicationAddress;
    LPCVOID fin = inforSistema.lpMaximumApplicationAddress;
    MEMORY_BASIC_INFORMATION inforMemoria;

    for (LPCVOID address = inicio; address < fin;) {
        if (VirtualQueryEx(gestionar, address, &inforMemoria, sizeof(inforMemoria)) == 0) {
            break;
        }

        // Verificamos que el bloque de memoria sea accesible y escribible
        if (inforMemoria.State == MEM_COMMIT &&
            (inforMemoria.Protect == PAGE_READWRITE || inforMemoria.Protect == PAGE_EXECUTE_READWRITE)) {
            BYTE* buffer = new BYTE[inforMemoria.RegionSize];
            SIZE_T byteLeido;

            if (ReadProcessMemory(gestionar, address, buffer, inforMemoria.RegionSize, &byteLeido)) {
                for (SIZE_T i = 0; i < byteLeido - sizeof(float); i++) {
                    float value = *(float*)(buffer + i);
                    // Usamos comparación con tolerancia para float
                    if (std::fabs(value - targetValue) < 0.0001f) {
                        void* direccionEncontrada = static_cast<void*>((BYTE*)address + i);
                        direcciones.push_back(direccionEncontrada);
                        cout << "Se encuentra: " << value
                             << " en: " << direccionEncontrada << endl;
                    }
                }
            }
            delete[] buffer;
            }
        address = (BYTE*)inforMemoria.BaseAddress + inforMemoria.RegionSize;
    }
}
bool funCambiarValor(HANDLE proceso, LPVOID direccion, float valor, DWORD &pid) {

    proceso = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    SIZE_T byteEscrito;
    if (WriteProcessMemory(proceso, direccion, &valor, sizeof(valor), &byteEscrito)) {
        cout << "Valor cambiado exitosamente a: " << valor << endl;
        return true;
    } else {
        cout << "Error al cambiar el valor en la dirección: " << direccion << endl;
        return false;
    }
}


