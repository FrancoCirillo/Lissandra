
#include "instrucciones.h"

void ejecutar_instruccion_select(instr_t *instruccion)
{
	puts("Ejecutando instruccion Select");
	int seEncontro = 0; //No cambiar hasta que se implemente conexionKERNEL
	sleep(1);			//Buscar
	if (seEncontro)
	{
		//Directo para el Kernel
		t_list *listaParam = list_create();
		char cadena[400];
		sprintf(cadena, "%s%s%s%s%s%s%s%lu", "Se encontro ", (char *)list_get(instruccion->parametros, 0), " | ", (char *)list_get(instruccion->parametros, 1), " | ", (char *)list_get(instruccion->parametros, 2), " | ", (mseg_t)instruccion->timestamp);
		list_add(listaParam, cadena);

		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
	}
	else
	{
		puts("La tabla no se encontro en Memoria. Consultando al FS");
		int conexionFS = obtener_fd_out("FileSystem");
		enviar_request(instruccion, conexionFS);
	}
}

void ejecutar_instruccion_devolucion_select(instr_t *instruccion)
{
	puts("Select realizado en FS, se guardo la siguiente tabla en la memoria:");
	print_instruccion(instruccion);
	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena, "%s%s%s%s%s%s%s%u", "Se encontro ",
										(char *)list_get(instruccion->parametros, 0), " | ",
										(char *)list_get(instruccion->parametros, 1), " | ",
										(char *)list_get(instruccion->parametros, 2), " | ",
										(unsigned int)instruccion->timestamp);
	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
}

void ejecutar_instruccion_insert(instr_t *instruccion)
{
	puts("Ejecutando instruccion Insert");
	if(0)
	{
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
	}
	else
	{
		insertar_instruccion(instruccion);

		t_list *listaParam = list_create();
		char cadena[400];
		sprintf(cadena, "%s%s%s%s%s%s%s%lu", "Se inserto ", (char *)list_get(instruccion->parametros, 0), " | ", (char *)list_get(instruccion->parametros, 1), " | ", (char *)list_get(instruccion->parametros, 2), " | ", (mseg_t)instruccion->timestamp);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);

	}
}

void ejecutar_instruccion_create(instr_t *instruccion)
{
	puts("Ejecutando instruccion Create");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_describe(instr_t *instruccion)
{
	puts("Ejecutando instruccion Describe");
	puts("La tabla no se encontro en Memoria. Consultando al FS");
	int conexionFS = obtener_fd_out("FileSystem");
	puts("Se tiene el fd_out");
	if(enviar_request(instruccion, conexionFS)==-1) puts("No se envio");
	puts("Se envio al FS");
}

void ejecutar_instruccion_drop(instr_t *instruccion)
{
	puts("Ejecutando instruccion Drop");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_exito(instr_t *instruccion)
{
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, instruccion->parametros);
}


void ejecutar_instruccion_journal(instr_t *instruccion)
{
	puts("Ejecutando instruccion Journal");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}
