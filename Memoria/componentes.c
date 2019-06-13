//--------componentes.c--------

#include "componentes.h"

void gran_malloc_inicial()
{
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	memoriaPrincipal = malloc(tamanioMemoria);
	if (memoriaPrincipal == NULL)
	{
		loggear_error(g_logger, &mutex_log, string_from_format("No se pudo reservar espacio para la memoria principal"));
	}
	loggear_debug(debug_logger, &mutex_log, string_from_format("Espacio para la memoria principal reservada"));
}
void inicializar_tabla_segmentos()
{
	tablaDeSegmentos = dictionary_create();
	paginasSegunUso = list_create();
}

void inicializar_sectores_memoria()
{
	tamanioRegistro = sizeof(mseg_t) + sizeof(uint16_t) + tamanioValue;
	cantidadDeSectores = configuracion.TAMANIO_MEMORIA / tamanioRegistro; //Se trunca automaticamente al entero (por ser todos int)

	loggear_debug(debug_logger, &mutex_log,
			string_from_format("cantidadDeSectores = TAMANIO_MEMORIA / tamanioRegistro\n%d = %d / %d", cantidadDeSectores, configuracion.TAMANIO_MEMORIA, tamanioRegistro));

	sectorOcupado = malloc(cantidadDeSectores * sizeof(bool));
	memset(sectorOcupado, false, cantidadDeSectores * sizeof(bool));
}

t_list *nueva_tabla_de_paginas()
{
	t_list *tablaDePaginas = list_create();

	return tablaDePaginas;
}

filaTabPags *agregar_fila_tabla(t_list *tablaDePaginas, int numPag, void *pagina, bool flagMod)
{
	filaTabPags *tabla = malloc(sizeof(filaTabPags)); //free al hacer DROP, JOURNAL o LRU

	tabla->numeroDePagina = numPag;
	tabla->ptrPagina = pagina;
	tabla->flagModificado = flagMod;
	list_add(tablaDePaginas, tabla);

	return tabla;
}

void *insertar_instruccion_en_memoria(instr_t *instruccion, int *nroPag)
{
	int desplazamiento = 0;
	registro *reg = obtener_registro_de_instruccion(instruccion);
	int sectorDisponible = get_proximo_sector_disponible();
	if (sectorDisponible == -1)
	{
		if (memoria_esta_full())
		{
			ejecutar_instruccion_journal(instruccion);
		}
		else
		{ //Algoritmo de reemplazo:
			loggear_debug(debug_logger, &mutex_log, string_from_format("Ejecutando el algoritmo de reemplazo"));
			int *numeroDeSector = pagina_lru();

			int indiceEnTabla = 0;

			//Buscando la fila correspondiente a la pagina menos usada
			char *segmentoConFilaABorrar;
			fila_correspondiente_a_esa_pagina((*numeroDeSector), &indiceEnTabla, &segmentoConFilaABorrar);

			//El segmento (Tabla) al que corresponde esa pagina es segmentoConFilaABorrar
			t_list *suTablaDePaginas = dictionary_get(tablaDeSegmentos, segmentoConFilaABorrar);

			//Modificando esa pagina (Esto tambien podría hacerse al principio del algoritmo, es lo mismo)
			desplazamiento += ((*numeroDeSector) * tamanioRegistro);
			memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
			desplazamiento += sizeof(mseg_t);
			memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
			desplazamiento += sizeof(uint16_t);
			memcpy(memoriaPrincipal + desplazamiento, &reg->value, tamanioValue);
			*nroPag = (*numeroDeSector);

			//Borrando la fila "indiceEnTabla" de la Tabla de paginas "segmentoConFilaABorrar"
			list_remove_and_destroy_element(suTablaDePaginas, indiceEnTabla, (void *)free);
			if (list_is_empty(suTablaDePaginas))
			{
				//La tabla de páginas quedó vacía, no tiene sentido guardar el puntero
				dictionary_remove_and_destroy(tablaDeSegmentos, segmentoConFilaABorrar, (void *)free);
			}
			free(segmentoConFilaABorrar); //malloc en fila_correspondiente_a_esa_pagina
			free(reg); //malloc en obtener_registro_de_instruccion
			return memoriaPrincipal + ((*numeroDeSector) * tamanioRegistro);
		}
	}
	else
	{
		sectorOcupado[sectorDisponible] = true;
		desplazamiento += (sectorDisponible * tamanioRegistro);
		memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
		desplazamiento += sizeof(mseg_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
		desplazamiento += sizeof(uint16_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->value, tamanioValue);
		*nroPag = sectorDisponible;
		free(reg);
		return memoriaPrincipal + (sectorDisponible * tamanioRegistro);
	}

	loggear_error(g_logger, &mutex_log, string_from_format("Error al insertrar la pagina en Memoria"));
	return NULL;
}

void actualizar_pagina(void *paginaAActualizar, mseg_t nuevoTimestamp, char *nuevoValue)
{

	int desplazamiento = 0;
	memcpy(paginaAActualizar + desplazamiento, &nuevoTimestamp, sizeof(mseg_t));
	desplazamiento += sizeof(mseg_t);
	//	memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t)); La key no se mofidica
	desplazamiento += sizeof(uint16_t);
	memcpy(paginaAActualizar+ desplazamiento, &nuevoValue, tamanioValue);
}
registro *obtener_registro_de_instruccion(instr_t *instruccion)
{
	char *keyChar = (char *)list_get(instruccion->parametros, 1);
	char *valueNuevo = (char *)list_get(instruccion->parametros, 2);
	mseg_t timestampNuevo = instruccion->timestamp;
	uint16_t keyNueva;
	str_to_uint16(keyChar, &keyNueva);

	registro registroCreado = {
		.timestamp = timestampNuevo,
		.key = keyNueva,
		.value = valueNuevo};

	registro *miReg = malloc(sizeof(registroCreado));
	memcpy(miReg, &registroCreado, sizeof(registroCreado));

	return miReg;
}

int get_proximo_sector_disponible()
{
	for (int sector = 0; sector < cantidadDeSectores; sector++)
	{
		if (sectorOcupado[sector] == false)
		{
			return sector;
		}
	}
	return -1;
}

mseg_t get_ts_pagina(void *pagina)
{
	mseg_t timestampPagina;
	memcpy(&timestampPagina, pagina, sizeof(mseg_t));
	return timestampPagina;
}

uint16_t get_key_pagina(void *pagina)
{
	uint16_t keyPagina;
	memcpy(&keyPagina, pagina + sizeof(mseg_t), sizeof(uint16_t));
	return keyPagina;
}

char *get_value_pagina(void *pagina)
{
	char *keyPagina = malloc(tamanioValue);
	memcpy(&keyPagina, pagina + sizeof(mseg_t) + sizeof(uint16_t), sizeof(tamanioValue));
	return keyPagina;
}

registro *obtener_registro_de_pagina(void *pagina)
{
	mseg_t timestamp = get_ts_pagina(pagina);
	uint16_t key = get_key_pagina(pagina);
	char *value = get_value_pagina(pagina);

	registro *miRegistro = malloc(tamanioRegistro + 1);
	miRegistro->timestamp = timestamp;
	miRegistro->key = key;
	miRegistro->value = value;

	return miRegistro;
}

char *registro_a_str(registro *registro)
{
	char* regString = string_from_format("	Timestamp:	%"PRIu64"\n	Key:		%u\n	Value:		%s\n", registro->timestamp, registro->key, registro->value);
	return regString;
}

char *pagina_a_str(void *pagina)
{
	return registro_a_str(obtener_registro_de_pagina(pagina));
}

void loggear_tabla_de_paginas(t_list *tablaDePaginas, t_log* logger)
{

	char* texto = string_new();

	void iterator(filaTabPags * fila)
	{
		char* paginaStr = pagina_a_str(fila->ptrPagina);
		string_append_with_format(&texto, " ~~~~~~~~~~~~~~~~~~~~\n"); //No le gusta todo en un solo append
		string_append_with_format(&texto, " Numero de pagina: %d\n", fila->numeroDePagina);
		string_append_with_format(&texto, " Puntero a pagina: %p\n", fila->ptrPagina);
		string_append_with_format(&texto, "  Pagina en memo: \n%s", paginaStr);
		string_append_with_format(&texto, " Flag modificado : %d\n", fila->flagModificado);
		free(paginaStr);
	}

	list_iterate(tablaDePaginas, (void *)iterator);
	loggear_info(logger, &mutex_log, texto);
}

void se_uso(int paginaUtilizada){

	int *paginaUsada = malloc(sizeof(int));
	*paginaUsada = paginaUtilizada;

	bool esPaginaRequerida(int *numeroDePagina)
	{
		if (*paginaUsada == *numeroDePagina)
		{
			return true;
		}
		else
			return false;
	}

	if ((list_find(paginasSegunUso, (void *)esPaginaRequerida)) != NULL)
	{
		list_remove_and_destroy_by_condition(paginasSegunUso, (void *)esPaginaRequerida, (void *)free);
		//En list[0] queda el que menos se usa
	}
	list_add(paginasSegunUso, paginaUsada); //Lo agrega en el último lugar, si ya existia se duplica

}

t_list *segmento_de_esa_tabla(char *tabla)
{
	return dictionary_get(tablaDeSegmentos, tabla);
}

bool tabla_de_paginas_full(t_list *tablaDePaginas)
{
	bool esta_modificado(filaTabPags * fila)
	{
		return fila->flagModificado;
	}
	return list_all_satisfy(tablaDePaginas, (void *)esta_modificado);
}

bool memoria_esta_full()
{
	//Un dictionary_all casero
	int cantTablasFull = 0;
	void esta_full(char *segmento, t_list *tablaDePaginas)
	{
		if (tabla_de_paginas_full(tablaDePaginas))
		{
			cantTablasFull++;
		}
	}

	dictionary_iterator(tablaDeSegmentos, (void *)esta_full);
	int cantidadSegmentosTotal = dictionary_size(tablaDeSegmentos);

	if (cantTablasFull == cantidadSegmentosTotal)
	{ //Porque cada segmento tiene su correspondiente tabla de paginas
		return true;
	}
	return false;
}

int *pagina_lru()
{
	return (int*) list_get(paginasSegunUso, 0);
}


filaTabPags *fila_con_el_numero(t_list *suTablaDePaginas, int numeroDePagina, int *indiceEnTabla)
{

	bool tiene_el_numero(filaTabPags * fila)
	{
		if (fila->numeroDePagina == numeroDePagina)
		{
			return 1;
		}

		else
		{
			(*indiceEnTabla)++;
			return 0;
		}
	}

	return list_find(suTablaDePaginas, (void *)tiene_el_numero);
}

filaTabPags *fila_con_la_key(t_list *suTablaDePaginas, uint16_t keyBuscada){

	bool tiene_la_key(filaTabPags * fila)
	{
		if (get_key_pagina(fila->ptrPagina) == keyBuscada)
		{
			return 1;
		}

		else
		{
			return 0;
		}
	}

	return list_find(suTablaDePaginas, (void *)tiene_la_key);
}

filaTabPags *fila_correspondiente_a_esa_pagina(int numeroDePagina, int *indiceEnTabla, char** segmentoQueLaTiene)
{
	//Un dictionary_find casero

	filaTabPags *filaEncontrada;
	filaTabPags *filaPosible;
	int seEncontro = 0;

	void tiene_esa_pagina_la_fila(char *segmento, t_list *suTablaDePaginas)
	{
		if(seEncontro == 0)
		{
			*indiceEnTabla= 0;
			filaPosible = fila_con_el_numero(suTablaDePaginas, numeroDePagina, indiceEnTabla);
			if (filaPosible != NULL)
			{ //Se encontro la fila que tenia ese numero
				seEncontro = 1;
				filaEncontrada = filaPosible;
				*segmentoQueLaTiene = malloc(sizeof(segmento));
				*segmentoQueLaTiene = strcpy(*segmentoQueLaTiene, segmento);
			}

		}
	}

	dictionary_iterator(tablaDeSegmentos, (void *)tiene_esa_pagina_la_fila);

	if (filaPosible == NULL)
	{
		loggear_error(g_logger, &mutex_log, string_from_format("No se encontro la fila con la pagina")); //Durante el algoritmo de reemplazo nunca se debería dar
		*segmentoQueLaTiene = NULL;
		return NULL;
	}
	else
		return filaEncontrada;
}



void ejecutar_instruccion_journal(instr_t *instruccion)
{
	loggear_info(g_logger, &mutex_log, string_from_format("Ejecutando instruccion Journal"));
	int conexionConFS = obtener_fd_out("FileSystem");

	loggear_debug(debug_logger, &mutex_log, string_from_format("Se encontro el fd_out"));
	void enviar_paginas_modificadas(char *segmento, t_list *suTablaDePaginas)
	{
		char* tablaAInsertar = string_from_format("%s", segmento);

		printf("Tabla a journalear: %s\n", tablaAInsertar);
		loggear_debug(debug_logger, &mutex_log, string_from_format("Chequeando una tabla de paginas"));
		void enviar_si_esta_modificada(filaTabPags * fila)
		{
			loggear_debug(debug_logger, &mutex_log, string_from_format("Chequeando tiene una fila modificada"));
			if(fila->flagModificado)
			{
				loggear_debug(debug_logger, &mutex_log, string_from_format("Se encontro una pagina modificada"));
				cod_op codOp = CODIGO_INSERT;

				if(quien_pidio(instruccion) == CONSOLA_KERNEL)
				{
					loggear_debug(debug_logger, &mutex_log, string_from_format("El journal lo pidio Kernel"));
					codOp += BASE_CONSOLA_KERNEL;
				}
				else
				{
					loggear_debug(debug_logger, &mutex_log, string_from_format("El journal lo pidio Memoria"));
					codOp += BASE_CONSOLA_MEMORIA;
				}

				instr_t* instruccionAEnviar = fila_a_instr(tablaAInsertar, fila, codOp);
				loggear_debug(debug_logger, &mutex_log, string_from_format("Se genero la instruccion a enviar"));
				enviar_request(instruccionAEnviar, conexionConFS);
				list_destroy(instruccionAEnviar->parametros); //No hacemos free a sus elementos xq son punteros a la Memoria Principal
				loggear_debug(debug_logger, &mutex_log, string_from_format("Se vacio la lista y se destruyeron sus elementos"));
				free(instruccionAEnviar);
				loggear_debug(debug_logger, &mutex_log, string_from_format("free(instruccionAEnviar)"));
			}
		}

		list_iterate(suTablaDePaginas, (void *)enviar_si_esta_modificada);

		free(tablaAInsertar); //malloc en string_from_format
		loggear_debug(debug_logger, &mutex_log, string_from_format("free(tablaAInsertar)"));
	}

	dictionary_iterator(tablaDeSegmentos, (void *)enviar_paginas_modificadas);

	limpiar_memoria();

	loggear_debug(debug_logger, &mutex_log, string_from_format("Se recorrieron todas las paginas"));
	t_list *listaParam = list_create();
	char *cadena = "Journal realizado.";
	list_add(listaParam, cadena);
	cod_op codOp = CODIGO_EXITO;
	imprimir_donde_corresponda(codOp, instruccion, listaParam);
}

instr_t	 *fila_a_instr(char* tablaAInsertar, filaTabPags* fila, cod_op codOp){
	registro* suRegistro = obtener_registro_de_pagina(fila->ptrPagina);
	loggear_debug(debug_logger, &mutex_log, string_from_format("Se tienen los datos de la pagina"));
	return registro_a_instr(tablaAInsertar, suRegistro, codOp);
}

instr_t *registro_a_instr(char* tablaAInsertar,registro* unRegistro, cod_op codOp){
	loggear_debug(debug_logger, &mutex_log, string_from_format("Registro: %s\n", registro_a_str(unRegistro)));
	t_list* listaParam  = list_create();
	loggear_debug(debug_logger, &mutex_log, string_from_format("Lista param creada\n"));
	list_add(listaParam, tablaAInsertar);
	loggear_debug(debug_logger, &mutex_log, string_from_format("Nombre de la tabla a insertar agregado\n"));
	char* keyChar = string_from_format("%d", unRegistro->key);
	list_add(listaParam, keyChar);

	list_add(listaParam, unRegistro->value);

	instr_t* instruccionCreada = crear_instruccion(unRegistro->timestamp, codOp, listaParam);
	return instruccionCreada;
}

void limpiar_memoria(){
	loggear_debug(debug_logger, &mutex_log, string_from_format("Limpiando mem ppal"));
	limpiar_memoria_principal();
	loggear_debug(debug_logger, &mutex_log, string_from_format("Limpiando segmentos"));
	limpiar_segmentos();
	loggear_debug(debug_logger, &mutex_log, string_from_format("Segmentos limpios"));
}


void limpiar_memoria_principal(){
	memset(sectorOcupado, false, cantidadDeSectores * sizeof(bool)); //Ahora indica que todos los sectores de la memoria se pueden sobreescribir
	//Supongo que no hace falta que hagamos un memset en 0 a la Memoria Principal en si
}

void limpiar_segmentos(){

	void limpiar_tabla_de_paginas(char *segmento, t_list *suTablaDePaginas)
		{
			list_destroy_and_destroy_elements(suTablaDePaginas, (void*)free);
		}

	dictionary_iterator(tablaDeSegmentos, (void *)limpiar_tabla_de_paginas);

	dictionary_clean(tablaDeSegmentos); //No dictionary_clean_and_destroy_elements porque ya los limpio arriba
}


void *ejecutar_journal()
{

    t_list *listaParam = list_create();
    int i = 0;
    list_add(listaParam, &i);
    instr_t* miInstruccion = crear_instruccion(obtener_ts(), CONSOLA_MEM_JOURNAL, listaParam);

	while(1)
	{
		usleep(configuracion.RETARDO_JOURNAL * 1000);
		sem_wait(&mutex_journal);
		ejecutar_instruccion_journal(miInstruccion);
		sem_post(&mutex_journal);
	}
}

void *ejecutar_gossiping()
{
	while(1)
	{
		usleep(configuracion.RETARDO_GOSSIPING * 1000);
		loggear_debug(g_logger, &mutex_log, string_from_format("Realizando gossiping programado"));
		ejecutar_instruccion_gossip();
	}
}



int eliminar_tabla(instr_t* instruccion)
{
	char* tablaABorrar = (char*)list_get(instruccion->parametros, 0);
	t_list* segmentoABorrar = segmento_de_esa_tabla(tablaABorrar);

	dictionary_remove(tablaDeSegmentos, tablaABorrar);
	list_destroy_and_destroy_elements(segmentoABorrar, (void*)free);

	return 0;
}


void imprimir_segmento(char* nombreSegmento, t_list* suTablaDePaginas){

	loggear_info(g_logger, &mutex_log, string_from_format("Tabla: %s\n", nombreSegmento));
	loggear_tabla_de_paginas(suTablaDePaginas, g_logger);
}

void imprimir_segmento_basico(char* nombreSegmento, t_list* suTablaDePaginas){
	char* texto = string_new();
	string_append_with_format(&texto, "------------------------\n");
	string_append_with_format(&texto, "Tabla: %s\n", nombreSegmento);

	void iterator(filaTabPags* fila){
		char* paginaStr = pagina_a_str(fila->ptrPagina);
		string_append(&texto, paginaStr);
		free(paginaStr);
	}
	list_iterate(suTablaDePaginas, (void *)iterator);
	loggear_info(g_logger, &mutex_log, texto);
}

void mostrar_paginas(instr_t* instruccion){
	if(list_get(instruccion->parametros, 0) != NULL){
		dictionary_iterator(tablaDeSegmentos,(void*) imprimir_segmento);
	}
	else{
		dictionary_iterator(tablaDeSegmentos,(void*) imprimir_segmento_basico);
	}
}
