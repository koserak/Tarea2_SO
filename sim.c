#include "sim.h"

int calcularLog2(int n) {
    int log = 0;
    while (n > 1) {
        n / 2;
        log++;
    }
    return log;
}

Simulador* iniciarSim(int nMarcosE, int tMarcoE, bool verboseE) { // Entradas definidas como ..E para evitar confusiones.
    Simulador *sim = (Simulador*)malloc(sizeof(Simulador));
    sim -> nMarcos = nMarcosE;
    sim -> tMarco = tMarcoE;
    sim -> b = calcularLog2(tMarcoE);
    sim -> mask = (1 << sim->b) - 1;  // Mascara = 2^b - 1
    sim -> verbose = verboseE;
    sim -> punteroReloj = 0;
    sim -> fallos = 0;
    sim -> referenciasTotales = 0;
    sim -> nPaginas = MAX_PAGINAS;
    sim -> tablaPaginas = (EntradaTP*)malloc(sim->nPaginas * sizeof(EntradaTP)); // Tabla de paginas.
    for (int i = 0; i < sim->nPaginas; i++) {
        sim -> tablaPaginas[i].marco = -1;
        sim -> tablaPaginas[i].valido = false;
        sim -> tablaPaginas[i].bitReferencia = false;
    }
    sim -> marcoOcupados = (bool*)calloc(nMarcosE, sizeof(bool)); // Marcos.
    return sim;
}

// Descomponer direccion virtual en npv y offset segun la especificacion entregada.
void dirVirtual(Simulador *sim, unsigned int dv, unsigned int *npv, unsigned int *offset) {
    *offset = dv & sim -> mask;      // offset = DV & MASK (sacada del enunciado).
    *npv = dv >> sim -> b;           // npv = DV >> b (sacada del enunciado).
}

int buscarMarco(Simulador *sim) { // Busca un marco libre.
    for (int i = 0; i < sim -> nMarcos; i++) {
        if (!sim -> marcoOcupados[i]) {
            return i;
        }
    }
    return -1;
}

int reloj(Simulador *sim, unsigned int *npvExpulsada) { // Algoritmo reloj.
    int intentos = 0;
    int maxIntentos = sim -> nMarcos * 2;
    
    while (intentos < maxIntentos) { // Busca en que página virtual está en el marco actual.
        int npvActual = -1;
        for (int i = 0; i < sim -> nPaginas; i++) {
            if (sim -> tablaPaginas[i].valido && sim -> tablaPaginas[i].marco == sim -> punteroReloj) {
                npvActual = i;
                break;
            }
        }
        
        if (npvActual != -1) { // Si el bit de referencia es 0, se reemplaza la pagina.
            if (!sim -> tablaPaginas[npvActual].bitReferencia) {
                int victima = sim -> punteroReloj;
                *npvExpulsada = npvActual; // Se invalida la pagina expulsada.
                sim -> tablaPaginas[npvActual].valido = false;
                sim -> tablaPaginas[npvActual].marco = -1;
                sim -> tablaPaginas[npvActual].bitReferencia = false;
                sim -> punteroReloj = (sim -> punteroReloj + 1) % sim -> nMarcos;
                return victima;
            }
            sim -> tablaPaginas[npvActual].bitReferencia = false; // Si el bit es 1, se pone en 0 y avanza.
        }   
        sim -> punteroReloj = (sim -> punteroReloj + 1) % sim -> nMarcos;
        intentos++;
    }
    return sim -> punteroReloj;
}

unsigned int traducirDireccion(Simulador *sim, unsigned int dv) { // Traduce la direccion virtual.
    unsigned int npv, offset;
    dirVirtual(sim, dv, &npv, &offset);
    sim -> referenciasTotales++;
    if (npv >= sim -> nPaginas) { // Verificacion del npv en el rango valido.
        if (sim -> verbose) {
            printf("DV: 0x%X, npv %u fuera de rango\n", dv, npv);
        }
        return 0xFFFFFFFF;
    }
    
    bool hit = sim -> tablaPaginas[npv].valido;
    int marco;
    unsigned int df;
    
    if (hit) { // La pagina esta en la tabla.
        marco = sim -> tablaPaginas[npv].marco;
        sim -> tablaPaginas[npv].bitReferencia = true;
        df = (marco << sim -> b) | offset;
        
        if (sim -> verbose) {
            printf("DV: 0x%X, npv: %u, offset: %u, HIT, marco: %d, DF: 0x%X\n", dv, npv, offset, marco, df);
        }
    } else { // La pagina no esta en la tabla.
        sim -> fallos++;
        marco = buscarMarco(sim);
        if (marco == -1) { // Si no hay un marco libre se llama al algoritmo reloj.
            unsigned int npvExpulsada;
            marco = reloj(sim, &npvExpulsada);
            if (sim -> verbose) {
                printf("DV: 0x%X, npv: %u, offset: %u, FALLO, marco: %d (Reloj: expulsada npv %u), ", dv, npv, offset, marco, npvExpulsada);
            }
        } else {
            if (sim -> verbose) {
                printf("DV: 0x%X, npv: %u, offset: %u, FALLO, marco: %d (libre), ", dv, npv, offset, marco);
            }
        }
        
        sim -> tablaPaginas[npv].marco = marco; // Se le da un marco a la pagina.
        sim -> tablaPaginas[npv].valido = true;
        sim -> tablaPaginas[npv].bitReferencia = true;
        sim -> marcoOcupados[marco] = true;
        df = (marco << sim -> b) | offset;
        if (sim->verbose) {
            printf("DF: 0x%X\n", df);
        }
    }
    return df;
}

void procesarArchivo(Simulador *sim, const char *archivo) {
    FILE *fp = fopen(archivo, "r");
    if (!fp) {
        fprintf(stderr, "No se pudo abrir el archivo '%s'\n", archivo);
        exit(1);
    }
    
    unsigned int dv;
    char linea[256];
    
    while (fgets(linea, sizeof(linea), fp)) { // Decimal y Hexadecimal
        if (strstr(linea, "0x") || strstr(linea, "0X")) {
            sscanf(linea, "%x", &dv);
        } else {
            sscanf(linea, "%u", &dv);
        }
        traducirDireccion(sim, dv);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "./sim nMarcos tMarco [--verbose] archivo.txt\n");
        return 1;
    }
    
    int nMarcos = atoi(argv[1]);
    int tMarco = atoi(argv[2]);
    bool verbose = false;
    char *archivoTraza;
    
    if (argc == 5 && strcmp(argv[3], "--verbose") == 0) { // Se recibe el --verbose.
        verbose = true;
        archivoTraza = argv[4];
    } else if (argc == 4) {
        archivoTraza = argv[3];
    } else {
        fprintf(stderr, "Argumentos inválidos\n");
        fprintf(stderr, "./sim nMarcos tMarco [--verbose] archivo.txt\n");
        return 1;
    }
    
    int temp = tMarco; // Validacion del tamaño de marco, para que sea potencia de 2.
    int count = 0;
    while (temp > 0) {
        count += (temp & 1);
        temp >>= 1;
    }
    if (count != 1) {
        fprintf(stderr, "El tamaño de marco debe ser potencia de 2\n");
        return 1;
    }
    
    printf("=== Simulador de Traduccion de Direcciones ===\n");
    printf("Numero de marcos:  %d\n", nMarcos);
    printf("Tamaño de marco:   %d bytes (2^%d)\n", tMarco, calcularLog2(tMarco));
    printf("Archivo:           %s\n", archivoTraza);
    printf("verbose:           %s\n", verbose ? "Sí" : "No");
    printf("Algoritmo:         Reloj\n\n");
    
    Simulador *sim = iniciarSim(nMarcos, tMarco, verbose);
    procesarArchivo(sim, archivoTraza);
    
    printf("\n====== Estadísticas ======\n");
    printf("Referencias:    %d\n", sim -> referenciasTotales);
    printf("Fallos de página:    %d\n", sim -> fallos);
    if (sim -> referenciasTotales > 0) {
        double porcentajeFallos = (sim -> fallos * 100.0) / sim -> referenciasTotales;
        printf("Tasa de fallos:    %.2f%%\n", porcentajeFallos);
    }
    free(sim -> tablaPaginas);
    free(sim -> marcoOcupados);
    free(sim);
    
    return 0;
}