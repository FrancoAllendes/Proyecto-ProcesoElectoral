#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SistemaElectoral {
    struct NodoEleccion *headelecciones;
    struct NodoVotante *rairegistroelectoral;
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
    struct NodoVotante *izq;
    struct NodoVotante *der;
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