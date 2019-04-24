//-------definicionesConexiones.h-------

#ifndef MEMORIA_DEFINICIONESCONEXIONES_H_
#define MEMORIA_DEFINICIONESCONEXIONES_H_

#include <commons/collections/list.h>

#define IP_FILESYSTEM "127.0.0.2"
#define IP_MEMORIA "127.0.0.3"
#define IP_KERNEL "127.0.0.4"
#define PORT "4444"

//TODO Diseñar correctamente las herramientas y conexiones (arreglar este .h)

typedef enum cod_proceso{
	KERNEL,
	MEMORIA,
	FILESYSTEM
}cod_proceso;

typedef enum cod_op{
	CODIGO_EXITO,
	CODIGO_SELECT = 3,
	ERROR_SELECT = -3,
	CODIGO_INSERT = 4,
	ERROR_INSERT = -4,
	CODIGO_CREATE = 5,
	ERROR_CREATE = -5,
	CODIGO_DESCRIBE = 6,
	ERROR_DESCRIBE = -6,
	CODIGO_DROP = 7,
	ERROR_DROP = -7,
	CODIGO_JOURNAL = 8,
	ERROR_JOURNAL = -8
}cod_op;

typedef struct instr_t{
	time_t timestamp;
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

typedef struct{
	int size;
	void* stream;
}t_buffer;

typedef struct{
	time_t timestamp;
	cod_op codigo_operacion;
	t_buffer* buffer;
}t_paquete;


#endif /* MEMORIA_DEFINICIONESCONEXIONES_H_ */
