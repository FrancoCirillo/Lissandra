//-------fileSystem.h-------

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "../Herramientas/tiempo.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsCliente.h"
#include "../Herramientas/utilsServidor.h"
#include "../Herramientas/serializaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <math.h>
#include <inttypes.h>
#include "Estructuras.h"
#include "compactador.h"
#include "memtable.h"
#include "instructions.h"
#include "registros.h"

//ORDEN PARAMETROS

//SELECT [NOMBRE_TABLA] [KEY]
//INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]
//CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
//DESCRIBE [NOMBRE_TABLA]
//DROP [NOMBRE_TABLA]



/*STRUCTS*/

typedef unsigned long long mseg_t;

typedef struct ruta_t {
	char* tablas;
	char* bloques;
	char* carpeta_metadata;
	char* bitmap;
	char* metadata;
} ruta_t;


typedef struct archivo_t {
	int size;
	int blocks[];
} archivo_t;

typedef struct metadata_t {
	char* consistency;
	char* partitions;
	mseg_t compactation_time;
} metadata_t;

typedef struct config_FS_t {
	char* puerto_escucha;
	char* punto_montaje;
	int tamanio_value;
	mseg_t retardo;
	mseg_t tiempo_dump;
} config_FS_t;

typedef struct metadata_FS_t {
	int block_size;
	int blocks;
	char* magic_number;
} metadata_FS_t;


/*SEMAFOROS*/
sem_t mutex_tiempo_dump_config;
sem_t mutex_tiempo_retardo_config;
sem_t mutex_memtable;
sem_t mutex_log;
sem_t mutex_cant_bloques;

config_FS_t config_FS;
metadata_FS_t Metadata_FS;
ruta_t g_ruta;

t_log* g_logger;

t_config* meta_config;

/*TESTS*/
registro_t* crearRegistro(mseg_t, uint16_t, char*);
void		imprimirRegistro(void*);
void		imprimirContenidoArchivo(char*);
void		imprimirMetadata(char*);
t_list*		listaRegistros();
void		pruebaDump();
void		pruebaTmp();
void		pruebaGeneral();


/*INICIALIZACION Y FINALIZACION*/
void inicializar_FS();
void finalizar_FS();
void iniciar_semaforos();
void iniciar_rutas();
void finalizar_rutas();
void actualizar_config();

/*FUNCIONES DE BLOQUES*/
t_list* leer_binario(char*, uint16_t);
t_list* leer_archivos_temporales(char*, uint16_t);
void 	escribir_bloque(registro_t*);
void 	eliminar_archivos(char*); //TODO hacer

char* 	obtener_registro_mas_reciente(t_list*);
t_list* obtener_registros_key(char*, uint16_t);

/*FUNCIONES AUXILIARES*/
char*	obtener_parametro(instr_t*, int);
int  	obtener_particion_key(char*, int);
char*	obtener_nombre_tabla(instr_t*);
_Bool 	es_registro_mas_reciente(void*, void*);

/*Comunicación*/
//eliminar
//void response(remitente_t*);
void 	liberar_memoria_instr(instr_t *);

/* CONEXIONES */
int fd_out_inicial;
char* miIP;
t_dictionary *conexionesActuales;
void (*callback)(instr_t *instruccion, char *remitente);
void 	inicializar_conexiones();
void 	imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam, char *remitente);
int 	obtener_fd_out(char *proceso);
void 	enviar_tamanio_value(char* remitente);
t_dictionary *auxiliarConexiones;

#endif /* FILE_SYSTEM_H */
