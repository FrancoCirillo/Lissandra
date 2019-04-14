/*
 * definicionesConexiones.h
 *
 *  Created on: 7 abr. 2019
 *      Author: utnso
 */

#ifndef MEMORIA_DEFINICIONESCONEXIONES_H_
#define MEMORIA_DEFINICIONESCONEXIONES_H_

#define IP_FILESYSTEM "127.0.0.1"
#define IP_MEMORIA "127.0.0.2"
#define IP_KERNEL "127.0.0.3"
#define PORT "4444"

//TODO Diseñar correctamente las herramientas y conexiones (arreglar este .h)
typedef enum
{
	KERNEL,
	MEMORIA,
	FILESYSTEM
}cod_proceso;

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct instr_t{
	time_t timestamp;
	int codigoInstruccion;
	char* param1;
	char* param2;
	char* param3;
	char* param4;
}instr_t;
typedef struct remitente_t{
	char* ip;
	char* puerto;
}remitente_t;
typedef struct remitente_instr_t{
	instr_t *instruccion;
	remitente_t *remitente;
}remitente_instr_t;


#endif /* MEMORIA_DEFINICIONESCONEXIONES_H_ */
