//--------instrucciones.c--------

#include "instrucciones.h"


void ejecutar_instruccion_select(instr_t *instruccion)
{
	puts("Ejecutando instruccion Select");
	char* tabla = (char *)list_get(instruccion->parametros, 0);
	t_list *suTablaDePaginas = segmento_de_esa_tabla(tabla);
	if (suTablaDePaginas != NULL) //El segmento ya existia, se encontro su tabla de paginas
	{
		char* keyChar = (char *)list_get(instruccion->parametros, 1);
		uint16_t keyBuscada;
		str_to_uint16(keyChar, &keyBuscada);
		filaTabPags* filaEncontrada = fila_con_la_key(suTablaDePaginas, keyBuscada);
		if(filaEncontrada != NULL)
		{	//"La key pertenece a una fila preexistente"
			registro* registroEncontrado = obtener_registro_de_pagina(filaEncontrada->ptrPagina);
			t_list *listaParam = list_create();
			char cadena[400];
			sprintf(cadena, "Se encontro %s | %d | %s | %"PRIu64" en Memoria",
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
			log_debug(debug_logger, "La key no se encontro en Memoria. Consultando al FS");
			int conexionFS = obtener_fd_out("FileSystem");
			enviar_request(instruccion, conexionFS);
		}
	}
	else
	{
		log_debug(debug_logger, "La tabla no se encontro en Memoria. Consultando al FS");
		int conexionFS = obtener_fd_out("FileSystem");
		enviar_request(instruccion, conexionFS);
	}
}

void ejecutar_instruccion_devolucion_select(instr_t *instruccion)
{
	log_debug(debug_logger, "FS devolvio la tabla solicitada.");
	int paginaInsertada = ejecutar_instruccion_insert(instruccion, false);
	se_uso(paginaInsertada);
	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena,
			"Se encontro %s | %s | %s | %"PRIu64" en FS",
			(char *)list_get(instruccion->parametros, 0), //Tabla
			(char *)list_get(instruccion->parametros, 1),//Key
			(char *)list_get(instruccion->parametros, 2), //Value
			(mseg_t)instruccion->timestamp); //Timestamp

	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
}

int ejecutar_instruccion_insert(instr_t *instruccion, bool flagMod) //Si se inserta desde FS no tiene el flagMod
{
	if(flagMod) log_info(g_logger, "Ejecutando instruccion Insert"); //Si el flag es 0 es xq no se hizo un insert directamente, entonces que lo haga callado

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
			log_debug(debug_logger, "\nPagina agregada: \n%s\n", pagina_a_str(paginaAgregada));

			suTablaDePaginas = nueva_tabla_de_paginas();
			dictionary_put(tablaDeSegmentos, (char *)list_get(instruccion->parametros, 0), suTablaDePaginas);

			filaTabPags * filaAgregada = agregar_fila_tabla(suTablaDePaginas, numeroDePaginaAgregado, paginaAgregada, flagMod);
			log_debug(debug_logger, "\nTabla de paginas actual: (Nueva)");
			imprimir_tabla_de_paginas(suTablaDePaginas);
			log_debug(debug_logger, " ~~~~~~~~~~~~~~~~~~~~\n");

			numeroDePaginaInsertada = filaAgregada->numeroDePagina;
		}


		else{ //Ya existía el segmento correspondiente a la página, hay que ver si ya existia la key

			char* keyChar = (char *)list_get(instruccion->parametros, 1);
			uint16_t keyBuscada;
			str_to_uint16(keyChar, &keyBuscada);
			filaTabPags* filaEncontrada = fila_con_la_key(suTablaDePaginas,keyBuscada);
//CASO 2:
			if(filaEncontrada !=NULL){ // Ya existia la key en ese segmento
				mseg_t nuevoTimestamp = instruccion->timestamp;
				char* nuevoValue = (char *) list_get(instruccion->parametros, 2);
				actualizar_pagina(filaEncontrada->ptrPagina, nuevoTimestamp, nuevoValue);
				filaEncontrada->flagModificado = flagMod;
				//La fila de la tabla de paginas no se modifica, porque guarda un puntero a la pagina
				log_debug(debug_logger, "\nTabla de paginas actual: (Key preexistente)");
				imprimir_tabla_de_paginas(suTablaDePaginas);
				log_debug(debug_logger, "~~~~~~~~~~~~~~~~~~~~\n");

				numeroDePaginaInsertada = filaEncontrada->numeroDePagina;
			}


//CASO 3:
			else{ //No existia la key en ese segment
				void *paginaAgregada = insertar_instruccion_en_memoria(instruccion, &numeroDePaginaAgregado);
				log_debug(debug_logger, "\nPagina agregada: \n%s\n", pagina_a_str(paginaAgregada));
				filaTabPags * filaAgregada = agregar_fila_tabla(suTablaDePaginas, numeroDePaginaAgregado, paginaAgregada, flagMod);
				log_debug(debug_logger, "Tabla de paginas actual: (Fila nueva)");
				imprimir_tabla_de_paginas(suTablaDePaginas);
				log_debug(debug_logger, "~~~~~~~~~~~~~~~~~~~~\n");

				numeroDePaginaInsertada = filaAgregada->numeroDePagina;
			}



		}
		if(flagMod){
			char cadena[500];
			t_list *listaParam = list_create();
			sprintf(cadena, "Se inserto %s | %s | %s | %"PRIu64" en la Memoria",
					(char *)list_get(instruccion->parametros, 0),
					(char *)list_get(instruccion->parametros, 1),
					(char *)list_get(instruccion->parametros, 2),
					(mseg_t)instruccion->timestamp);
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
		}
			return numeroDePaginaInsertada;
	}
}

void ejecutar_instruccion_create(instr_t *instruccion)
{
	log_info(g_logger, "Ejecutando instruccion Create");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_describe(instr_t *instruccion)
{
	log_info(g_logger, "Ejecutando instruccion Describe");
	log_debug(debug_logger, "La tabla no se encontro en Memoria. Consultando al FS");
	int conexionFS = obtener_fd_out("FileSystem");
	log_debug(debug_logger, "Se tiene el fd_out");
	if(enviar_request(instruccion, conexionFS)==-1) log_error(g_logger, "No se envio el Describe al FS");
	log_debug(debug_logger, "Se envio al FS");
}

void ejecutar_instruccion_drop(instr_t *instruccion)
{
	log_info(g_logger, "Ejecutando instruccion Drop");

	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_exito(instr_t *instruccion)
{
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, instruccion->parametros);
}

void ejecutar_instruccion_error(instr_t * instruccion)
{
	imprimir_donde_corresponda(instruccion->codigo_operacion, instruccion, instruccion->parametros);
}
