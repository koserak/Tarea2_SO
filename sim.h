#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PAGINAS 1048576  // 32 bits

typedef struct {
    int marco;                  // Numero de marco.
    bool valido;                // Bit de validacion.
    bool bitReferencia;         // Bit de referencia para el algoritmo Reloj.
} EntradaTP;                    // Struct de la Tabla de Paginas.

typedef struct {
    EntradaTP *tablaPaginas;    // Tabla de paginas.
    int nPaginas;           
    bool *marcoOcupados;        // bool que indica si el marco esta ocupado.     
    int nMarcos;                // numero total de marcos.
    int tMarco;                 // Tamaño del marco.
    int b;                      // Bits de desplazamiento.
    unsigned int mask;          // Mascara para offset.
    int punteroReloj;           // Puntero para el algoritmo de reloj.
    int fallos;         
    int referenciasTotales;   
    bool verbose;              
} Simulador;                    // Struct del simulador.

