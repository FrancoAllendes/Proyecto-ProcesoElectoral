#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------- */
/* --- DEFINES --- */
/* ----------------------------------------------------------------- */
#define MAXCANDIDATOS 50 /* Limite para el pool de PLibre */

/* ----------------------------------------------------------------- */
/* --- ESTRUCTURAS DE DATOS --- */
/* ----------------------------------------------------------------- */

struct SistemaElectoral {
    struct NodoEleccion *headelecciones;
    struct NodoVotante *headregistroelectoral;
    struct NodoVotante *tailregistroelectoral;
};

struct NodoEleccion {
    int numerovuelta;
    char fecha[20];
    struct Candidato *arraycandidatos;
    int numcandidatos;
    struct NodoMesa *raizarbolmesas;
    struct NodoEleccion *sig;
};

struct NodoVotante {
    long rut;
    char nombre[100];
    char comuna[50];
    char region[50];
    int havotado;
    struct NodoMesa *mesaasinada;
    struct NodoVotante *sig;
    struct NodoVotante *ant;
};

struct NodoMesa {
    int idmesa;
    char comuna[50];
    char region[50];
    int votosemitidos;
    long *conteovotos;
    struct NodoVoto *headlistavotos;
    struct NodoMesa *izq;
    struct NodoMesa *der;
};

struct Candidato {
    int idcandidato;
    char nombre[100];
    char partido[50];
};

struct NodoVoto {
    long rutvotante;
    int idcandidatovotado;
    struct NodoVoto *sig;
};

/* ----------------------------------------------------------------- */
/* --- VARIABLES GLOBALES (Para PLibre) --- */
/* ----------------------------------------------------------------- */

struct Candidato poolCandidatos[MAXCANDIDATOS];
int plibre = 0;

/* ----------------------------------------------------------------- */
/* --- PROTOTIPOS DE FUNCIONES --- */
/* ----------------------------------------------------------------- */

/* --- Funciones de Utilidad --- */
void limpiarPantalla();
void limpiarBuffer();
void esperarEnter();
int ingresarOpcion();

/* --- Gestion del Registro Electoral (Votantes) --- */
void menuGestionVotantes(struct SistemaElectoral *sistema);
void agregarVotante(struct SistemaElectoral *sistema);
struct NodoVotante* buscarVotante(struct SistemaElectoral *sistema, long rutBuscado); 
int modificarVotante(struct SistemaElectoral *sistema, long rutModificar); 
int eliminarVotante(struct SistemaElectoral *sistema, long rutEliminar); 
void listarVotantes(struct SistemaElectoral *sistema);
int asignarMesaAVotante(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual); 

/* --- Gestion de Candidaturas --- */
void menuGestionCandidatos(struct NodoEleccion *eleccionActual);
void agregarCandidato(struct NodoEleccion *eleccionActual);
struct Candidato* buscarCandidato(struct NodoEleccion *eleccionActual, int idBuscado); 
int modificarCandidato(struct NodoEleccion *eleccionActual, int idModificar); 
int eliminarCandidato(struct NodoEleccion *eleccionActual, int idEliminar); 
void listarCandidatos(struct NodoEleccion *eleccionActual);

/* --- Gestion de Mesas de Votacion --- */
void menuGestionMesas(struct NodoEleccion *eleccionActual);
void agregarMesa(struct NodoEleccion *eleccionActual);
struct NodoMesa* buscarMesa(struct NodoEleccion *eleccionActual, int idBuscada); 
int modificarMesa(struct NodoEleccion *eleccionActual, int idModificar); 
int eliminarMesa(struct NodoEleccion *eleccionActual, int idEliminar); 
void listarMesas(struct NodoEleccion *eleccionActual);

/* --- Votacion y Escrutinio --- */
void menuVotacion(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);
void registrarVoto(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);

/* --- Funciones Extras --- */
/* Submenu que agrupa las operaciones de Escrutinio y 2da Vuelta */
void menuOperacionesEspeciales(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);

/* Recorre el arbol de mesas y calcula/imprime los resultados nacionales */
void realizarEscrutinioNacional(struct NodoEleccion *eleccionActual);

/* Revisa resultados y, si es necesario, crea y enlaza un nuevo NodoEleccion */
void gestionarSegundaVuelta(struct SistemaElectoral *sistema);

/* ----------------------------------------------------------------- */
/* --- FUNCION PRINCIPAL --- */
/* ----------------------------------------------------------------- */

int main() {
/* 1. Inicializar el Sistema */
    struct SistemaElectoral sistema;
    sistema.headelecciones = NULL;
    sistema.headregistroelectoral = NULL; /* Puntero 'head' de la Lista Doble de Votantes */
    sistema.tailregistroelectoral = NULL; /* Puntero 'tail' de la Lista Doble de Votantes */

    /* 2. Logica para crear e inicializar la Primera Vuelta */
    /* (Creamos la primera vuelta y la enganchamos al sistema) */
    struct NodoEleccion *primeraVuelta = (struct NodoEleccion*)malloc(sizeof(struct NodoEleccion));
    primeraVuelta->numerovuelta = 1;
    strcpy(primeraVuelta->fecha, "19-11-2025"); /* Fecha de ejemplo */
    primeraVuelta->arraycandidatos = NULL; /* Se llena con PLibre */
    primeraVuelta->numcandidatos = 0;
    primeraVuelta->raizarbolmesas = NULL; /* Arbol de mesas vacio */
    primeraVuelta->sig = NULL;
    
    sistema.headelecciones = primeraVuelta;
    
    /* (Fin de la inicializacion) */

    int opcion = 0;
    struct NodoEleccion *eleccionActual = sistema.headelecciones;
    
    do {
        limpiarPantalla();
        printf("\n=== SISTEMA ELECTORAL PRESIDENCIAL CHILENO ===\n");
        printf(" (Vuelta Actual: %d - Fecha: %s)\n", eleccionActual->numerovuelta, eleccionActual->fecha);
        printf("------------------------------------------------\n");
        printf(" 1. Gestionar Registro Electoral (Votantes)\n");
        printf(" 2. Gestionar Candidaturas (de la vuelta actual)\n");
        printf(" 3. Gestionar Mesas (de la vuelta actual)\n");
        printf(" 4. Iniciar Proceso de Votacion\n");
        printf(" 5. Funciones Extras (Escrutinio / 2da Vuelta)\n");
        printf(" 0. Salir\n");
        
        opcion = ingresarOpcion();

        switch (opcion) {
            case 1:
                /* Llama al submenu de Votantes */
                menuGestionVotantes(&sistema);
                break;
            case 2:
                /* Llama al submenu de Candidatos */
                menuGestionCandidatos(eleccionActual);
                break;
            case 3:
                /* Llama al submenu de Mesas */
                menuGestionMesas(eleccionActual);
                break;
            case 4:
                /* Llama al submenu de Votacion */
                menuVotacion(&sistema, eleccionActual);
                break;
            case 5:
                /* Llama al submenu de Funciones Extras */
                menuFuncionesExtras(&sistema, eleccionActual);
                break;
            case 0:
                printf("Saliendo del sistema.\n");
                break;
            default:
                printf("Opcion no valida.\n");
                esperarEnter();
        }
        
        /* (Logica simple para avanzar a la 2da vuelta si se creo) */
        if (eleccionActual->sig != NULL) {
            eleccionActual = eleccionActual->sig;
        }

    } while (opcion != 0);
    return 0;
}

void limpiarPantalla() {
    int i;
    for(i = 0; i < 30; i++) {
        printf("\n");
    }
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void esperarEnter() {
    printf("\nPresione Enter para continuar...\n");
    limpiarBuffer(); /* Limpia cualquier '\n' que haya quedado antes */
    (void)getchar(); /* Espera a que el usuario presione Enter */
}

int ingresarOpcion() {
    int opcion;
    char buffer[100]; /* Un buffer para leer la linea completa */

    printf("Seleccione una opcion: ");
    
    /* Leemos la linea entera como texto */
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        
        /* Intentamos convertir el texto a un numero */
        /* sscanf() devuelve 1 si tuvo exito */
        if (sscanf(buffer, "%d", &opcion) == 1) {
            return opcion; /* Exito: devuelve el numero */
        }
    }
    
    /* Si no se pudo leer o no fue un numero, devuelve -1 */
    return -1; 
}