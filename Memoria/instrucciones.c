
#include "instrucciones.h"


void ejecutar_instruccion_select(instr_t *instruccion)
{
	puts("Ejecutando instruccion Select");
	char* tabla = (char *)list_get(instruccion->parametros, 0);
	t_list *suTablaDePaginas = segmento_de_esa_tabla(tabla);
	if (suTablaDePaginas != NULL) //El segmento ya existia, se encontro su tabla de paginas
	{
		char* keyChar = (char *)list_get(instruccion->parametros, 1);
		filaTabPags* filaEncontrada = en_que_fila_esta_key(suTablaDePaginas, keyChar);
		if(filaEncontrada != NULL)
		{	//"La key pertenece a una fila preexistente"
			registro* registroEncontrado = obtener_registro_de_pagina(filaEncontrada->ptrPagina);
			t_list *listaParam = list_create();
			char cadena[400];
			sprintf(cadena, "Se encontro %s | %d | %s | %lu en Memoria",
					tabla,
					registroEncontrado->key,
					registroEncontrado->value,
					registroEncontrado->timestamp);
			list_add(listaParam, cadena);
			se_uso(filaEncontrada->numeroDePagina);
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
	puts("FS devolvio la tabla solicitada.");
	int paginaInsertada = ejecutar_instruccion_insert(instruccion, false);
	se_uso(paginaInsertada);
	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena, "%s%s%s%s%s%s%s%lu en FS", "Se encontro",
										(char *)list_get(instruccion->parametros, 0), " | ", //Tabla
										(char *)list_get(instruccion->parametros, 1), " | ", //Key
										(char *)list_get(instruccion->parametros, 2), " | ", //Value
										(mseg_t)instruccion->timestamp); //Timestamp
	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
}

int ejecutar_instruccion_insert(instr_t *instruccion, bool flagMod) //Si se inserta desde FS no tiene el flagMod
{
	puts("Ejecutando instruccion Insert");

	int numeroDePaginaInsertada;

	if(strlen((char *)list_get(instruccion->parametros, 2))>tamanioValue)
	{
		char cadena[500];
		t_list *listaParam = list_create();
		sprintf(cadena, "El tamanio del value introducido (%d) es mayor al tamanio admitido (%d)",strlen((char *)list_get(instruccion->parametros, 2)), tamanioValue);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam);
		return -1;
	}
	else
	{
		int numeroDePaginaAgregado;
		t_list *suTablaDePaginas = segmento_de_esa_tabla((char *)list_get(instruccion->parametros, 0));

//CASO 1:
		if(suTablaDePaginas == NULL){ //No existia un segmento correspondiente a esa tabla
			void *paginaAgregada = insertar_instruccion_en_memoria(instruccion, &numeroDePaginaAgregado);
			printf("\nPagina agregada: \n%s\n", pagina_a_str(paginaAgregada));

			suTablaDePaginas = nueva_tabla_de_paginas();
			dictionary_put(tablaDeSegmentos, (char *)list_get(instruccion->parametros, 0), suTablaDePaginas);

			filaTabPags * filaAgregada = agregar_fila_tabla(suTablaDePaginas, numeroDePaginaAgregado, paginaAgregada, flagMod);
			puts("Tabla de paginas actual: (Nueva)");
			imprimir_tabla_de_paginas(suTablaDePaginas);
			printf(" ~~~~~~~~~~~~~~~~~~~~\n");

			numeroDePaginaInsertada = filaAgregada->numeroDePagina;
		}


		else{ //Ya existía el segmento correspondiente a la página, hay que ver si ya existia la key

			char* keyChar = (char *)list_get(instruccion->parametros, 1);
			filaTabPags* filaEncontrada = en_que_fila_esta_key(suTablaDePaginas, keyChar);
//CASO 2:
			if(filaEncontrada !=NULL){ // Ya existia la key en ese segmento
				mseg_t nuevoTimestamp = instruccion->timestamp;
				char* nuevoValue = (char *) list_get(instruccion->parametros, 2);
				actualizar_pagina(filaEncontrada->ptrPagina, nuevoTimestamp, nuevoValue);
				//La fila de la tabla de paginas no se modifica, porque guarda un puntero a la pagina
				puts("Tabla de paginas actual: (Key preexistente)");
				imprimir_tabla_de_paginas(suTablaDePaginas);
				printf(" ~~~~~~~~~~~~~~~~~~~~\n");

				numeroDePaginaInsertada = filaEncontrada->numeroDePagina;
			}


//CASO 3:
			else{ //No existia la key en ese segment
				void *paginaAgregada = insertar_instruccion_en_memoria(instruccion, &numeroDePaginaAgregado);
				printf("\nPagina agregada: \n%s\n", pagina_a_str(paginaAgregada));
				filaTabPags * filaAgregada = agregar_fila_tabla(suTablaDePaginas, numeroDePaginaAgregado, paginaAgregada, flagMod);
				puts("Tabla de paginas actual: (Fila nueva)");
				imprimir_tabla_de_paginas(suTablaDePaginas);
				printf(" ~~~~~~~~~~~~~~~~~~~~\n");

				numeroDePaginaInsertada = filaAgregada->numeroDePagina;
			}



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
		return numeroDePaginaInsertada;
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
