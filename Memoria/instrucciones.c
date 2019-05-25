
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
		void *paginaAgregada = insertar_instruccion_en_memoria(instruccion);
//		puts("Se agrego la pagina a la memoria principal");
		printf("\nPagina agregada: %s\n", pagina_a_str(paginaAgregada));
		t_list *suTablaDePaginas = dictionary_get(tablaDeSegmentos, (char *)list_get(instruccion->parametros, 0));
		if(suTablaDePaginas == NULL){
			suTablaDePaginas = nueva_tabla_de_paginas();
//			puts("Se creo una nueva tabla de paginas");
			dictionary_put(tablaDeSegmentos, (char *)list_get(instruccion->parametros, 0), suTablaDePaginas);
			printf("Se agrego %s al diccionario\n", (char *)list_get(instruccion->parametros, 0));
			agregar_fila_tabla(suTablaDePaginas, 1, paginaAgregada, 1);
			puts("Tabla de paginas actual: (New)");
			imprimir_tabla_de_paginas(suTablaDePaginas);
		}
		else{
			agregar_fila_tabla(suTablaDePaginas, 1, paginaAgregada, 1);
			puts("Tabla de paginas actual: ");
			imprimir_tabla_de_paginas(suTablaDePaginas);
		}
//		puts("Se agrego una nueba fila a la tabla de paginas");



//		char cadena[500];
//		t_list *listaParam = list_create();
//		sprintf(cadena, "%s%s%s%s%s%s%s%lu", "Se inserto ", (char *)list_get(instruccion->parametros, 0), " | ", (char *)list_get(instruccion->parametros, 1), " | ", (char *)list_get(instruccion->parametros, 2), " | ", (mseg_t)instruccion->timestamp);
//		list_add(listaParam, cadena);
//		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);

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
