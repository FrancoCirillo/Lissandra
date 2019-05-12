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
#define COLOR_ANSI_AMARILLO  "\x1b[33m"
#define COLOR_ANSI_AZUL "\x1b[34m"
#define COLOR_ANSI_MAGENTA "\x1b[35m"
#define COLOR_ANSI_CYAN    "\x1b[36m"
#define COLOR_ANSI_RESET   "\x1b[0m"


typedef enum cod_proceso{
	KERNEL,
	MEMORIA,
	FILESYSTEM
}cod_proceso;

typedef enum cod_op{
	CODIGO_EXITO,
	CODIGO_SELECT,
	ERROR_SELECT = -1,
	CODIGO_INSERT = 2,
	ERROR_INSERT = -2,
	CODIGO_CREATE = 3,
	ERROR_CREATE = -3,
	CODIGO_DESCRIBE = 4,
	ERROR_DESCRIBE = -4,
	CODIGO_DROP = 5,
	ERROR_DROP = -5,
	CODIGO_JOURNAL = 6,
	ERROR_JOURNAL = -6,
	CODIGO_ADD = 7,
	ERROR_ADD = -7,
	CODIGO_RUN = 8,
	ERROR_RUN = -8,
	CODIGO_METRICS = 9,
	ERROR_METRICS = -9
}cod_op;

typedef struct instr_t{
	mseg_t timestamp;
	cod_op codigo_operacion;
	t_list* parametros;
}instr_t;

typedef struct remitente_t{
	char* ip;
	char* puerto;
}remitente_t;

typedef struct remitente_instr_t{
	instr_t *instruccion;
	remitente_t *remitente;
}remitente_instr_t;

typedef struct t_buffer{
	int size;
	void* stream;
}t_buffer;

typedef struct t_paquete{
	mseg_t timestamp;
	cod_op codigo_operacion;
	t_buffer* buffer;
}t_paquete;


#endif /* MEMORIA_DEFINICIONESCONEXIONES_H_ */
