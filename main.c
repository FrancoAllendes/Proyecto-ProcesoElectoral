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
    char rut[11];
    char nombre[100];
    char comuna[50];
    char region[50];
    int havotado;
    struct NodoMesa *mesaasignada; 
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
    char rutvotante[11];
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
void normalizarRUT(char* rutLimpio, char* rutSocio);

/* --- Gestion del Registro Electoral (Votantes) --- */
void menuGestionVotantes(struct SistemaElectoral *sistema);
void agregarVotante(struct SistemaElectoral *sistema);
struct NodoVotante* buscarVotante(struct SistemaElectoral *sistema, char* rutBuscado); 
int modificarVotante(struct SistemaElectoral *sistema, char* rutModificar); 
int eliminarVotante(struct SistemaElectoral *sistema, char* rutEliminar); 
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
void menuOperacionesEspeciales(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);
void realizarEscrutinioNacional(struct NodoEleccion *eleccionActual);
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
                menuOperacionesEspeciales(&sistema, eleccionActual);
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
/* ----------------------------------------------------------------- */
/* --- IMPLEMENTACION DE FUNCIONES DE UTILIDAD --- */
/* ----------------------------------------------------------------- */

void normalizarRUT(char* rutLimpio, char* rutSocio) {
    /* Declaracion de variables (ANSI C) */
    int i;
    int j;
    
    i = 0; /* Indice para leer el rutSocio */
    j = 0; /* Indice para escribir en rutLimpio */

    /* Recorremos el RUT "sucio" de entrada */
    while (rutSocio[i] != '\0' && j < 10) { /* j < 10 para evitar desbordar rutLimpio[11] */
        
        /* 1. Si es un digito (0-9) o la letra 'k' */
        if ( (rutSocio[i] >= '0' && rutSocio[i] <= '9') || 
             (rutSocio[i] == 'k' || rutSocio[i] == 'K') ) 
        {
            rutLimpio[j] = rutSocio[i];
            j++;
        }
        /* 2. Si es un guion */
        else if (rutSocio[i] == '-') 
        {
            rutLimpio[j] = rutSocio[i];
            j++;
        }
        /* 3. Si es un punto ('.') o un espacio, lo ignoramos */
        else if (rutSocio[i] == '.' || rutSocio[i] == ' ') 
        {
            /* No hacer nada, solo avanzar i */
        }
        
        i++;
    }
    
    /* 4. Cerramos el string limpio */
    rutLimpio[j] = '\0';
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
/* ----------------------------------------------------------------- */
/* --- IMPLEMENTACION GESTION DE MESAS --- */
/* ----------------------------------------------------------------- */

/*  --- Funciones Internas del Arbol BST ---  */

/* Funcion interna: Crea un nuevo NodoMesa y asigna memoria */
struct NodoMesa* crearNodoMesa(int id, char* com, char* reg, int numCands) {
    struct NodoMesa* nuevo;
    int i;

    nuevo = (struct NodoMesa*)malloc(sizeof(struct NodoMesa));
    if (nuevo == NULL) {
        printf("ERROR FATAL: No hay memoria para crear la mesa.\n");
        return NULL;
    }

    nuevo->idmesa = id;
    strcpy(nuevo->comuna, com);
    strcpy(nuevo->region, reg);
    nuevo->votosemitidos = 0;
    nuevo->headlistavotos = NULL;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    
    nuevo->conteovotos = (long*)malloc(numCands * sizeof(long));
    if (nuevo->conteovotos == NULL) {
        printf("ERROR FATAL: No hay memoria para contadores de votos.\n");
        return NULL;
    }
    
    for (i = 0; i < numCands; i++) {
        nuevo->conteovotos[i] = 0;
    }

    return nuevo;
}

/* Funcion recursiva interna: Inserta un NodoMesa en el Arbol BST */
struct NodoMesa* insertarMesaRec(struct NodoMesa* raiz, struct NodoMesa* nuevo) {
    if (raiz == NULL) {
        return nuevo;
    }

    if (nuevo->idmesa < raiz->idmesa) {
        raiz->izq = insertarMesaRec(raiz->izq, nuevo);
    } else if (nuevo->idmesa > raiz->idmesa) {
        raiz->der = insertarMesaRec(raiz->der, nuevo);
    } else {
        printf("Error: La mesa con ID %d ya existe.\n", nuevo->idmesa);
    }
    return raiz;
}

/* Funcion recursiva interna: Busca un NodoMesa en el BST por ID */
struct NodoMesa* buscarMesaRec(struct NodoMesa* raiz, int idBuscada) {
    if (raiz == NULL) {
        return NULL;
    }
    
    if (raiz->idmesa == idBuscada) {
        return raiz;
    }

    if (idBuscada < raiz->idmesa) {
        return buscarMesaRec(raiz->izq, idBuscada);
    } else {
        return buscarMesaRec(raiz->der, idBuscada);
    }
}

/* Funcion recursiva interna: Recorre (InOrden) y muestra todas las mesas */
void listarMesasRec(struct NodoMesa* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    listarMesasRec(raiz->izq);
    
    printf("  -> ID Mesa: %-5d | Comuna: %-20s | Region: %s\n",
           raiz->idmesa, raiz->comuna, raiz->region);
           
    listarMesasRec(raiz->der);
}

/* Funcion interna (para 'eliminar'): Busca el nodo con el valor mas pequeno */
struct NodoMesa* encontrarMinimo(struct NodoMesa* nodo) {
    struct NodoMesa* actual = nodo;

    while (actual != NULL && actual->izq != NULL) {
        actual = actual->izq;
    }

    return actual;
}

/* Funcion recursiva interna: Elimina un nodo del BST */
struct NodoMesa* eliminarMesaRec(struct NodoMesa* raiz, int idEliminar) {
    struct NodoMesa* sucesor;

    if (raiz == NULL) {
        return NULL;
    }

    if (idEliminar < raiz->idmesa) {
        raiz->izq = eliminarMesaRec(raiz->izq, idEliminar);
    } else if (idEliminar > raiz->idmesa) {
        raiz->der = eliminarMesaRec(raiz->der, idEliminar);
    
    } else {
        
        if (raiz->izq == NULL) {
            return raiz->der;
        
        } else if (raiz->der == NULL) {
            return raiz->izq;
        }

        sucesor = encontrarMinimo(raiz->der);

        raiz->idmesa = sucesor->idmesa;
        strcpy(raiz->comuna, sucesor->comuna);
        strcpy(raiz->region, sucesor->region);
        raiz->votosemitidos = sucesor->votosemitidos;
        raiz->conteovotos = sucesor->conteovotos;
        raiz->headlistavotos = sucesor->headlistavotos;

        raiz->der = eliminarMesaRec(raiz->der, sucesor->idmesa);
    }
    return raiz;
}


/*  --- Funciones Publicas (Llamadas por el Menu) --- */

/* Pide datos y agrega una nueva mesa al arbol de la eleccion actual */
void agregarMesa(struct NodoEleccion *eleccionActual) {
    int idMesa;
    char comuna[50];
    char region[50];
    char buffer[100];
    struct NodoMesa* nuevoNodo;
    
    limpiarPantalla();
    printf("--- Agregar Nueva Mesa de Votacion ---\n");

    if (eleccionActual == NULL) {
        printf("Error: No hay una eleccion activa.\n");
        esperarEnter();
        return;
    }
    if (eleccionActual->numcandidatos == 0) {
        printf("Error: Debe agregar candidatos a la eleccion primero.\n");
        esperarEnter();
        return;
    }

    printf("Ingrese ID numerico para la mesa (ej: 101): ");
    idMesa = ingresarOpcion();
    
    printf("Ingrese Comuna: ");
    fgets(comuna, sizeof(comuna), stdin);
    comuna[strcspn(comuna, "\n")] = 0;
    
    printf("Ingrese Region: ");
    fgets(region, sizeof(region), stdin);
    region[strcspn(region, "\n")] = 0;

    nuevoNodo = crearNodoMesa(idMesa, comuna, region, eleccionActual->numcandidatos);
    if (nuevoNodo == NULL) {
        printf("Error en la creacion del nodo.\n");
        esperarEnter();
        return;
    }

    eleccionActual->raizarbolmesas = insertarMesaRec(eleccionActual->raizarbolmesas, nuevoNodo);
    
    printf("\n¡Mesa %d (Comuna: %s) agregada exitosamente!\n", idMesa, comuna);
    esperarEnter();
}

/* Busca una mesa y reporta el resultado al usuario */
struct NodoMesa* buscarMesa(struct NodoEleccion *eleccionActual, int idBuscada) {
    struct NodoMesa* mesaEncontrada;

    limpiarPantalla();
    printf("--- Buscando Mesa ID: %d ---\n", idBuscada);
    
    mesaEncontrada = buscarMesaRec(eleccionActual->raizarbolmesas, idBuscada);

    if (mesaEncontrada != NULL) {
        printf("¡Mesa encontrada!\n");
        printf("  Comuna: %s\n", mesaEncontrada->comuna);
        printf("  Region: %s\n", mesaEncontrada->region);
        printf("  Votos emitidos: %d\n", mesaEncontrada->votosemitidos);
    } else {
        printf("Error: Mesa con ID %d no encontrada.\n", idBuscada);
    }
    
    esperarEnter();
    return mesaEncontrada;
}

/* Busca una mesa por ID y permite al usuario cambiar su comuna o region */
int modificarMesa(struct NodoEleccion *eleccionActual, int idModificar) {
    struct NodoMesa* mesa;
    int opcion;
    char buffer[100];

    limpiarPantalla();
    printf("--- Modificar Mesa ID: %d ---\n", idModificar);

    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idModificar);

    if (mesa == NULL) {
        printf("Error: Mesa con ID %d no encontrada.\n", idModificar);
        esperarEnter();
        return 0;
    }

    printf("Mesa encontrada. Datos actuales:\n");
    printf(" 1. Comuna: %s\n", mesa->comuna);
    printf(" 2. Region: %s\n", mesa->region);
    printf(" 0. Cancelar\n");
    printf("\n¿Que desea modificar?\n");
    opcion = ingresarOpcion();

    switch (opcion) {
        case 1:
            printf("Ingrese nueva Comuna: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(mesa->comuna, buffer);
            printf("¡Comuna actualizada!\n");
            break;
        case 2:
            printf("Ingrese nueva Region: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(mesa->region, buffer);
            printf("¡Region actualizada!\n");
            break;
        case 0:
            printf("Modificacion cancelada.\n");
            break;
        default:
            printf("Opcion no valida.\n");
            break;
    }
    
    esperarEnter();
    return 1;
}

/* Llama a la funcion recursiva para eliminar una mesa del arbol */
int eliminarMesa(struct NodoEleccion *eleccionActual, int idEliminar) {
    struct NodoMesa* nodoCheck;

    limpiarPantalla();
    printf("--- Eliminar Mesa ID: %d ---\n", idEliminar);

    nodoCheck = buscarMesaRec(eleccionActual->raizarbolmesas, idEliminar);
    if (nodoCheck == NULL) {
        printf("Error: No se puede eliminar. La mesa %d no existe.\n", idEliminar);
        esperarEnter();
        return 0;
    }

    eleccionActual->raizarbolmesas = eliminarMesaRec(eleccionActual->raizarbolmesas, idEliminar);
    
    printf("¡Mesa %d eliminada del arbol!\n", idEliminar);
    printf("(Nota: La memoria del nodo no se libera, solo se desenlaza).\n");
    
    esperarEnter();
    return 1;
}

/* Muestra un listado de todas las mesas ordenadas por ID */
void listarMesas(struct NodoEleccion *eleccionActual) {
    limpiarPantalla();
    printf("--- Listado de Mesas (Ordenadas por ID) ---\n");

    if (eleccionActual == NULL || eleccionActual->raizarbolmesas == NULL) {
        printf(" (No hay mesas registradas en esta eleccion)\n");
    } else {
        listarMesasRec(eleccionActual->raizarbolmesas);
    }
    
    esperarEnter();
}


/* Muestra el submenu para la gestion de mesas */
void menuGestionMesas(struct NodoEleccion *eleccionActual) {
    int opcion;
    int idMesa;

    if (eleccionActual == NULL) {
        printf("Error: No existe una eleccion activa.\n");
        esperarEnter();
        return;
    }
    
    opcion = 0;
    
    do {
        limpiarPantalla();
        printf("\n...:: Gestion de Mesas de Votacion (Vuelta %d) ::...\n", eleccionActual->numerovuelta);
        printf(" 1. Agregar Mesa\n");
        printf(" 2. Buscar Mesa por ID\n");
        printf(" 3. Modificar Mesa\n");
        printf(" 4. Eliminar Mesa\n");
        printf(" 5. Listar Todas las Mesas (Ordenadas)\n");
        printf(" 0. Volver al Menu Principal\n");
        
        opcion = ingresarOpcion();

        switch (opcion) {
            case 1:
                agregarMesa(eleccionActual);
                break;
            case 2:
                printf("Ingrese ID de la mesa a BUSCAR: ");
                idMesa = ingresarOpcion();
                buscarMesa(eleccionActual, idMesa); 
                break;
            case 3:
                printf("Ingrese ID de la mesa a MODIFICAR: ");
                idMesa = ingresarOpcion();
                modificarMesa(eleccionActual, idMesa);
                break;
            case 4:
                printf("Ingrese ID de la mesa a ELIMINAR: ");
                idMesa = ingresarOpcion();
                eliminarMesa(eleccionActual, idMesa);
                break;
            case 5:
                listarMesas(eleccionActual);
                break;
            case 0:
                printf("Volviendo al menu principal...\n");
                break;
            default:
                printf("Opcion no valida.\n");
                esperarEnter();
        }
    } while (opcion != 0);
}