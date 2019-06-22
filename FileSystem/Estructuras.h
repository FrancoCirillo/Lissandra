//--------estructuras.h--------

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "memtable.h"
#include "fileSystem.h"
#include <commons/txt.h>
#include <commons/bitarray.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <stdio.h>

#include "registros.h"


//GLOBALES
void iniciar_logger();
int  bloques_disponibles;
char* rutaConfiguracion;
t_config* g_config;
t_dictionary* dic_semaforos_tablas;


//LOGS Y CONFIG
void  loggear_FS(char*);
void  loggear_FS_error(char*, instr_t*);
void  inicializar_configuracion(void);
void  actualizar_tiempo_retardo_config(mseg_t);   //ver si le llega mseg_t o un char*
void  actualizar_tiempo_dump_config(mseg_t);		//ver si le llega mseg_t o un char*
//char* obtener_por_clave(char* , char*);
int   obtener_tiempo_dump_config();


//METADATA TABLAS
void	  metadata_inicializar(FILE*, instr_t*);
void      crear_metadata(instr_t*);
t_config* obtener_metadata(char*);
int 	  obtener_part_metadata(char*);
char* 	  obtener_consistencia_metadata(char*);
int 	  obtener_tiempo_compactacion_metadata(char*);

void 	  leer_metadata_FS();

//BITARRAY
int 		bloques_en_bytes();
void 		inicializar_bitmap();
t_bitarray* levantar_bitmap();
void 		actualizar_bitmap(t_bitarray*);
int 		cant_bloques_disp();
void 		eliminar_bitarray(t_bitarray*);
int			bloque_esta_ocupado(t_bitarray*, int);
int 		siguiente_bloque_disponible(); //NO VALIDA SI HAY BLOQUES DISPONIBLES
void 		ocupar_bloque(int);
void 		liberar_bloque(int);


//DIRECTORIOS Y ARCHIVOS
void  inicializar_directorios(void);
void  crear_directorio(char*, char*);
int   eliminar_directorio(char*);
int   carpeta_esta_vacia(char*);
FILE* crear_archivo(char*, char*, char*);


//BLOQUES
void  crear_bloque(char*);
void  crear_bloques();
char* formatear_registro(registro_t*);
char* obtener_ruta_bloque(int);

void inicializar_bloques_disp();
void escribir_registro_bloque(registro_t*, char*, char*);
int  puede_crear_particiones(instr_t*);
int  cant_bloques_disponibles();
void restar_bloques_disponibles(int);
void incrementar_bloques_disponibles(int);

t_list*	buscar_key_en_bloques(char*, uint16_t, int);


//TEMPORALES Y BINARIOS
int		obtener_ultimo_bloque(char*);
char*	aplanar(char**);
char*	agregar_bloque_bloques(char**, int);
int		agregar_bloque_archivo(char*, int);
FILE*	crear_tmp(char*, char*);
void	crear_particiones(instr_t*);
int		tam_registro(registro_t*);
int		obtener_tam_archivo(char*);
void	aumentar_tam_archivo(char*, registro_t*);
int		cantidad_bloques_usados(char*);
int		espacio_restante_bloque(char*);
void	liberar_bloques(char*);
int 	obtener_siguiente_bloque_archivo(char*, int);


//SEMAFOROS
void   crear_dic_semaforos_FS();
void   agregar_mutex_a_dic(char*, sem_t*);
int obtener_mutex_tabla(char*, sem_t*);
sem_t* aux_obtener_mutex_tabla(char*);
int existe_mutex(char*);
void eliminar_mutex_de_tabla(char*);


void borrar_lista_parametros_impresion(t_list*);
int archivo_inicializar(FILE*);



#endif /* ESTRUCTURAS_H_ */
