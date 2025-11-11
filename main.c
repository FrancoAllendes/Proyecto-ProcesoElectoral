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

/* Estructura Estatica: Arreglo Compacto */
struct Candidato {
    int idcandidato;
    char nombre[100];
    char partido[50];
};

/* Estructura Dinamica Simple: Lista Circular */
struct NodoVoto {
    long rutvotante;
    int idcandidatovotado;
    struct NodoVoto *sig;
};

/* Estructura Dinamica Compleja: Arbol BST */
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

/* Estructura Dinamica Compleja: Arbol BST */
struct NodoVotante {
    long rut;
    char nombre[100];
    char comuna[50];
    char region[50];
    int havotado;
    struct NodoMesa *mesaasinada;
    struct NodoVotante *izq;
    struct NodoVotante *der;
};

/* Estructura Dinamica Simple: Lista Simple */
struct NodoEleccion {
    int numerovuelta;
    char fecha[20];
    struct Candidato *arraycandidatos;
    int numcandidatos;
    struct NodoMesa *raizarbolmesas;
    struct NodoEleccion *sig;
};

/* Estructura Raiz */
struct SistemaElectoral {
    struct NodoEleccion *headelecciones;
    struct NodoVotante *rairegistroelectoral;
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
void buscarVotante(struct SistemaElectoral *sistema);
void modificarVotante(struct SistemaElectoral *sistema);
void eliminarVotante(struct SistemaElectoral *sistema);
void listarVotantes(struct SistemaElectoral *sistema);
void asignarMesaAVotante(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);

/* --- Gestion de Candidaturas --- */
void menuGestionCandidatos(struct NodoEleccion *eleccionActual);
void agregarCandidato(struct NodoEleccion *eleccionActual);
void buscarCandidato(struct NodoEleccion *eleccionActual);
void modificarCandidato(struct NodoEleccion *eleccionActual);
void eliminarCandidato(struct NodoEleccion *eleccionActual);
void listarCandidatos(struct NodoEleccion *eleccionActual);

/* --- Gestion de Mesas de Votacion --- */
void menuGestionMesas(struct NodoEleccion *eleccionActual);
void agregarMesa(struct NodoEleccion *eleccionActual);
void buscarMesa(struct NodoEleccion *eleccionActual);
void modificarMesa(struct NodoEleccion *eleccionActual);
void eliminarMesa(struct NodoEleccion *eleccionActual);
void listarMesas(struct NodoEleccion *eleccionActual);

/* --- Votacion y Escrutinio --- */
void menuVotacion(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);
void registrarVoto(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);

/* --- Funciones Extras --- */
void menuFuncionesExtras(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);
void funcionExtra1_Escrutinio(struct NodoEleccion *eleccionActual);
void funcionExtra2_CrearSegundaVuelta(struct SistemaElectoral *sistema);

/* ----------------------------------------------------------------- */
/* --- FUNCION PRINCIPAL --- */
/* ----------------------------------------------------------------- */

int main() {

    return 0;

}