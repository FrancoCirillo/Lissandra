//--------instrucciones.c--------

#include "instrucciones.h"

void ejecutar_instruccion_select(instr_t *instruccion)
{
	loggear_info(string_from_format("Ejecutando instruccion Select"));
	usleep(configuracion.RETARDO_MEMORIA * 1000);

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
			char* cadena = string_from_format(
					"Se encontro %s%s | %d | %s | %"PRIu64" en Memoria",
					puntoMontaje, tabla,
					registroEncontrado->key,
					registroEncontrado->value,
					registroEncontrado->timestamp);
			list_add(listaParam, cadena);
			se_uso(filaEncontrada->numeroDePagina);
			imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
			free(registroEncontrado->value);
			free(registroEncontrado);
			list_destroy_and_destroy_elements(instruccion->parametros, free);
			free(instruccion);

		}
		else
		{
			loggear_info(string_from_format("La key no se encontro en Memoria. Consultando al FS"));
			int conexionFS = obtener_fd_out("FileSystem");
			usleep(configuracion.RETARDO_FS * 1000);
			if(enviar_liberando_request(instruccion, conexionFS)==-1)
				loggear_error(string_from_format("No se logro enviar el Select al FS"));
		}
	}
	else
	{
		loggear_info(string_from_format("La tabla no se encontro en Memoria. Consultando al FS"));
		int conexionFS = obtener_fd_out("FileSystem");
		usleep(configuracion.RETARDO_FS * 1000);
		if(enviar_liberando_request(instruccion, conexionFS)==-1){
			loggear_error(string_from_format("No se logro enviar el Select al FS"));
		}
	}


}

void ejecutar_instruccion_devolucion_select(instr_t *instruccion)
{
	loggear_debug(string_from_format("FS devolvio la tabla solicitada."));
	t_list* listaABorrar = list_duplicate(instruccion->parametros);
	int paginaInsertada = ejecutar_instruccion_insert(instruccion, false);
	se_uso(paginaInsertada);
	t_list *listaParam = list_create();
	char* cadena = string_from_format(
			"Se encontro %s%s | %s | %s | %"PRIu64" en FS",
			puntoMontaje, (char *)list_get(instruccion->parametros, 0), //Tabla
			(char *)list_get(instruccion->parametros, 1),//Key
			(char *)list_get(instruccion->parametros, 2), //Value
			(mseg_t)instruccion->timestamp); //Timestamp

	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
	list_destroy_and_destroy_elements(listaABorrar, free);
}

int ejecutar_instruccion_insert(instr_t *instruccion, bool flagMod) //Si se inserta desde FS no tiene el flagMod
{
	if(flagMod) loggear_info(string_from_format("Ejecutando instruccion Insert")); //Si el flag es 0 es xq no se hizo un insert directamente, entonces que lo haga callado
	usleep(configuracion.RETARDO_MEMORIA * 1000);
	int numeroDePaginaInsertada;
	if(strlen((char *)list_get(instruccion->parametros, 2))>tamanioValue)
	{
		t_list *listaParam = list_create();
		char* cadena = string_from_format("El tamanio del value introducido (%d) es mayor al tamanio admitido (%d)",strlen((char *)list_get(instruccion->parametros, 2)), tamanioValue);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam);
		free(instruccion); //TODO: list_destroy(instruccion->parametros);
		return -1;
	}
	else
	{
		int numeroDePaginaAgregado;
		t_list *suTablaDePaginas = segmento_de_esa_tabla((char *)list_get(instruccion->parametros, 0));

//CASO 1:
		if(suTablaDePaginas == NULL){ //No existia un segmento correspondiente a esa tabla
			void *paginaAgregada = NULL;
			paginaAgregada = insertar_instruccion_en_memoria(instruccion, &numeroDePaginaAgregado);
			if(paginaAgregada !=NULL){
				char* paginaShow = pagina_a_str(paginaAgregada);
				loggear_debug(string_from_format("\nPagina agregada: \n%s\n", paginaShow));
				free(paginaShow);
				suTablaDePaginas = nueva_tabla_de_paginas();
				dictionary_put(tablaDeSegmentos, (char *)list_get(instruccion->parametros, 0), suTablaDePaginas);

				filaTabPags * filaAgregada = agregar_fila_tabla(suTablaDePaginas, numeroDePaginaAgregado, paginaAgregada, flagMod);
				loggear_trace(string_from_format("\nTabla de paginas actual: (Nueva)"));
				loggear_tabla_de_paginas(suTablaDePaginas, loggear_trace);
				loggear_trace(string_from_format(" ~~~~~~~~~~~~~~~~~~~~\n"));

				numeroDePaginaInsertada = filaAgregada->numeroDePagina;
			}
			else{ //Se ejecuto el Journal, se reintenta insertar
				return ejecutar_instruccion_insert(instruccion, flagMod);
			}
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
				loggear_trace(string_from_format("\nTabla de paginas actual: (Key preexistente)"));
				loggear_tabla_de_paginas(suTablaDePaginas, loggear_trace);
				loggear_trace(string_from_format("~~~~~~~~~~~~~~~~~~~~\n"));

				numeroDePaginaInsertada = filaEncontrada->numeroDePagina;
			}


//CASO 3:
			else{ //No existia la key en ese segment
				void *paginaAgregada = insertar_instruccion_en_memoria(instruccion, &numeroDePaginaAgregado);
				if(paginaAgregada != NULL){
					char* paginaStr = pagina_a_str(paginaAgregada);
					loggear_trace(string_from_format("\nPagina agregada: \n%s\n", paginaStr));
					free(paginaStr);
					filaTabPags * filaAgregada = agregar_fila_tabla(suTablaDePaginas, numeroDePaginaAgregado, paginaAgregada, flagMod);
					loggear_trace(string_from_format("Tabla de paginas actual: (Fila nueva)"));
					loggear_tabla_de_paginas(suTablaDePaginas, loggear_trace);
					loggear_trace(string_from_format("~~~~~~~~~~~~~~~~~~~~\n"));

					numeroDePaginaInsertada = filaAgregada->numeroDePagina;
				}
				else{//Se ejecuto el Journal, se reintenta insertar
					return ejecutar_instruccion_insert(instruccion, flagMod);
				}
			}

		}
		if(flagMod){
			t_list *listaParam = list_create();
			char *cadena = string_from_format(
					"Se inserto %s%s | %s | %s | %"PRIu64" en la Memoria",
					puntoMontaje, (char *)list_get(instruccion->parametros, 0),
					(char *)list_get(instruccion->parametros, 1),
					(char *)list_get(instruccion->parametros, 2),
					(mseg_t)instruccion->timestamp);
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
		}

		list_destroy_and_destroy_elements(instruccion->parametros, free);
		free(instruccion);
		return numeroDePaginaInsertada;
	}
}

void ejecutar_instruccion_create(instr_t *instruccion)
{
	loggear_info(string_from_format("Ejecutando instruccion Create"));
	int conexionFS = obtener_fd_out("FileSystem");
	usleep(configuracion.RETARDO_FS * 1000);
	if(enviar_liberando_request(instruccion, conexionFS)==-1)
		loggear_error(string_from_format("No se envio el Create al FS"));
	loggear_trace(string_from_format("Se borraron los parametros del create"));
}

void ejecutar_instruccion_describe(instr_t *instruccion, char* remitente)
{
	loggear_info(string_from_format("Ejecutando instruccion Describe"));
	int conexionFS = obtener_fd_out("FileSystem");
	usleep(configuracion.RETARDO_FS * 1000);
	if(enviar_liberando_request(instruccion, conexionFS)==-1)
		loggear_error(string_from_format("No se envio el Describe al FS"));
	loggear_trace(string_from_format("Se borraron los parametros del Describe"));
}

void ejecutar_instruccion_drop(instr_t *instruccion)
{
	usleep(configuracion.RETARDO_MEMORIA * 1000);
	loggear_info(string_from_format("Ejecutando instruccion Drop"));

	eliminar_tabla(instruccion);
	int conexionFS = obtener_fd_out("FileSystem");
	usleep(configuracion.RETARDO_FS * 1000);
	if(enviar_liberando_request(instruccion, conexionFS)==-1)
		loggear_error(string_from_format("No se envio el Drop al FS"));
	loggear_trace(string_from_format("Se borraron los parametros del Drop"));
}

void ejecutar_instruccion_exito(instr_t *instruccion)
{
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, instruccion->parametros);
	free(instruccion);
}

void ejecutar_instruccion_error(instr_t * instruccion)
{
	imprimir_donde_corresponda(instruccion->codigo_operacion, instruccion, instruccion->parametros);
	free(instruccion);
}

void sleep_acceso_memoria(){
	sem_wait(&mutex_config);
	int retardo = configuracion.RETARDO_MEMORIA;
	sem_post(&mutex_config);
	usleep(retardo* 1000);
}

void sleep_acceso_fs(){
	sem_wait(&mutex_config);
	int retardo = configuracion.RETARDO_FS;
	sem_post(&mutex_config);
	usleep(retardo* 1000);
}
