
#include "instrucciones.h"

registro* encontrar_value(t_list *suTablaDePaginas, char* keyChar){
	uint16_t keyPedida;
			str_to_uint16(keyChar, &keyPedida);
			uint16_t keyEnMemoria;

			for(int i = 0;i<list_size(suTablaDePaginas); i++){
				filaTabPags * fila = list_get(suTablaDePaginas, i);
				keyEnMemoria = get_key_pagina(fila->ptrPagina);
				if(keyEnMemoria == keyPedida){
					return obtener_registro_de_pagina(fila->ptrPagina);
				}
			}
	return NULL;
}

void ejecutar_instruccion_select(instr_t *instruccion)
{
	puts("Ejecutando instruccion Select");
	char* tabla = (char *)list_get(instruccion->parametros, 0);
	t_list *suTablaDePaginas = dictionary_get(tablaDeSegmentos, tabla);
	sleep(1);			//Buscar
	if (suTablaDePaginas != NULL)
	{
		char* keyChar = (char *)list_get(instruccion->parametros, 1);
		registro* registroEncontrado = encontrar_value(suTablaDePaginas, keyChar);
		if(registroEncontrado != NULL)
		{
			t_list *listaParam = list_create();
			char cadena[400];
			sprintf(cadena, "Se encontro %s | %d | %s | %lu en Memoria",
					tabla,
					registroEncontrado->key,
					registroEncontrado->value,
					registroEncontrado->timestamp);
			list_add(listaParam, cadena);

			imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);

		}
		else
		{
			puts("La key no se encontro en Memoria. Consultando al FS");
			int conexionFS = obtener_fd_out("FileSystem");
			enviar_request(instruccion, conexionFS);
		}
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
	sprintf(cadena, "%s%s%s%s%s%s%s%lu en FS", "Se encontro",
										(char *)list_get(instruccion->parametros, 0), " | ",
										(char *)list_get(instruccion->parametros, 1), " | ",
										(char *)list_get(instruccion->parametros, 2), " | ",
										(mseg_t)instruccion->timestamp);
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
		if(strlen((char *)list_get(instruccion->parametros, 2))>tamanioValue)
		{
			char cadena[500];
			t_list *listaParam = list_create();
			sprintf(cadena, "El tamanio del value introducido (%d) es mayor al tamanio admitido (%d)",strlen((char *)list_get(instruccion->parametros, 2)), tamanioValue);
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam);
		}
		else
		{
			void *paginaAgregada = insertar_instruccion_en_memoria(instruccion);
			printf("\nPagina agregada: %s\n", pagina_a_str(paginaAgregada));
			t_list *suTablaDePaginas = dictionary_get(tablaDeSegmentos, (char *)list_get(instruccion->parametros, 0));
			if(suTablaDePaginas == NULL){
				suTablaDePaginas = nueva_tabla_de_paginas();
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

			char cadena[500];
			t_list *listaParam = list_create();
			sprintf(cadena, "Se inserto %s | %s | %s | %lu a la Memoria",
					(char *)list_get(instruccion->parametros, 0),
					(char *)list_get(instruccion->parametros, 1),
					(char *)list_get(instruccion->parametros, 2),
					(mseg_t)instruccion->timestamp);
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
		}
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

void ejecutar_instruccion_error(instr_t * instruccion)
{
	imprimir_donde_corresponda(instruccion->codigo_operacion, instruccion, instruccion->parametros);
}
