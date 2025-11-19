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
    char pais[50];
    char nacionalidad[50];
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
    long votos_nulos;
    long votos_blancos;
    struct NodoVoto *headlistavotos;
    struct NodoMesa *izq;
    struct NodoMesa *der;
};

struct Candidato {
    int idcandidato;
    char nombre[100];
    char partido[50];
    int libre;
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

void inicializarPoolCandidatos() {
    int i;
    plibre = 0;
    for (i = 0; i < MAXCANDIDATOS; i++) {
        poolCandidatos[i].libre = 1;
        poolCandidatos[i].idcandidato = -1;
    }
}

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
void menuGestionVotantes(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual);
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
    inicializarPoolCandidatos();
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
                menuGestionVotantes(&sistema, eleccionActual);;
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
/* ----------------------------------------------------------------- */
/* --- IMPLEMENTACION GESTION DE VOTANTES --- */
/* ----------------------------------------------------------------- */

/*  --- Funciones Internas de la Lista Doble --- */

/* Funcion interna: Crea un nuevo NodoVotante y asigna memoria */
struct NodoVotante* crearNodoVotante(char* rut, char* nombre,char* nacionalidad,char *pais,char *region, char* comuna) {
    struct NodoVotante* nuevo;

    /* 1. Pedir memoria */
    nuevo = (struct NodoVotante*)malloc(sizeof(struct NodoVotante));
    if (nuevo == NULL) {
        printf("ERROR FATAL: No hay memoria para crear el votante.\n");
        return NULL;
    }

    /* 2. Llenar los datos */
    strcpy(nuevo->rut, rut);
    strcpy(nuevo->nombre, nombre);
    strcpy(nuevo->nacionalidad, nacionalidad);
    strcpy(nuevo->pais, pais);
    strcpy(nuevo->region, region);
    strcpy(nuevo->comuna, comuna);
    nuevo->havotado = 0;

    /* 3. Inicializar punteros */
    nuevo->mesaasignada = NULL;
    nuevo->sig = NULL;
    nuevo->ant = NULL;

    return nuevo;
}

/*  --- Funciones Publicas (Llamadas por el Menu) --- */

/* Pide datos y agrega un nuevo votante al final de la lista doble */
void agregarVotante(struct SistemaElectoral *sistema) {
    char rutSocio[100];
    char rutLimpio[11];
    char nombre[100];
    char nacionalidad[50]; 
    char pais[50];
    char comuna[50];
    char region[50];
    struct NodoVotante* nuevoNodo;
    struct NodoVotante* votanteExistente;
    int esChileno;

    limpiarPantalla();
    printf("--- Agregar Votante al Registro Electoral ---\n");

    /* 1. Validacion de Nacionalidad  */
    printf("¿El votante posee nacionalidad Chilena? (1: Si / 0: No): ");
    esChileno = ingresarOpcion();

    if (esChileno != 1) {
        printf("Error: Segun la normativa, debe cumplir requisitos de nacionalidad chilena.\n");
        printf("Registro cancelado.\n");
        esperarEnter();
        return;
    }
    strcpy(nacionalidad, "Chilena");

    /* 2. Pedir RUT y normalizarlo */
    printf("Ingrese RUT (ej: 21.776.530-3): ");
    fgets(rutSocio, sizeof(rutSocio), stdin);
    rutSocio[strcspn(rutSocio, "\n")] = 0;
    normalizarRUT(rutLimpio, rutSocio);

    /* Verificar duplicados */
    votanteExistente = buscarVotante(sistema, rutLimpio);
    if (votanteExistente != NULL) {
        printf("\nError: El RUT %s ya existe en el registro.\n", rutLimpio);
        esperarEnter();
        return;
    }

    /* 3. Pedir datos Personales */
    /*printf("RUT normalizado: %s\n", rutLimpio); 
    
    printf("Ingrese Comuna: ");
    fgets(comuna, sizeof(comuna), stdin);
    comuna[strcspn(comuna, "\n")] = 0;

    */
    
    printf("Ingrese Nombre Completo: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;

    /* 4. Gestion de Residencia (Chile vs Extranjero)  */
    printf("Ingrese Pais de Residencia (Escriba 'Chile' si reside en el pais): ");
    fgets(pais, sizeof(pais), stdin);
    pais[strcspn(pais, "\n")] = 0;
    
    if (strcmp(pais, "Chile") == 0 || strcmp(pais, "chile") == 0) {
        printf("Ingrese Region: ");
        fgets(region, sizeof(region), stdin);
        region[strcspn(region, "\n")] = 0;
        
        printf("Ingrese Comuna: ");
        fgets(comuna, sizeof(comuna), stdin);
        comuna[strcspn(comuna, "\n")] = 0;
    } else {
        /* Si es extranjero, usamos region/comuna para ciudad/estado */
        printf("Votante en el Extranjero detectado.\n");
        printf("Ingrese Ciudad de Residencia (se guardara como Comuna): ");
        fgets(comuna, sizeof(comuna), stdin);
        comuna[strcspn(comuna, "\n")] = 0;
        
        printf("Ingrese Estado/Provincia (se guardara como Region): ");
        fgets(region, sizeof(region), stdin);
        region[strcspn(region, "\n")] = 0;
    }

    /* 5. Crear el nodo */
    nuevoNodo = crearNodoVotante(rutLimpio, nombre, nacionalidad, pais, region, comuna);
    if (nuevoNodo == NULL) return;
    
    strcpy(nuevoNodo->pais, pais);
    strcpy(nuevoNodo->nacionalidad, nacionalidad);
    strcpy(nuevoNodo->region, region); 

    /* 6. Insertar al final de la Lista Doblemente Enlazada */
    nuevoNodo->ant = sistema->tailregistroelectoral;

    if (sistema->headregistroelectoral == NULL) {
        /* La lista esta vacia */
        sistema->headregistroelectoral = nuevoNodo;
        sistema->tailregistroelectoral = nuevoNodo;
    } else {
        /* Agregar al final */
        sistema->tailregistroelectoral->sig = nuevoNodo;
        sistema->tailregistroelectoral = nuevoNodo;
    }

    printf("\n¡Votante agregado exitosamente!\n");
    if (strcmp(pais, "Chile") != 0) {
        printf("(Registrado como votante en el extranjero: %s)\n", pais);
    }
    esperarEnter();
}

/* Busca un votante por RUT en la Lista Doble */
struct NodoVotante* buscarVotante(struct SistemaElectoral *sistema, char* rutBuscado) {
    struct NodoVotante* actual;

    actual = sistema->headregistroelectoral;

    /* Recorrer la lista desde el inicio (head) */
    while (actual != NULL) {
        if (strcmp(actual->rut, rutBuscado) == 0) {
            return actual;
        }
        actual = actual->sig;
    }

    return NULL;
}

/* Permite al usuario cambiar datos de un votante encontrado */
int modificarVotante(struct SistemaElectoral *sistema, char* rutModificar) {
    struct NodoVotante* votante;
    int opcion;
    char buffer[100];

    /* 1. Buscar al votante */
    votante = buscarVotante(sistema, rutModificar);
    if (votante == NULL) {
        printf("Error: Votante con RUT %s no encontrado.\n", rutModificar);
        esperarEnter();
        return 0;
    }

    /* 2. Mostrar menu de modificacion */
    limpiarPantalla();
    printf("--- Modificar Votante ---\n");
    printf("Votante encontrado: %s (RUT: %s)\n", votante->nombre, votante->rut);
    printf("\n¿Que desea modificar?\n");
    printf(" 1. Nombre (Actual: %s)\n", votante->nombre);
    printf(" 2. Comuna (Actual: %s)\n", votante->comuna);
    printf(" 0. Cancelar\n");

    opcion = ingresarOpcion();

    switch (opcion) {
        case 1:
            printf("Ingrese nuevo Nombre: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(votante->nombre, buffer);
            printf("¡Nombre actualizado!\n");
            break;
        case 2:
            printf("Ingrese nueva Comuna: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(votante->comuna, buffer);
            printf("¡Comuna actualizada!\n");
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

/* Elimina un votante de la Lista Doble */
int eliminarVotante(struct SistemaElectoral *sistema, char* rutEliminar) {
    struct NodoVotante* votante;

    /* 1. Buscar al votante */
    votante = buscarVotante(sistema, rutEliminar);
    if (votante == NULL) {
        printf("Error: Votante con RUT %s no encontrado.\n", rutEliminar);
        esperarEnter();
        return 0;
    }

    /* 2. Logica de re-enlace de Lista Doble */

    /* Caso A: Es el NODO HEAD */
    if (votante->ant == NULL) {
        sistema->headregistroelectoral = votante->sig;
        if (sistema->headregistroelectoral != NULL) {
            /* Aun quedan nodos, actualizar el 'ant' del nuevo head */
            sistema->headregistroelectoral->ant = NULL;
        } else {
            /* La lista quedo vacia, actualizar el 'tail' */
            sistema->tailregistroelectoral = NULL;
        }
    }
    /* Caso B: Es el NODO TAIL */
    else if (votante->sig == NULL) {
        sistema->tailregistroelectoral = votante->ant;
        sistema->tailregistroelectoral->sig = NULL;
    }
    /* Caso C: Es un NODO INTERMEDIO */
    else {
        votante->ant->sig = votante->sig;
        votante->sig->ant = votante->ant;
    }

    /* (El nodo 'votante' queda en memoria, pero desenlazado) */
    printf("¡Votante %s (RUT: %s) eliminado del registro!\n", votante->nombre, votante->rut);
    esperarEnter();
    return 1; /* Exito */
}

/* Muestra todos los votantes en la Lista Doble */
void listarVotantes(struct SistemaElectoral *sistema) {
    struct NodoVotante* actual;
    int i;

    limpiarPantalla();
    printf("--- Listado del Registro Electoral (Padron) ---\n");

    actual = sistema->headregistroelectoral;

    if (actual == NULL) {
        printf(" (El registro electoral esta vacio)\n");
    }

    i = 1;
    while (actual != NULL) {
        printf(" %d. RUT: %-11s | Nombre: %-30s | Comuna: %s\n",
               i, actual->rut, actual->nombre, actual->comuna);

        /* Imprimir mesa asignada (si la tiene) */
        if (actual->mesaasignada != NULL) {
            printf("     -> Mesa Asignada: ID %d (%s)\n",
                   actual->mesaasignada->idmesa,
                   actual->mesaasignada->comuna);
        } else {
            printf("     -> (Mesa aun no asignada)\n");
        }

        actual = actual->sig;
        i++;
    }

    esperarEnter();
}

/* Asigna un Votante (de la lista global) a una Mesa (del arbol de la eleccion) */
int asignarMesaAVotante(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual) {
    char rutSocio[100];
    char rutLimpio[11];
    int idMesa;
    struct NodoVotante* votante;
    struct NodoMesa* mesa; /* (Necesita 'buscarMesaRec') */

    limpiarPantalla();
    printf("--- Asignar Votante a Mesa de Votacion ---\n");

    /* 1. Validar que existan mesas */
    if (eleccionActual == NULL || eleccionActual->raizarbolmesas == NULL) {
        printf("Error: No hay mesas creadas en la eleccion actual.\n");
        printf("Primero debe agregar mesas en el Menu 3.\n");
        esperarEnter();
        return 0;
    }

    /* 2. Pedir y buscar al Votante */
    printf("Ingrese RUT del votante a asignar: ");
    fgets(rutSocio, sizeof(rutSocio), stdin);
    rutSocio[strcspn(rutSocio, "\n")] = 0;
    normalizarRUT(rutLimpio, rutSocio);

    votante = buscarVotante(sistema, rutLimpio);
    if (votante == NULL) {
        printf("Error: Votante con RUT %s no encontrado.\n", rutLimpio);
        esperarEnter();
        return 0;
    }

    /* 3. Pedir y buscar la Mesa */
    printf("Votante encontrado: %s\n", votante->nombre);
    printf("Ingrese ID de la mesa a la que sera asignado: ");
    idMesa = ingresarOpcion();


    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idMesa);
    if (mesa == NULL) {
        printf("Error: Mesa con ID %d no encontrada en esta eleccion.\n", idMesa);
        esperarEnter();
        return 0;
    }

    /* 4. Realizar la asignacion (la conexion clave) */
    votante->mesaasignada = mesa;

    printf("\n¡Exito! Votante %s (RUT: %s) ha sido asignado a la:\n", votante->nombre, votante->rut);
    printf("Mesa ID %d (Comuna: %s)\n", mesa->idmesa, mesa->comuna);
    esperarEnter();
    return 1;
}

/* Muestra el submenu para la gestion de votantes */
void menuGestionVotantes(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual) {
    int opcion;
    char rutSocio[100];
    char rutLimpio[11];
    struct NodoVotante* votanteEncontrado;

    opcion = 0;

    do {
        limpiarPantalla();
        printf("\n...:: Gestion de Registro Electoral (Votantes) ::...\n");
        printf(" 1. Agregar Votante\n");
        printf(" 2. Buscar Votante por RUT\n");
        printf(" 3. Modificar Votante por RUT\n");
        printf(" 4. Eliminar Votante por RUT\n");
        printf(" 5. Listar Todos los Votantes\n");
        printf(" 6. Asignar Votante a Mesa (Vuelta Actual)\n");
        printf(" 0. Volver al Menu Principal\n");

        opcion = ingresarOpcion();

        switch (opcion) {
            case 1:
                agregarVotante(sistema);
                break;
            case 2:
                printf("Ingrese RUT a buscar: ");
                fgets(rutSocio, sizeof(rutSocio), stdin);
                rutSocio[strcspn(rutSocio, "\n")] = 0;
                normalizarRUT(rutLimpio, rutSocio);

                votanteEncontrado = buscarVotante(sistema, rutLimpio);

                limpiarPantalla();
                printf("--- Resultado de la Busqueda ---\n");
                if (votanteEncontrado != NULL) {
                    printf("Votante encontrado:\n");
                    printf("  RUT: %s\n", votanteEncontrado->rut);
                    printf("  Nombre: %s\n", votanteEncontrado->nombre);
                    printf("  Comuna: %s\n", votanteEncontrado->comuna);
                } else {
                    printf("Votante con RUT %s no encontrado.\n", rutLimpio);
                }
                esperarEnter();
                break;
            case 3:
                printf("Ingrese RUT a modificar: ");
                fgets(rutSocio, sizeof(rutSocio), stdin);
                rutSocio[strcspn(rutSocio, "\n")] = 0;
                normalizarRUT(rutLimpio, rutSocio);

                modificarVotante(sistema, rutLimpio);
                break;
            case 4:
                printf("Ingrese RUT a eliminar: ");
                fgets(rutSocio, sizeof(rutSocio), stdin);
                rutSocio[strcspn(rutSocio, "\n")] = 0;
                normalizarRUT(rutLimpio, rutSocio);

                eliminarVotante(sistema, rutLimpio);
                break;
            case 5:
                listarVotantes(sistema);
                break;
            case 6:
                /* Ahora 'eleccionActual' esta disponible gracias al parametro */
                asignarMesaAVotante(sistema, eleccionActual);
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
/* ----------------------------------------------------------------- */
/* --- IMPLEMENTACION GESTION DE CANDIDATURAS --- */
/* ----------------------------------------------------------------- */

/* --- Submenu de Candidatos --- */
void menuGestionCandidatos(struct NodoEleccion *eleccionActual) {
    int opcion;
    int idCandidato;
    struct Candidato *candEncontrado;

    if (eleccionActual == NULL) {
       printf("Error: No existe una eleccion activa.\n");
       esperarEnter();
       return;
    }

    opcion = 0;
    do {
       limpiarPantalla();
       printf("\n...:: Gestion de Candidaturas (Vuelta %d) ::...\n", eleccionActual->numerovuelta);
       printf(" Candidatos Registrados: %d/%d\n", eleccionActual->numcandidatos, MAXCANDIDATOS);
       printf("------------------------------------------------\n");
       printf(" 1. Agregar Candidato\n");
       printf(" 2. Buscar Candidato por ID\n");
       printf(" 3. Modificar Candidato (Nombre/Partido)\n");
       printf(" 4. Eliminar Candidato\n");
       printf(" 5. Listar Todos los Candidatos\n");
       printf(" 0. Volver al Menu Principal\n");

       opcion = ingresarOpcion();

       switch (opcion) {
           case 1:
               agregarCandidato(eleccionActual);
               break;
           case 2:
           {
               printf("Ingrese ID del candidato a BUSCAR: ");
               idCandidato = ingresarOpcion();

               candEncontrado = buscarCandidato(eleccionActual, idCandidato);

               limpiarPantalla();
               if (candEncontrado != NULL) {
                   printf("\n¡Candidato encontrado (ID: %d)!\n", candEncontrado->idcandidato);
                   printf("  Nombre: %s\n", candEncontrado->nombre);
                   printf("  Partido: %s\n", candEncontrado->partido);
               } else {
                   printf("\nError: Candidato con ID %d no encontrado.\n", idCandidato);
               }

               esperarEnter();
               break;
           }
           case 3:
               printf("Ingrese ID del candidato a MODIFICAR: ");
               idCandidato = ingresarOpcion();
               modificarCandidato(eleccionActual, idCandidato);
               break;
           case 4:
               printf("Ingrese ID del candidato a ELIMINAR: ");
               idCandidato = ingresarOpcion();
               eliminarCandidato(eleccionActual, idCandidato);
               break;
           case 5:
               listarCandidatos(eleccionActual);
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

/* Busca el primer slot libre en el pool o usa plibre si es el siguiente */
void agregarCandidato(struct NodoEleccion *eleccionActual) {
    int i;
    int slotLibre;
    char nombre[100];
    char partido[50];
    int idCandidatoTemp;

    limpiarPantalla();
    printf("--- Agregar Nuevo Candidato ---\n");

    if (eleccionActual->numcandidatos >= MAXCANDIDATOS) {
       printf("Error: Maximo de %d candidatos alcanzado.\n", MAXCANDIDATOS);
       esperarEnter();
       return;
    }

    /* 1. Buscar un slot libre (reuso de slots eliminados) */
    slotLibre = -1;
    for (i = 0; i < MAXCANDIDATOS; i++) {
       /* Si el candidato esta libre (libre=1) y su ID es -1 (confirmacion extra) */
       if (poolCandidatos[i].libre == 1 && poolCandidatos[i].idcandidato == -1) {
           slotLibre = i;
           break;
       }
    }

    /* Si no encontro un slot libre (nunca se ha eliminado nada), usa el 'plibre' */
    if (slotLibre == -1) {
       slotLibre = plibre;
    }

    /* 2. Pedir datos del candidato */
    printf("Ingrese ID numerico para el candidato: ");
    /* El ID es arbitrario, no esta relacionado con la posicion en el array */
    idCandidatoTemp = ingresarOpcion();

    /* Validar que el ID no exista */
    if (buscarCandidato(eleccionActual, idCandidatoTemp) != NULL) {
       printf("\nError: Ya existe un candidato con ese ID. Cancelando operacion.\n");
       esperarEnter();
       return;
    }

    /* Si el ID es valido, se asigna */
    poolCandidatos[slotLibre].idcandidato = idCandidatoTemp;

    printf("Ingrese Nombre del Candidato: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    strcpy(poolCandidatos[slotLibre].nombre, nombre);

    printf("Ingrese Partido Politico: ");
    fgets(partido, sizeof(partido), stdin);
    partido[strcspn(partido, "\n")] = 0;
    strcpy(poolCandidatos[slotLibre].partido, partido);

    /* 3. Marcar como ocupado y actualizar contadores */
    poolCandidatos[slotLibre].libre = 0; /* Marcar como ocupado */
    eleccionActual->numcandidatos++;

    if (slotLibre == plibre) {
       plibre++;
    }

    printf("\n¡Candidato %d (%s) agregado exitosamente en el slot %d!\n",
          poolCandidatos[slotLibre].idcandidato, poolCandidatos[slotLibre].nombre, slotLibre);
    esperarEnter();
}

/* Busca un candidato en el array por ID. Devuelve el puntero al struct Candidato o NULL. */
struct Candidato* buscarCandidato(struct NodoEleccion *eleccionActual, int idBuscado) {
    int i;

    for (i = 0; i < MAXCANDIDATOS; i++) {
       /* Solo revisar los que no esten marcados como libre */
       if (poolCandidatos[i].libre == 0 && poolCandidatos[i].idcandidato == idBuscado) {
           return &poolCandidatos[i];
       }
    }

    return NULL;
}

/* Busca un candidato y permite modificar su nombre y/o partido */
int modificarCandidato(struct NodoEleccion *eleccionActual, int idModificar) {
    struct Candidato *candidato;
    int opcion;
    char buffer[100];

    limpiarPantalla();
    printf("--- Modificar Candidato ID: %d ---\n", idModificar);

    candidato = buscarCandidato(eleccionActual, idModificar);

    if (candidato == NULL) {
       printf("\nError: Candidato con ID %d no encontrado.\n", idModificar);
       esperarEnter();
       return 0;
    }

    /* Si se encontro, mostrar la info */
    printf("\n¡Candidato encontrado (ID: %d)!\n", candidato->idcandidato);
    printf("  Nombre: %s\n", candidato->nombre);
    printf("  Partido: %s\n", candidato->partido);
    esperarEnter();
    limpiarPantalla();

    printf("Candidato encontrado. Datos actuales:\n");
    printf(" ID: %d\n", candidato->idcandidato);
    printf(" 1. Nombre: %s\n", candidato->nombre);
    printf(" 2. Partido: %s\n", candidato->partido);
    printf(" 0. Cancelar\n");
    printf("\n¿Que desea modificar?\n");

    opcion = ingresarOpcion();

    switch (opcion) {
       case 1:
           printf("Ingrese nuevo Nombre: ");
           fgets(buffer, sizeof(buffer), stdin);
           buffer[strcspn(buffer, "\n")] = 0;
           strcpy(candidato->nombre, buffer);
           printf("¡Nombre actualizado!\n");
           break;
       case 2:
           printf("Ingrese nuevo Partido: ");
           fgets(buffer, sizeof(buffer), stdin);
           buffer[strcspn(buffer, "\n")] = 0;
           strcpy(candidato->partido, buffer);
           printf("¡Partido actualizado!\n");
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

/* Elimina (marca como libre) un candidato del array */
int eliminarCandidato(struct NodoEleccion *eleccionActual, int idEliminar) {
    int i;

    limpiarPantalla();
    printf("--- Eliminar Candidato ID: %d ---\n", idEliminar);

    /* 1. Buscar el slot que contiene al candidato */
    for (i = 0; i < MAXCANDIDATOS; i++) {
       if (poolCandidatos[i].libre == 0 && poolCandidatos[i].idcandidato == idEliminar) {

           /* 2. Marcar el slot como libre (Eliminar) */
           poolCandidatos[i].libre = 1;
           poolCandidatos[i].idcandidato = -1; /* ID invalido para seguridad */

           /* 3. Actualizar contadores */
           eleccionActual->numcandidatos--;

           printf("¡Candidato con ID %d eliminado (slot %d liberado)!\n", idEliminar, i);
           esperarEnter();
           return 1;
       }
    }

    printf("Error: Candidato con ID %d no encontrado. No se puede eliminar.\n", idEliminar);
    esperarEnter();
    return 0;
}

/* Muestra un listado de todos los candidatos activos */
void listarCandidatos(struct NodoEleccion *eleccionActual) {
    int i;

    limpiarPantalla();
    printf("--- Listado de Candidatos Activos (Vuelta %d) ---\n", eleccionActual->numerovuelta);
    printf(" Total: %d/%d\n", eleccionActual->numcandidatos, MAXCANDIDATOS);
    printf("--------------------------------------------------\n");

    if (eleccionActual->numcandidatos == 0) {
       printf(" (No hay candidatos registrados)\n");
       esperarEnter();
       return;
    }

    for (i = 0; i < MAXCANDIDATOS; i++) {
       /* Solo mostrar los que no esten marcados como libre */
       if (poolCandidatos[i].libre == 0) {
           printf("  -> ID: %-5d | Nombre: %-30s | Partido: %s\n",
                  poolCandidatos[i].idcandidato,
                  poolCandidatos[i].nombre,
                  poolCandidatos[i].partido);
       }
    }

    esperarEnter();
}