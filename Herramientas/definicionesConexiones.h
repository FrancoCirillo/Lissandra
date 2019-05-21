//-------definicionesConexiones.h-------

#ifndef MEMORIA_DEFINICIONESCONEXIONES_H_
#define MEMORIA_DEFINICIONESCONEXIONES_H_

#include <commons/collections/list.h>
#include "tiempo.h"

#define IP_FILESYSTEM "127.0.0.2"
#define IP_MEMORIA "127.0.0.3"
#define IP_KERNEL "127.0.0.4"
#define PORT "4444"
//TODO Diseñar correctamente las herramientas y conexiones (arreglar este .h)

#define COLOR_ANSI_ROJO "\x1b[31m"
#define COLOR_ANSI_VERDE "\x1b[32m"
#define COLOR_ANSI_AMARILLO "\x1b[33m"
#define COLOR_ANSI_AZUL "\x1b[34m"
#define COLOR_ANSI_MAGENTA "\x1b[35m"
#define COLOR_ANSI_CYAN "\x1b[36m"
#define COLOR_ANSI_RESET "\x1b[0m"

#define CONSOLA_KERNEL 0
#define CONSOLA_MEMORIA 1
#define CONSOLA_FS 2

#define BASE_CONSOLA_KERNEL 0
#define BASE_CONSOLA_MEMORIA 100
#define BASE_CONSOLA_FS 200
#define BASE_COD_ERROR 1000

typedef enum cod_proceso {
	KERNEL,
	MEMORIA,
	FILESYSTEM
} cod_proceso;

typedef enum cod_op {
	CODIGO_EXITO = 0,
	CODIGO_SELECT = 1,
	CODIGO_INSERT = 2,
	CODIGO_CREATE = 3,
	CODIGO_DESCRIBE = 4,
	CODIGO_DROP = 5,
	CODIGO_JOURNAL = 6,
	CODIGO_ADD = 7,
	CODIGO_RUN = 8,
	CODIGO_METRICS = 9,
	DEVOLUCION_SELECT = 10,

	ERROR_SELECT = BASE_COD_ERROR + CODIGO_SELECT,
	ERROR_INSERT = BASE_COD_ERROR + CODIGO_INSERT,
	ERROR_CREATE = BASE_COD_ERROR + CODIGO_CREATE,
	ERROR_DESCRIBE = BASE_COD_ERROR + CODIGO_DESCRIBE,
	ERROR_DROP = BASE_COD_ERROR + CODIGO_DROP,
	ERROR_JOURNAL = BASE_COD_ERROR + CODIGO_JOURNAL,
	ERROR_ADD = BASE_COD_ERROR + CODIGO_ADD,
	ERROR_RUN = BASE_COD_ERROR + CODIGO_RUN,
	ERROR_METRICS = BASE_COD_ERROR + CODIGO_METRICS,

	// Solo para saber si devolver los resultados al Kernel o Memoria para que imprima
	CONSOLA_FS_EXITO = BASE_CONSOLA_FS + CODIGO_EXITO,
	CONSOLA_MEM_EXITO = BASE_CONSOLA_MEMORIA + CODIGO_EXITO,
	CONSOLA_KRN_EXITO = BASE_CONSOLA_KERNEL + CODIGO_EXITO,

	CONSOLA_FS_SELECT = BASE_CONSOLA_FS + CODIGO_SELECT,
	CONSOLA_FS_INSERT = BASE_CONSOLA_FS + CODIGO_INSERT,
	CONSOLA_FS_CREATE = BASE_CONSOLA_FS + CODIGO_CREATE,
	CONSOLA_FS_DESCRIBE = BASE_CONSOLA_FS + CODIGO_DESCRIBE,
	CONSOLA_FS_DROP = BASE_CONSOLA_FS + CODIGO_DROP,

	CONSOLA_MEM_SELECT = BASE_CONSOLA_MEMORIA + CODIGO_SELECT,
	CONSOLA_MEM_INSERT = BASE_CONSOLA_MEMORIA + CODIGO_INSERT,
	CONSOLA_MEM_CREATE = BASE_CONSOLA_MEMORIA + CODIGO_CREATE,
	CONSOLA_MEM_DESCRIBE = BASE_CONSOLA_MEMORIA + CODIGO_DESCRIBE,
	CONSOLA_MEM_DROP = BASE_CONSOLA_MEMORIA + CODIGO_DROP,
	CONSOLA_MEM_JOURNAL = BASE_CONSOLA_MEMORIA + CODIGO_JOURNAL,
	CONSOLA_MEM_RTA_SELECT = BASE_CONSOLA_MEMORIA + DEVOLUCION_SELECT,

	CONSOLA_KRN_SELECT = BASE_CONSOLA_KERNEL + CODIGO_SELECT,
	CONSOLA_KRN_RTA_SELECT = BASE_CONSOLA_KERNEL + DEVOLUCION_SELECT,
	CONSOLA_KRN_INSERT = BASE_CONSOLA_KERNEL + CODIGO_INSERT,
	CONSOLA_KRN_CREATE = BASE_CONSOLA_KERNEL + CODIGO_CREATE,
	CONSOLA_KRN_DESCRIBE = BASE_CONSOLA_KERNEL + CODIGO_DESCRIBE,
	CONSOLA_KRN_DROP = BASE_CONSOLA_KERNEL + CODIGO_DROP,
	CONSOLA_KRN_JOURNAL = BASE_CONSOLA_KERNEL + CODIGO_JOURNAL,
	CONSOLA_KRN_ADD = BASE_CONSOLA_KERNEL + CODIGO_ADD,
	CONSOLA_KRN_RUN = BASE_CONSOLA_KERNEL + CODIGO_RUN,
	CONSOLA_KRN_METRICS = BASE_CONSOLA_KERNEL + CODIGO_METRICS,

	CODIGO_HANDSHAKE = 11,

	INPUT_ERROR = 12
} cod_op;

typedef struct instr_t
{
	mseg_t timestamp;
	cod_op codigo_operacion;
	t_list *parametros;
} instr_t;

typedef struct remitente_t
{
	char *ip;
	char *puerto;
} remitente_t;

typedef struct remitente_instr_t
{
	instr_t *instruccion;
	remitente_t *remitente;
} remitente_instr_t;

/*
 * En stream se guardan los valores y los tamanios de los valores de manera consecutiva
 * Size indica el tamanio total del buffer, esto es en tamanio de los valores + n*int
 *
 * Ej:
 * 		buffer: unValor7otroValor9
 * 		size: 7+9+(2*sizeof(int))
 *
 */
typedef struct t_buffer
{
	int size;
	void *stream;
} t_buffer;

typedef struct t_paquete
{
	mseg_t timestamp;
	cod_op codigo_operacion;
	t_buffer *buffer;
} t_paquete;

//para usar con diccionario {<Nombre>:{fd_out, fd_in,puerto, IP}}
typedef struct identificador
{
	int fd_out;
	int fd_in;
	char puerto[8];
	char ip_proceso[17]; //"000.000.000.000\n"
} identificador;

#endif /* MEMORIA_DEFINICIONESCONEXIONES_H_ */
