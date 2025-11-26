#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------- */
/* --- DEFINES --- */
/* ----------------------------------------------------------------- */
#define MAXCANDIDATOS 50 /* Limite para el pool de PLibre */
#define CLAVE_SERVEL 1234 /* clave simple para el servel */

/* ----------------------------------------------------------------- */
/* --- ESTRUCTURAS DE DATOS --- */
/* ----------------------------------------------------------------- */

/* Contiene todo el sistema, contiene a la lista simplemente enlazada de elecciones (vueltas), la lista doblemente enlazada de votantes y al arbol de mesas*/
struct SistemaElectoral {
    struct NodoEleccion *headelecciones;
    struct NodoVotante *headregistroelectoral;
    struct NodoVotante *tailregistroelectoral;
};

/* Lista simplemente enlazada de vueltas, que apunta al arreglo de candidatos y el arbol de mesas*/
struct NodoEleccion {
    int numerovuelta;
    char fecha[20];
    struct Candidato *arraycandidatos;
    int numcandidatos;
    struct NodoMesa *raizarbolmesas;
    struct NodoEleccion *sig;
};
/* Lista doblemente enlazada de votantes */
struct NodoVotante {
    char rut[11];
    char nombre[100];
    int edad;
    char pais[50];
    char nacionalidad[50];
    char comuna[50];
    char region[50];
    int havotado;
    struct NodoMesa *mesaasignada; 
    struct NodoVotante *sig;
    struct NodoVotante *ant;
};
/* Arbol binario de busqueda que contiene mesas y cada mesa tiene su lista cirular de votos*/
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
/* Arreglo de candidatos  */
struct Candidato {
    int idcandidato;
    char rut[12];
    char nombre[100];
    char partido[50];
    int edad;
    int firmasapoyo;
    int libre;
};
/* lista circular de votos */
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

/* Inicializa el pool de candidatos marcando todos como libres. */
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
void calcularRutConDV(char* rutSalida, char* rutEntrada);

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

/* Entrada al sistema Electoral, inicializa la primera vuelta presidencial y muestra el menu principal*/
int main() {
    struct SistemaElectoral sistema;
    struct NodoEleccion *primeraVuelta = NULL;
    struct NodoEleccion *eleccionActual = NULL;
    int opcion = 0;
    sistema.headelecciones = NULL;
    sistema.headregistroelectoral = NULL;
    sistema.tailregistroelectoral = NULL;

    inicializarPoolCandidatos();
    primeraVuelta = (struct NodoEleccion*)malloc(sizeof(struct NodoEleccion));
    primeraVuelta->numerovuelta = 1;
    strcpy(primeraVuelta->fecha, "19-11-2025"); 
    primeraVuelta->arraycandidatos = poolCandidatos; 
    primeraVuelta->numcandidatos = 0;
    primeraVuelta->raizarbolmesas = NULL; 
    primeraVuelta->sig = NULL;
    
    sistema.headelecciones = primeraVuelta;
    

    eleccionActual = sistema.headelecciones;
    
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
                menuGestionVotantes(&sistema, eleccionActual);;
                break;
            case 2:
                menuGestionCandidatos(eleccionActual);
                break;
            case 3:
                menuGestionMesas(eleccionActual);
                break;
            case 4:
                menuVotacion(&sistema, eleccionActual);
                break;
            case 5:
                menuOperacionesEspeciales(&sistema, eleccionActual);
                break;
            case 0:
                printf("Saliendo del sistema.\n");
                break;
            default:
                printf("Opcion no valida.\n");
                esperarEnter();
        }
        
        if (eleccionActual->sig != NULL) {
            eleccionActual = eleccionActual->sig;
        }

    } while (opcion != 0);
    return 0;
}
/* ----------------------------------------------------------------- */
/* --- IMPLEMENTACION DE FUNCIONES DE UTILIDAD --- */
/* ----------------------------------------------------------------- */

/* calcula el digito verificador del RUT */
void calcularRutConDV(char* rutSalida, char* rutEntrada) {
    int i, j, largo, len;
    int suma = 0;
    int multiplicador = 2;
    int resto, dvCalculado;
    char DigitoVer;
    char cuerpoLimpio[15];

    j = 0;
    for(i = 0; rutEntrada[i] != '\0' && j < 10; i++) {
        if(rutEntrada[i] >= '0' && rutEntrada[i] <= '9') {
            cuerpoLimpio[j++] = rutEntrada[i];
        }
    }
    cuerpoLimpio[j] = '\0';
    largo = j;

    if (largo == 0) {
        strcpy(rutSalida, "INVALIDO");
        return;
    }

    for(i = largo - 1; i >= 0; i--) {
        suma += (cuerpoLimpio[i] - '0') * multiplicador;
        multiplicador++;
        if(multiplicador > 7) multiplicador = 2;
    }

    resto = suma % 11;
    dvCalculado = 11 - resto;

    if (dvCalculado == 11) DigitoVer = '0';
    else if (dvCalculado == 10) DigitoVer = 'K';
    else DigitoVer = dvCalculado + '0';

    strcpy(rutSalida, cuerpoLimpio);
    len = strlen(rutSalida);
    rutSalida[len] = '-';
    rutSalida[len + 1] = DigitoVer;
    rutSalida[len + 2] = '\0';
}
/* Limpia la pantalla*/
void limpiarPantalla() {
    int i;
    for(i = 0; i < 30; i++) {
        printf("\n");
    }
}
/* vacia el buffer de la entrada */
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
/* muestra un mensaje solicitando que se presione Enter */
void esperarEnter() {
    printf("Presione Enter para continuar...\n");
    limpiarBuffer();
    (void)getchar();
}

int ingresarOpcion() {
    int opcion;
    char buffer[100];

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
struct NodoMesa* crearNodoMesa(int id, char* com, char* reg) {
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
    nuevo->votos_nulos = 0;
    nuevo->votos_blancos = 0;
    nuevo->headlistavotos = NULL;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    
    nuevo->conteovotos = (long*)malloc(MAXCANDIDATOS * sizeof(long));
    if (nuevo->conteovotos == NULL) {
        printf("ERROR FATAL: No hay memoria para contadores de votos.\n");
        return NULL;
    }
    
    for (i = 0; i < MAXCANDIDATOS; i++) {
        nuevo->conteovotos[i] = 0;
    }

    return nuevo;
}

/* Funcion recursiva interna: Inserta un NodoMesa en el Arbol ABB */
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

/* Funcion recursiva interna: Busca un NodoMesa en el ABB por ID */
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

/* Funcion recursiva interna: Elimina un nodo del ABB */
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
        raiz->votos_nulos = sucesor->votos_nulos;
        raiz->votos_blancos = sucesor->votos_blancos;
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
    
    if (buscarMesaRec(eleccionActual->raizarbolmesas, idMesa) != NULL) {
        printf("Error: La mesa con ID %d ya existe.\n", idMesa);
        esperarEnter();
        return;
    }

    printf("Ingrese Comuna: ");
    fgets(comuna, sizeof(comuna), stdin);
    comuna[strcspn(comuna, "\n")] = 0;
    
    printf("Ingrese Region: ");
    fgets(region, sizeof(region), stdin);
    region[strcspn(region, "\n")] = 0;

    nuevoNodo = crearNodoMesa(idMesa, comuna, region);
    if (nuevoNodo == NULL) {
        printf("Error en la creacion del nodo.\n");
        esperarEnter();
        return;
    }

    eleccionActual->raizarbolmesas = insertarMesaRec(eleccionActual->raizarbolmesas, nuevoNodo);
    
    printf("Mesa %d (Comuna: %s) agregada exitosamente!\n", idMesa, comuna);
    esperarEnter();
}

/* Busca una mesa y reporta el resultado al usuario */
struct NodoMesa* buscarMesa(struct NodoEleccion *eleccionActual, int idBuscada) {
    struct NodoMesa* mesaEncontrada;

    limpiarPantalla();
    printf("--- Buscando Mesa ID: %d ---\n", idBuscada);
    
    mesaEncontrada = buscarMesaRec(eleccionActual->raizarbolmesas, idBuscada);

    if (mesaEncontrada != NULL) {
        printf("Mesa encontrada!\n");
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
    printf("\nQue desea modificar?\n");
    opcion = ingresarOpcion();

    switch (opcion) {
        case 1:
            printf("Ingrese nueva Comuna: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(mesa->comuna, buffer);
            printf("Comuna actualizada!\n");
            break;
        case 2:
            printf("Ingrese nueva Region: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(mesa->region, buffer);
            printf("Region actualizada!\n");
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
    
    printf("Mesa %d eliminada del arbol!\n", idEliminar);
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

    nuevo = (struct NodoVotante*)malloc(sizeof(struct NodoVotante));
    if (nuevo == NULL) {
        printf("ERROR FATAL: No hay memoria para crear el votante.\n");
        return NULL;
    }

    strcpy(nuevo->rut, rut);
    strcpy(nuevo->nombre, nombre);
    strcpy(nuevo->nacionalidad, nacionalidad);
    strcpy(nuevo->pais, pais);
    strcpy(nuevo->region, region);
    strcpy(nuevo->comuna, comuna);
    nuevo->havotado = 0;

    nuevo->mesaasignada = NULL;
    nuevo->sig = NULL;
    nuevo->ant = NULL;

    return nuevo;
}

/*  --- Funciones Publicas (Llamadas por el Menu) --- */

/* Pide datos y agrega un nuevo votante al final de la lista doble */
void agregarVotante(struct SistemaElectoral *sistema) {
    char rutSucio[100];
    char rutLimpio[11];
    char nombre[100];
    char nacionalidad[50]; 
    char pais[50];
    char comuna[50];
    char region[50];
    int votanteEdad;
    int esChileno;
    int opResidencia;
    struct NodoVotante* nuevoNodo;
    struct NodoVotante* votanteExistente;

    limpiarPantalla();
    printf("--- Agregar Votante al Registro Electoral ---\n");

    printf("El votante posee nacionalidad Chilena? (1: Si / 0: No): ");
    esChileno = ingresarOpcion();

    if (esChileno != 1) {
        printf("Error: Segun la normativa, debe cumplir requisitos de nacionalidad chilena.\n");
        printf("Registro cancelado.\n");
        esperarEnter();
        return;
    }
    strcpy(nacionalidad, "Chilena");

    printf("Ingrese EDAD del votante: ");
    votanteEdad = ingresarOpcion();
    if (votanteEdad < 18) {
        printf("Error: El votante debe tener 18 años o mas para participar.\n");
        esperarEnter();
        return; 
    }

    printf("Ingrese RUT (Solo numeros, SIN puntos ni digito verificador): ");
    fgets(rutSucio, sizeof(rutSucio), stdin);
    rutSucio[strcspn(rutSucio, "\n")] = 0;
    
    calcularRutConDV(rutLimpio, rutSucio);
    printf("-> RUT Calculado: %s\n", rutLimpio);

    votanteExistente = buscarVotante(sistema, rutLimpio);
    if (votanteExistente != NULL) {
        printf("Error: Este RUT ya esta registrado.\n");
        esperarEnter();
        return;
    }
    
    printf("Ingrese Nombre Completo: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;

    do {
        printf("Reside en Chile actualmente? (1: Si / 0: No): ");
        opResidencia = ingresarOpcion();
        if (opResidencia != 0 && opResidencia != 1) {
            printf("Opcion no valida. Ingrese 1 o 0.\n");
        }
    } while (opResidencia != 0 && opResidencia != 1);

    switch (opResidencia) {
        case 1:
            strcpy(pais, "Chile");
            
            printf("Ingrese Region: ");
            fgets(region, sizeof(region), stdin);
            region[strcspn(region, "\n")] = 0;
            
            printf("Ingrese Comuna: ");
            fgets(comuna, sizeof(comuna), stdin);
            comuna[strcspn(comuna, "\n")] = 0;
            break;
            
        case 0:
            printf("Ingrese Pais de Residencia: ");
            fgets(pais, sizeof(pais), stdin);
            pais[strcspn(pais, "\n")] = 0;
            
            printf("Ingrese Estado/Provincia (se guardara como Region): ");
            fgets(region, sizeof(region), stdin);
            region[strcspn(region, "\n")] = 0;

            printf("Ingrese Ciudad de Residencia (se guardara como Comuna): ");
            fgets(comuna, sizeof(comuna), stdin);
            comuna[strcspn(comuna, "\n")] = 0;
            break;
    }

    nuevoNodo = crearNodoVotante(rutLimpio, nombre, nacionalidad, pais, region, comuna);
    if (nuevoNodo == NULL) return;
    
    nuevoNodo->edad = votanteEdad;
    nuevoNodo->havotado = 0;
    
    nuevoNodo->ant = sistema->tailregistroelectoral;

    if (sistema->headregistroelectoral == NULL) {
        sistema->headregistroelectoral = nuevoNodo;
        sistema->tailregistroelectoral = nuevoNodo;
    } else {
        sistema->tailregistroelectoral->sig = nuevoNodo;
        sistema->tailregistroelectoral = nuevoNodo;
    }

    printf("Votante agregado exitosamente\n");
    if (strcmp(pais, "Chile") != 0) {
        printf("(Registrado como votante en el extranjero: %s)\n", pais);
    }
    esperarEnter();
}

/* Busca un votante por RUT en la Lista Doble */
struct NodoVotante* buscarVotante(struct SistemaElectoral *sistema, char* rutBuscado) {
    struct NodoVotante* actual;

    actual = sistema->headregistroelectoral;

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

    votante = buscarVotante(sistema, rutModificar);
    if (votante == NULL) {
        printf("Error: Votante con RUT %s no encontrado.\n", rutModificar);
        esperarEnter();
        return 0;
    }

    limpiarPantalla();
    printf("--- Modificar Votante ---\n");
    printf("Votante encontrado: %s (RUT: %s)\n", votante->nombre, votante->rut);
    printf("\nQue desea modificar?\n");
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
            printf("Nombre actualizado!\n");
            break;
        case 2:
            printf("Ingrese nueva Comuna: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(votante->comuna, buffer);
            printf("Comuna actualizada!\n");
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

    votante = buscarVotante(sistema, rutEliminar);
    if (votante == NULL) {
        printf("Error: Votante con RUT %s no encontrado.\n", rutEliminar);
        esperarEnter();
        return 0;
    }

    if (votante->ant == NULL) {
        sistema->headregistroelectoral = votante->sig;
        if (sistema->headregistroelectoral != NULL) {
            sistema->headregistroelectoral->ant = NULL;
        } else {
            sistema->tailregistroelectoral = NULL;
        }
    }
    else if (votante->sig == NULL) {
        sistema->tailregistroelectoral = votante->ant;
        sistema->tailregistroelectoral->sig = NULL;
    }
    else {
        votante->ant->sig = votante->sig;
        votante->sig->ant = votante->ant;
    }

    printf("Votante %s (RUT: %s) eliminado del registro!\n", votante->nombre, votante->rut);
    esperarEnter();
    return 1;
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
    char rutSucio[100];
    char rutLimpio[11];
    int idMesa;
    struct NodoVotante* votante;
    struct NodoMesa* mesa; 

    limpiarPantalla();
    printf("--- Asignar Votante a Mesa de Votacion ---\n");

    if (eleccionActual == NULL || eleccionActual->raizarbolmesas == NULL) {
        printf("Error: No hay mesas creadas en la eleccion actual.\n");
        printf("Primero debe agregar mesas en el Menu 3.\n");
        esperarEnter();
        return 0;
    }

    printf("\nIngrese RUT del votante a asignar (Sin Digito Verificador): ");
    fgets(rutSucio, sizeof(rutSucio), stdin);
    rutSucio[strcspn(rutSucio, "\n")] = 0;
    calcularRutConDV(rutLimpio, rutSucio);
    votante = buscarVotante(sistema, rutLimpio);
    
    if (votante == NULL) {
        printf("Error: Votante con RUT %s no encontrado.\n", rutLimpio);
        esperarEnter();
        return 0;
    }
    if (votante->mesaasignada != NULL) {
        printf("Error: El votante %s ya tiene asignada la Mesa %d.\n", 
               votante->nombre, votante->mesaasignada->idmesa);
        esperarEnter();
        return 0;
    }

    printf("Votante encontrado: %s\n", votante->nombre);
    printf("Ingrese ID de la mesa a la que sera asignado: ");
    idMesa = ingresarOpcion();


    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idMesa);
    if (mesa == NULL) {
        printf("Error: Mesa con ID %d no encontrada en esta eleccion.\n", idMesa);
        esperarEnter();
        return 0;
    }

    votante->mesaasignada = mesa;

    printf("Exito! Votante %s (RUT: %s) ha sido asignado a la:\n", votante->nombre, votante->rut);
    printf("Mesa ID %d (Comuna: %s)\n", mesa->idmesa, mesa->comuna);
    esperarEnter();
    return 1;
}

/* Muestra el submenu para la gestion de votantes */
void menuGestionVotantes(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual) {
    int opcion;
    char rutSucio[100];
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
                printf("Ingrese RUT a buscar (Sin Digito Verificador): ");
                fgets(rutSucio, sizeof(rutSucio), stdin);
                rutSucio[strcspn(rutSucio, "\n")] = 0;
                calcularRutConDV(rutLimpio, rutSucio);
                
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
                printf("Ingrese RUT a Modificar (Sin Digito Verificador): ");
                fgets(rutSucio, sizeof(rutSucio), stdin);
                rutSucio[strcspn(rutSucio, "\n")] = 0;
                calcularRutConDV(rutLimpio, rutSucio);
                
                modificarVotante(sistema, rutLimpio);
                break;
            case 4:
                printf("Ingrese RUT a Eliminar (Sin Digito Verificador): ");
                fgets(rutSucio, sizeof(rutSucio), stdin);
                rutSucio[strcspn(rutSucio, "\n")] = 0;
                calcularRutConDV(rutLimpio, rutSucio);

                eliminarVotante(sistema, rutLimpio);
                break;
            case 5:
                listarVotantes(sistema);
                break;
            case 6:
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
                   printf("\nCandidato encontrado (ID: %d)!\n", candEncontrado->idcandidato);
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
    char rutSucio[100];
    char partido[50];
    int idCandidatoTemp;
    int candidatoEdad;
    int firmas;

    limpiarPantalla();
    printf("--- Agregar Nuevo Candidato ---\n");

    if (eleccionActual->numcandidatos >= MAXCANDIDATOS) {
       printf("Error: Maximo de %d candidatos alcanzado.\n", MAXCANDIDATOS);
       esperarEnter();
       return;
    }

    printf("Ingrese EDAD del candidato: ");
    candidatoEdad = ingresarOpcion();
    if (candidatoEdad < 35) {
        printf("Error: El candidato presidencial debe tener 35 años o mas.\n");
        esperarEnter();
        return;
    }

    slotLibre = -1;
    for (i = 0; i < MAXCANDIDATOS; i++) {
       if (poolCandidatos[i].libre == 1 && poolCandidatos[i].idcandidato == -1) {
           slotLibre = i;
           break;
       }
    }

    if (slotLibre == -1) {
       slotLibre = plibre;
    }

    printf("Ingrese ID numerico para el candidato: ");
    idCandidatoTemp = ingresarOpcion();

    if (buscarCandidato(eleccionActual,idCandidatoTemp) != NULL) {
       printf("\nError: Ya existe un candidato con ese ID. Cancelando operacion.\n");
       esperarEnter();
       return;
    }

    printf("Ingrese RUT del Candidato (Sin Digito Verificador): ");
    fgets(rutSucio, sizeof(rutSucio), stdin);
    rutSucio[strcspn(rutSucio, "\n")] = 0;
    
    calcularRutConDV(poolCandidatos[slotLibre].rut, rutSucio);

    printf("Ingrese Firmas de Apoyo (0 si postula por partido): ");
    firmas = ingresarOpcion();
    if (firmas == 0) {
        printf("ADVERTENCIA: Candidato independiente debe tener firmas (asumiendo que este es de partido).\n");
    }

    poolCandidatos[slotLibre].idcandidato = idCandidatoTemp;
    poolCandidatos[slotLibre].edad = candidatoEdad; 
    poolCandidatos[slotLibre].firmasapoyo = firmas;

    printf("Ingrese Nombre del Candidato: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    strcpy(poolCandidatos[slotLibre].nombre, nombre);

    printf("Ingrese Partido Politico: ");
    fgets(partido, sizeof(partido), stdin);
    partido[strcspn(partido, "\n")] = 0;
    strcpy(poolCandidatos[slotLibre].partido, partido);

    poolCandidatos[slotLibre].libre = 0;
    eleccionActual->numcandidatos++;

    if (slotLibre == plibre) {
       plibre++;
    }

    printf("\nCandidato %d (Nombre: %s) agregado exitosamente en el slot %d!\n",
          poolCandidatos[slotLibre].idcandidato, poolCandidatos[slotLibre].nombre, slotLibre);
    esperarEnter();
}

/* Busca un candidato en el array por ID. Devuelve el puntero al struct Candidato o NULL. */
struct Candidato* buscarCandidato(struct NodoEleccion *eleccionActual,int idBuscado) {
    int i;
    
    printf("  (Buscando en los registros de la Vuelta %d...)\n", eleccionActual->numerovuelta);
    
    for (i = 0; i < MAXCANDIDATOS; i++) {
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

    candidato = buscarCandidato(eleccionActual,idModificar);

    if (candidato == NULL) {
       printf("\nError: Candidato con ID %d no encontrado.\n", idModificar);
       esperarEnter();
       return 0;
    }

    printf("Candidato encontrado (ID: %d)!\n", candidato->idcandidato);
    printf("  Nombre: %s\n", candidato->nombre);
    printf("  Partido: %s\n", candidato->partido);
    esperarEnter();
    limpiarPantalla();

    printf("Candidato encontrado. Datos actuales:\n");
    printf(" ID: %d\n", candidato->idcandidato);
    printf(" 1. Nombre: %s\n", candidato->nombre);
    printf(" 2. Partido: %s\n", candidato->partido);
    printf(" 0. Cancelar\n");
    printf("Que desea modificar?\n");

    opcion = ingresarOpcion();

    switch (opcion) {
       case 1:
           printf("Ingrese nuevo Nombre: ");
           fgets(buffer, sizeof(buffer), stdin);
           buffer[strcspn(buffer, "\n")] = 0;
           strcpy(candidato->nombre, buffer);
           printf("Nombre actualizado!\n");
           break;
       case 2:
           printf("Ingrese nuevo Partido: ");
           fgets(buffer, sizeof(buffer), stdin);
           buffer[strcspn(buffer, "\n")] = 0;
           strcpy(candidato->partido, buffer);
           printf("Partido actualizado!\n");
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

    for (i = 0; i < MAXCANDIDATOS; i++) {
       if (poolCandidatos[i].libre == 0 && poolCandidatos[i].idcandidato == idEliminar) {

           poolCandidatos[i].libre = 1;
           poolCandidatos[i].idcandidato = -1;

           eleccionActual->numcandidatos--;

           printf("Candidato con ID %d eliminado (slot %d liberado)!\n", idEliminar, i);
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
    int i, j, count;
    int limiteRecorrido;
    struct Candidato arregloTemp[MAXCANDIDATOS];
    struct Candidato aux;

    limpiarPantalla();
    printf("--- Listado de Candidatos Activos (Vuelta %d) ---\n", eleccionActual->numerovuelta);
    printf(" (Ordenados por ID)\n");
    printf("--------------------------------------------------\n");
    
    if (eleccionActual->numcandidatos == 0) {
       printf(" (No hay candidatos registrados)\n");
       esperarEnter();
       return;
    }

    if (eleccionActual->numerovuelta == 1) {
        limiteRecorrido = MAXCANDIDATOS;
    } else {
        limiteRecorrido = eleccionActual->numcandidatos;
    }
    count = 0;
    for (i = 0; i < limiteRecorrido; i++) {
        if (eleccionActual->arraycandidatos[i].libre == 0) {
            arregloTemp[count] = eleccionActual->arraycandidatos[i];
            count++;
        }
    }

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (arregloTemp[j].idcandidato > arregloTemp[j+1].idcandidato) {
                aux = arregloTemp[j];
                arregloTemp[j] = arregloTemp[j+1];
                arregloTemp[j+1] = aux;
            }
        }
    }
    
    for (i = 0; i < count; i++) {
        printf("  -> ID: %-5d | Nombre: %-30s | Partido: %s\n",
               arregloTemp[i].idcandidato, 
               arregloTemp[i].nombre, 
               arregloTemp[i].partido);
    }

    esperarEnter();
}

/* ----------------------------------------------------------------- */
/* --- VOTACION Y ESCRUTINIO --- */
/* ----------------------------------------------------------------- */

/* --- Funciones Auxiliares  --- */

/* Mapea el ID de un candidato al indice localdel arreglo conteovotos */
int obtenerIndiceLocal(struct NodoEleccion *eleccionActual, int idCandidato) {
    int i;
    for (i = 0; i < MAXCANDIDATOS; i++) {
         if (eleccionActual->arraycandidatos[i].libre == 0 && eleccionActual->arraycandidatos[i].idcandidato == idCandidato) {
             return i;
         }
    }
    return -1; 
}

/* Crea un nuevo NodoVoto y asigna memoria */
struct NodoVoto* crearNodoVoto(char* rut, int idcandidato) {
    struct NodoVoto *nuevo;

    nuevo = (struct NodoVoto*)malloc(sizeof(struct NodoVoto));
    if (nuevo == NULL) {
        printf("ERROR: No hay memoria para crear el nodo de voto.\n");
        return NULL;
    }

    strcpy(nuevo->rutvotante, rut);
    nuevo->idcandidatovotado = idcandidato;

    nuevo->sig = NULL; 

    return nuevo;
}

/* Inserta el nuevo voto en la Lista Circular de la Mesa */
void insertarVoto(struct NodoMesa *mesa, struct NodoVoto *nuevoVoto) {
    struct NodoVoto **pTail;

    pTail = &(mesa->headlistavotos);

    if (*pTail == NULL) {
        nuevoVoto->sig = nuevoVoto; 
        *pTail = nuevoVoto;
    } else {
        nuevoVoto->sig = (*pTail)->sig;
        
        (*pTail)->sig = nuevoVoto;
        
        *pTail = nuevoVoto;
    }
}

/* Busca un voto en la lista circular de una mesa */
struct NodoVoto* buscarVoto(struct NodoEleccion *eleccionActual, int idMesa, char *rutBuscado) {
    struct NodoMesa *mesa;
    struct NodoVoto *actual;
    char rutLimpio[15];
    
    calcularRutConDV(rutLimpio, rutBuscado);

    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idMesa);
    
    if (mesa == NULL || mesa->headlistavotos == NULL) {
        return NULL;
    }

    actual = mesa->headlistavotos->sig;
    
    do {
        if (strcmp(actual->rutvotante, rutLimpio) == 0) {
            return actual;
        }
        actual = actual->sig;
    } while (actual != mesa->headlistavotos->sig);

    return NULL;
}

/* ----------------------------------------------------------------- */
/* --- IMPLEMENTACION DE FUNCIONES PRINCIPALES --- */
/* ----------------------------------------------------------------- */

/* --- Registra el voto de la persona y lo guarda en la mesa --- */
void registrarVoto(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual) {
    char rutSucio[100];
    char rutLimpio[15];
    int idVoto;
    int indiceCandidato = -1;
    struct NodoVotante *votante;
    struct NodoMesa *mesa;
    struct NodoVoto *nuevoVoto;
    struct Candidato *candidato = NULL;
    
    limpiarPantalla();
    printf("--- REGISTRO DE VOTO ---\n");

    printf("\nIngrese RUT del votante (Sin Digito Verificador): ");
    fgets(rutSucio, sizeof(rutSucio), stdin);
    rutSucio[strcspn(rutSucio, "\n")] = 0;

    calcularRutConDV(rutLimpio, rutSucio);
    printf("RUT Procesado: %s\n", rutLimpio);
    votante = buscarVotante(sistema, rutLimpio);

    if (votante == NULL) {
        printf("\nError: Votante con RUT %s no encontrado en el padron.\n", rutLimpio);
        esperarEnter();
        return;
    }
    if (votante->mesaasignada == NULL) {
        printf("\nError: El votante %s no tiene mesa asignada. No puede votar.\n", votante->nombre);
        esperarEnter();
        return;
    }
    if (votante->havotado == 1) {
        printf("\nError: El votante %s ya ha votado.\n", votante->nombre);
        esperarEnter();
        return;
    }
    
    mesa = votante->mesaasignada;
    printf("\nVotante: %s. Mesa: %d\n", votante->nombre, mesa->idmesa);

    printf("\nIngrese ID del Candidato (o 0 para voto nulo/blanco): ");
    idVoto = ingresarOpcion();

    if (idVoto != 0) {
        indiceCandidato = obtenerIndiceLocal(eleccionActual, idVoto);
        
        if (indiceCandidato != -1) {
            candidato = &eleccionActual->arraycandidatos[indiceCandidato];
        }
    }
    
    if (idVoto != 0 && indiceCandidato == -1) {
        printf("\nAdvertencia: ID invalido. Voto se registrara como NULO.\n");
        idVoto = 0; 
        candidato = NULL;
    }

    
    nuevoVoto = crearNodoVoto(votante->rut, idVoto);
    if (nuevoVoto == NULL) return;
    insertarVoto(mesa, nuevoVoto);

    mesa->votosemitidos++;
    votante->havotado = 1; 
    
    if (idVoto != 0 && candidato != NULL) {
        mesa->conteovotos[indiceCandidato]++;
        printf("\nVoto valido por %s (ID %d) registrado con exito!\n", candidato->nombre, idVoto);
    } else {
        mesa->votos_nulos++; 
        printf("\nVoto registrado como *NULO* con exito!\n");
    }

    printf("Total de votos en Mesa %d: %d\n", mesa->idmesa, mesa->votosemitidos);
    esperarEnter();
}

/* busca y elimina un voto de la lista circular */
int eliminarVoto(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual, int idMesa, char* rutEliminar) {
    struct NodoMesa *mesa;
    struct NodoVoto *actual, *anterior;
    struct NodoVotante *votante;
    char rutLimpio[15];
    int idCandidatoEliminado;
    int indiceCandidato = -1;
    
    limpiarPantalla();
    printf("--- ANULAR VOTO REGISTRADO ---\n");
    
    calcularRutConDV(rutLimpio, rutEliminar);
    printf("Procesando anulacion para RUT: %s\n", rutLimpio);

    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idMesa);
    if (mesa == NULL || mesa->headlistavotos == NULL) {
        printf("Error: Mesa %d no encontrada o no tiene votos.\n", idMesa);
        esperarEnter();
        return 0;
    }

    anterior = mesa->headlistavotos;
    
    do {
        actual = anterior->sig;
        
        if (strcmp(actual->rutvotante, rutLimpio) == 0) {
            
            idCandidatoEliminado = actual->idcandidatovotado;
            
            if (anterior == actual) {
                mesa->headlistavotos = NULL; 
            }
            else if (actual == mesa->headlistavotos) {
                anterior->sig = actual->sig;
                mesa->headlistavotos = anterior;
            } 
            else {
                anterior->sig = actual->sig;
            }

            mesa->votosemitidos--;
            
            if (idCandidatoEliminado != 0) {
                indiceCandidato = obtenerIndiceLocal(eleccionActual, idCandidatoEliminado);
                if (indiceCandidato != -1) {
                    mesa->conteovotos[indiceCandidato]--;
                }
            } else {
                mesa->votos_nulos--;
            }

            votante = buscarVotante(sistema, rutLimpio);
            if (votante != NULL) {
                votante->havotado = 0;
            }
            
            printf("\n Voto de RUT %s en Mesa %d ANULADO. Conteo revertido.\n", rutLimpio, idMesa);
            esperarEnter();
            return 1;
        }

        anterior = anterior->sig;
        
    } while (anterior != mesa->headlistavotos);
    
    printf("\n Voto de RUT %s no encontrado en Mesa %d.\n", rutLimpio, idMesa);
    esperarEnter();
    return 0;
}

/* --- buscar un voto por el RUT del votante en una mesa y permite cambiar el candidato votado o marcarlo como nulo/blanco --- */
int modificarVoto(struct NodoEleccion *eleccionActual, int idMesa, char *rutModificar) {
    struct NodoVoto *voto;
    struct NodoMesa *mesa;
    int nuevoId, antiguoId, idxAntiguo, idxNuevo;
    
    limpiarPantalla();
    printf("--- MODIFICAR VOTO REGISTRADO ---\n");

    voto = buscarVoto(eleccionActual, idMesa, rutModificar);
    if (voto == NULL) {
        printf("Error: No se encontro el voto para el RUT ingresado en esa mesa.\n");
        esperarEnter();
        return 0;
    }
    
    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idMesa);
    
    printf("Voto actual por candidato ID: %d\n", voto->idcandidatovotado);
    printf("Ingrese NUEVO ID de Candidato (0 para nulo/blanco): ");
    nuevoId = ingresarOpcion();

    idxNuevo = -1;
    if (nuevoId != 0) {
        idxNuevo = obtenerIndiceLocal(eleccionActual, nuevoId);
        if (idxNuevo == -1) {
            printf("Error: El ID %d no corresponde a un candidato activo.\n", nuevoId);
            esperarEnter();
            return 0;
        }
    }
    antiguoId = voto->idcandidatovotado;
    if (antiguoId != 0) {
        idxAntiguo = obtenerIndiceLocal(eleccionActual, antiguoId);
        if (idxAntiguo != -1) { 
        mesa->conteovotos[idxAntiguo]--;
        }
    } else {
        mesa->votos_nulos--;
    }

    if (nuevoId != 0) {
        mesa->conteovotos[idxNuevo]++;
        printf("Contador actualizado: +1 para candidato ID %d.\n", nuevoId);
    } else {
        mesa->votos_nulos++;
        printf("Contador actualizado: +1 para Nulos/Blancos.\n");
    }
    voto->idcandidatovotado = nuevoId;
    printf("\n Modificacion completada con exito.\n");
    esperarEnter();
    return 1;
}

/* Muestra los votos de una mesa, el RUT de la persona y por quien voto */
void listarVotos(struct NodoEleccion *eleccionActual) {
    int idMesa;
    struct NodoMesa *mesa;
    struct NodoVoto *actual;
    int i = 1;

    limpiarPantalla();
    printf("--- Listado de Votos ---\n");
    printf("Ingrese ID de la mesa: ");
    idMesa = ingresarOpcion();

    mesa = buscarMesaRec(eleccionActual->raizarbolmesas, idMesa);
    
    if (mesa == NULL || mesa->headlistavotos == NULL) {
        printf("Mesa no encontrada o sin votos.\n");
        esperarEnter();
        return;
    }

    printf("\n[Votos en Mesa %d]\n", mesa->idmesa);
    
    actual = mesa->headlistavotos->sig; 
    do {
        printf(" %d. RUT: %-11s | Candidato ID: %d\n", i++, actual->rutvotante, actual->idcandidatovotado);
        actual = actual->sig;
    } while (actual != mesa->headlistavotos->sig);

}

/* muestra el menu de la gestion de votos */
void menuVotacion(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual) {
    int opcion;
    int idMesaOperacion;
    char rutSucio[100];
    char rutLimpio[15];
    struct NodoVoto *votoEncontrado;
    int claveIngresada;

    if (eleccionActual == NULL) {
        printf("Error: No existe una eleccion activa.\n");
        esperarEnter();
        return;
    }

    opcion = 0;
    do {
        limpiarPantalla();
        printf("\n...:: Proceso de Votacion (Vuelta %d) ::...\n", eleccionActual->numerovuelta);
        printf("------------------------------------------------\n");
        printf(" 1. Registrar un Voto (Simulacion)\n");
        printf(" --- AREA RESTRINGIDA (SERVEL) ---\n");
        printf(" 2. Listar Votos de una Mesa\n"); 
        printf(" 3. Buscar Voto por RUT y Mesa\n"); 
        printf(" 4. Eliminar Voto (Anulacion) por RUT\n"); 
        printf(" 5. Modificar Voto (Cambiar Candidato)\n");
        printf(" 0. Volver al Menu Principal\n");
        printf("------------------------------------------------\n");

        opcion = ingresarOpcion();
        
        if(opcion >= 2 && opcion <= 5){
            printf("\n[SEGURIDAD] Ingresar Clave de Feuncionario SERVEL: ");
            claveIngresada = ingresarOpcion();

            if(claveIngresada != CLAVE_SERVEL){
                printf("Clave incorrecta. Acceso denegado al registro secreto.\n");
                esperarEnter();
                continue;
            }
        }

        switch (opcion) {
            case 1:
                registrarVoto(sistema, eleccionActual);
                break;
            case 2:
                listarVotos(eleccionActual);
                break;
            case 3:
                printf("Ingrese ID de la mesa a buscar: ");
                idMesaOperacion = ingresarOpcion();
                
                printf("Ingrese RUT del votante (Sin Digito Verificador): ");
                fgets(rutSucio, sizeof(rutSucio), stdin);
                rutSucio[strcspn(rutSucio, "\n")] = 0;
                calcularRutConDV(rutLimpio, rutSucio);
                
                votoEncontrado = buscarVoto(eleccionActual, idMesaOperacion, rutLimpio);

                limpiarPantalla();
                printf("--- Resultado de la Busqueda ---\n");
                if (votoEncontrado != NULL) {
                    printf("Voto encontrado en Mesa ID %d!\n", idMesaOperacion);
                    printf("  RUT del Votante: %s\n", votoEncontrado->rutvotante);
                    printf("  ID del Candidato Votado: %d\n", votoEncontrado->idcandidatovotado);
                } else {
                    printf("Voto del RUT %s no encontrado en Mesa ID %d.\n", rutLimpio, idMesaOperacion);
                }
                esperarEnter();
                break;
            case 4:
                printf("Ingrese ID de la mesa donde se emitio el voto: ");
                idMesaOperacion = ingresarOpcion();
                
                printf("Ingrese RUT del votante para anular (Sin DV): ");
                fgets(rutSucio, sizeof(rutSucio), stdin);
                rutSucio[strcspn(rutSucio, "\n")] = 0;
                
                eliminarVoto(sistema, eleccionActual, idMesaOperacion, rutSucio);
                break;
            case 5:
                printf("Ingrese ID de la mesa del voto a corregir: ");
                idMesaOperacion = ingresarOpcion();
                
                printf("Ingrese RUT del votante (Sin DV): ");
                fgets(rutSucio, sizeof(rutSucio), stdin);
                rutSucio[strcspn(rutSucio, "\n")] = 0;
                
                modificarVoto(eleccionActual, idMesaOperacion, rutSucio);
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
/* --- IMPLEMENTACION FUNCIONES EXTRAS (ESCRUTINIO Y 2DA VUELTA) --- */
/* ----------------------------------------------------------------- */

/* --- Funciones Auxiliares para Escrutinio --- */
void contarVotosRecursivo(struct NodoMesa *raiz, long *conteoNacional, long *totalNulos, long *totalBlancos) {
    int i;
    if (raiz == NULL) return;

    contarVotosRecursivo(raiz->izq, conteoNacional, totalNulos, totalBlancos);

    *totalNulos += raiz->votos_nulos;
    *totalBlancos += raiz->votos_blancos;
    
    for (i = 0; i < MAXCANDIDATOS; i++) {
        conteoNacional[i] += raiz->conteovotos[i];
    }

    contarVotosRecursivo(raiz->der, conteoNacional, totalNulos, totalBlancos);
}

/* --- Funciones Principales del Modulo Escrutinio --- */

/* Realiza el conteo nacional y muestra porcentajes */
void realizarEscrutinioNacional(struct NodoEleccion *eleccionActual) {
    long *conteoNacional;
    long totalNulos = 0, totalBlancos = 0, totalEmitidos = 0, totalValidos = 0;
    int i, j;
    float porcentaje;
    struct Candidato *cand;
    int idx;

    limpiarPantalla();
    printf("--- ESCRUTINIO NACIONAL (Vuelta %d) ---\n", eleccionActual->numerovuelta);

    if (eleccionActual->raizarbolmesas == NULL) {
        printf("No hay mesas registradas para realizar el escrutinio.\n");
        esperarEnter();
        return;
    }

    conteoNacional = (long*)malloc(MAXCANDIDATOS * sizeof(long));
    if (conteoNacional == NULL) {
        printf("Error de memoria en escrutinio.\n");
        return;
    }

    for (i = 0; i < MAXCANDIDATOS; i++) {
        conteoNacional[i] = 0;
    }

    contarVotosRecursivo(eleccionActual->raizarbolmesas, conteoNacional, &totalNulos, &totalBlancos);

    for (j = 0; j < eleccionActual->numcandidatos; j++) {
        cand = &eleccionActual->arraycandidatos[j];
        idx = (int)(cand - poolCandidatos); 
        
        if (idx >= 0 && idx < MAXCANDIDATOS) {
            totalValidos += conteoNacional[idx];
        }
    }

    totalEmitidos = totalValidos + totalNulos + totalBlancos;

    printf("Total Votos Emitidos: %ld\n", totalEmitidos);
    printf("Total Votos Validos:  %ld\n", totalValidos);
    printf("Total Nulos:          %ld\n", totalNulos);
    printf("Total Blancos:        %ld\n", totalBlancos);
    printf("------------------------------------------------\n");
    printf("RESULTADOS POR CANDIDATO:\n");

    for (j = 0; j < eleccionActual->numcandidatos; j++) {
        cand = &eleccionActual->arraycandidatos[j];
        idx = (int)(cand - poolCandidatos);
        
        if (totalValidos > 0) {
            porcentaje = (float)conteoNacional[idx] * 100.0 / totalValidos;
        } else {
            porcentaje = 0.0;
        }
        
        printf(" %d. %-20s | Votos: %-8ld | %5.2f%%\n", 
               cand->idcandidato, 
               cand->nombre, 
               conteoNacional[idx], 
               porcentaje);
    }
    
    printf("------------------------------------------------\n");
    esperarEnter();
}

/* Gestiona la creacion automatica de la 2da vuelta */
void gestionarSegundaVuelta(struct SistemaElectoral *sistema) {
    struct NodoEleccion *actual;
    struct NodoEleccion *nuevaVuelta;
    struct NodoVotante *votanteActual;
    long *conteoNacional;
    long totalNulos = 0, totalBlancos = 0, totalValidos = 0;
    long votos1 = -1;
    long votos2 = -1;
    long maxVotos = -1;
    int i;
    int idx1 = -1, idx2 = -1; 
    int slot1, slot2;
    int ganadorIdx = -1;
    float porcentaje;

    actual = sistema->headelecciones;
    while (actual->sig != NULL) {
        actual = actual->sig;
    }

    if (actual->numerovuelta > 2) {
        printf("El proceso electoral ha finalizado (Ya hubo segunda vuelta).\n");
        esperarEnter();
        return;
    }

    conteoNacional = (long*)malloc(MAXCANDIDATOS * sizeof(long));
    if (conteoNacional == NULL) { 
        printf("Error memoria.\n"); return; 
    }
    
    for (i = 0; i < MAXCANDIDATOS; i++) {
        conteoNacional[i] = 0;
    }
    
    contarVotosRecursivo(actual->raizarbolmesas, conteoNacional, &totalNulos, &totalBlancos);
    
    for (i = 0; i < actual->numcandidatos; i++) {
        totalValidos += conteoNacional[i];
    }

    if (totalValidos == 0) {
        printf("No hay votos validos suficientes para calcular resultados.\n");
        esperarEnter(); 
        return;
    }
    /* --- LOGICA DE DECISION --- */

    if (actual->numerovuelta == 2) {
        ganadorIdx = -1;
        maxVotos = -1;

        for (i = 0; i < actual->numcandidatos; i++) {
             if (conteoNacional[i] > maxVotos) {
                 maxVotos = conteoNacional[i];
                 ganadorIdx = i;
             }
        }
        
        limpiarPantalla();
        printf("--- RESULTADOS FINALES SEGUNDA VUELTA ---\n");
        if (ganadorIdx != -1) {
            porcentaje = (float)maxVotos * 100.0 / totalValidos;
            printf("TENEMOS PRESIDENTE ELECTO!\n");
            printf("Ganador: %s\n", actual->arraycandidatos[ganadorIdx].nombre);
            printf("Votos: %ld (%.2f%%)\n", maxVotos, porcentaje);
        } else {
            printf("Empate o sin datos.\n");
        }
        esperarEnter();
        return;
    }

    for (i = 0; i < actual->numcandidatos; i++) {
         if ((float)conteoNacional[i] / totalValidos > 0.50) {
            printf("El candidato %s ha ganado con mas del 50%%\n", 
                   actual->arraycandidatos[i].nombre);
            printf("No es necesaria una segunda vuelta.\n");
            esperarEnter(); 
            return;
        }
        
        if (conteoNacional[i] > votos1) {
            votos2 = votos1; idx2 = idx1;
            votos1 = conteoNacional[i]; idx1 = i;
        } else if (conteoNacional[i] > votos2) {
            votos2 = conteoNacional[i]; idx2 = i;
        }
    }
    
    if (idx1 == -1 || idx2 == -1) {
        printf("Error: No hay suficientes candidatos con votos para segunda vuelta.\n");
        esperarEnter(); return;
    }

    limpiarPantalla();
    printf("--- PASANDO A SEGUNDA VUELTA ---\n");
    printf("Finalistas:\n");
    printf(" 1. %s (Votos: %ld)\n", actual->arraycandidatos[idx1].nombre, votos1);
    printf(" 2. %s (Votos: %ld)\n", actual->arraycandidatos[idx2].nombre, votos2);
    
    nuevaVuelta = (struct NodoEleccion*)malloc(sizeof(struct NodoEleccion));
    if (nuevaVuelta == NULL) return;
    
    nuevaVuelta->numerovuelta = 2;
    strcpy(nuevaVuelta->fecha, "19-12-2025");
    nuevaVuelta->numcandidatos = 2;
    nuevaVuelta->raizarbolmesas = NULL;
    nuevaVuelta->sig = NULL;

    slot1 = plibre++;
    poolCandidatos[slot1] = actual->arraycandidatos[idx1]; 
    poolCandidatos[slot1].libre = 0;
    
    slot2 = plibre++;
    poolCandidatos[slot2] = actual->arraycandidatos[idx2]; 
    poolCandidatos[slot2].libre = 0;

    nuevaVuelta->arraycandidatos = &poolCandidatos[slot1]; 

    votanteActual = sistema->headregistroelectoral;
    while (votanteActual != NULL) {
        votanteActual->havotado = 0;
        votanteActual = votanteActual->sig;
    }

    actual->sig = nuevaVuelta;
    
    printf("\nSegunda Vuelta creada exitosamente\n");
    printf("Los votantes han sido habilitados nuevamente.\n");
    printf("Nota: Debe crear nuevas mesas para esta segunda vuelta.\n");
    esperarEnter();
}

/* Submenu para Funciones Extras */
void menuOperacionesEspeciales(struct SistemaElectoral *sistema, struct NodoEleccion *eleccionActual) {
    int opcion;
    do {
        limpiarPantalla();
        printf("\n...:: Funciones Extras y Escrutinio (Vuelta %d) ::...\n", eleccionActual->numerovuelta);
        printf(" 1. Ver Escrutinio Nacional (Informe y %%)\n");
        printf(" 2. Gestionar Segunda Vuelta (Proclamacion)\n");
        printf(" 0. Volver al Menu Principal\n");
        
        opcion = ingresarOpcion();

        switch (opcion) {
            case 1:
                realizarEscrutinioNacional(eleccionActual);
                break;
            case 2:
                gestionarSegundaVuelta(sistema);
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