//--------componentes.c--------

#include "componentes.h"

void gran_malloc_inicial()
{
	sem_wait(&mutex_config);
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	sem_post(&mutex_config);
	memoriaPrincipal = malloc(tamanioMemoria);
	if (memoriaPrincipal == NULL)
	{
		loggear_error(string_from_format("No se pudo reservar espacio para la memoria principal"));
	}
	loggear_info(string_from_format("Espacio para la memoria principal reservada"));
}
void inicializar_tabla_segmentos()
{
	tablaDeSegmentos = dictionary_create();
	paginasSegunUso = list_create();
}

void inicializar_sectores_memoria()
{
	tamanioRegistro = sizeof(mseg_t) + sizeof(uint16_t) + tamanioValue + 1; //Porque guardamos el \0
	sem_wait(&mutex_config);
	if((cantidadDeSectores = configuracion.TAMANIO_MEMORIA / tamanioRegistro) == 0){	//Se trunca automaticamente al entero (por ser todos int)
		loggear_error(string_from_format("La Memoria es muy pequenia. No puede almacenar un value tan grande."));
	}
	sem_post(&mutex_config);
	loggear_info(string_from_format("La Memoria Principal quedo dividida en %d paginas", cantidadDeSectores));
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
	loggear_trace(string_from_format("REGISTRO:	Timestamp:	%" PRIu64 "\n	Key:		%u\n	Value:		%s\n", reg->timestamp, reg->key, reg->value));

	int sectorDisponible = get_proximo_sector_disponible();

	if(sectorDisponible == -1)
	{
		if (memoria_esta_full())
		{
			loggear_info(string_from_format("La memoria esta full - Todas las paginas presentes estan modificadas"));
			ejecutar_instruccion_journal(instruccion, 0);
			return NULL;
		}
		else
		{ //Algoritmo de reemplazo:
			loggear_debug(string_from_format("Ejecutando el algoritmo de reemplazo"));
			int *numeroDeSector = pagina_lru();
			int indiceEnTabla = 0;
			//Buscando la fila correspondiente a la pagina menos usada
			char *segmentoConFilaABorrar = NULL;

			fila_correspondiente_a_esa_pagina((*numeroDeSector), &indiceEnTabla, segmentoConFilaABorrar);

			//Modificando esa pagina (Esto tambien podría hacerse al principio del algoritmo, es lo mismo)
			desplazamiento += ((*numeroDeSector) * tamanioRegistro);
			memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
			desplazamiento += sizeof(mseg_t);
			memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
			desplazamiento += sizeof(uint16_t);
			memcpy(memoriaPrincipal + desplazamiento, reg->value, strlen(reg->value)+1);
			*nroPag = (*numeroDeSector);

			free(reg);					  //malloc en obtener_registro_de_instruccion
			return memoriaPrincipal + ((*numeroDeSector) * tamanioRegistro);
		}
	}
	else{
		sectorOcupado[sectorDisponible] = true;
		desplazamiento += (sectorDisponible * tamanioRegistro);
		memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
		desplazamiento += sizeof(mseg_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
		desplazamiento += sizeof(uint16_t);
		memcpy(memoriaPrincipal + desplazamiento, reg->value, strlen(reg->value)+1);
		*nroPag = sectorDisponible;
		loggear_trace(string_from_format("Memcpy realizado en la pagina"));
		free(reg->value);
		free(reg);
		return memoriaPrincipal + (sectorDisponible * tamanioRegistro);
	}

	loggear_error(string_from_format("No se pudo insertar lal pagina en memoria"));
	return NULL;
}

void actualizar_pagina(void *paginaAActualizar, mseg_t nuevoTimestamp, char *nuevoValue)
{
	int desplazamiento = 0;
	memcpy(paginaAActualizar + desplazamiento, &nuevoTimestamp, sizeof(mseg_t));
	desplazamiento += sizeof(mseg_t);
	//	memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t)); La key no se mofidica
	desplazamiento += sizeof(uint16_t);
	loggear_debug(string_from_format("Se esta actualizando la pagina con el value %s", nuevoValue));
	memcpy(paginaAActualizar + desplazamiento, nuevoValue, strlen(nuevoValue)+1);
}

/*
 * Despues de hacer esto se puede borrar la instruccion tranquilamente, hace un malloc nuevo
 */
registro *obtener_registro_de_instruccion(instr_t *instruccion)
{
	char *keyChar = strdup((char *)list_get(instruccion->parametros, 1));
	char *valueNuevo = (char *)list_get(instruccion->parametros, 2);
	printf("EL value nuevo es: %s\n", valueNuevo);
	printf("Y su tamanio sin contar el 0 es: %d\n", strlen(valueNuevo));
	mseg_t timestampNuevo = instruccion->timestamp;
	uint16_t keyNueva;
	str_to_uint16(keyChar, &keyNueva);
	free(keyChar);

	registro *miReg = malloc(sizeof(registro));
	miReg->timestamp= timestampNuevo;
	miReg->key = keyNueva;
	miReg->value = malloc(tamanioValue+1);
	memset(miReg->value, 0, tamanioValue+1);
	memcpy(miReg->value, valueNuevo, strlen(valueNuevo));
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
	uint16_t keyPagina = 0;
	memcpy(&keyPagina, pagina + sizeof(mseg_t), sizeof(uint16_t));
	return keyPagina;
}

char *get_value_pagina(void *pagina)
{
	char* value = calloc(1, tamanioValue+1);
	memcpy(value, pagina + sizeof(mseg_t) + sizeof(uint16_t), tamanioValue + 1);
	return value;
}

registro *obtener_registro_de_pagina(void *pagina)
{
	mseg_t timestamp = get_ts_pagina(pagina);
	uint16_t key = get_key_pagina(pagina);
	registro * miRegistro = calloc(1, sizeof(registro));
	miRegistro->timestamp = timestamp;
	miRegistro->key = key;
	miRegistro->value = calloc(1, tamanioValue + 1);
	memcpy(miRegistro->value, pagina + sizeof(mseg_t) + sizeof(uint16_t), tamanioValue + 1);
	return miRegistro;
}

char *registro_a_str(registro *registro)
{
	char *regString = NULL;
	regString = string_from_format("	Timestamp:	%" PRIu64 "\n	Key:		%u\n	Value:		%s\n", registro->timestamp, registro->key, registro->value);
	return regString;
}

char *pagina_a_str(void *pagina)
{
	registro* registroDePagina = obtener_registro_de_pagina(pagina);
	char* registroStr =  registro_a_str(registroDePagina);
	free(registroDePagina->value);
	free(registroDePagina);
	return registroStr;
}

void loggear_tabla_de_paginas(t_list *tablaDePaginas, void (*funcion_log)(char *texto))
{

	char *texto = string_new();

	void iterator(filaTabPags * fila)
	{
		char *paginaStr = pagina_a_str(fila->ptrPagina);
		string_append_with_format(&texto, " ~~~~~~~~~~~~~~~~~~~~\n"); //No le gusta todo en un solo append
		string_append_with_format(&texto, " Numero de pagina: %d\n", fila->numeroDePagina);
		string_append_with_format(&texto, " Puntero a pagina: %p\n", fila->ptrPagina);
		string_append_with_format(&texto, "  Pagina en memo: \n%s", paginaStr);
		string_append_with_format(&texto, " Flag modificado : %d\n", fila->flagModificado);
		free(paginaStr);
	}

	list_iterate(tablaDePaginas, (void *)iterator);
	funcion_log(texto);
}

void se_uso(int paginaUtilizada)
{

	int *paginaUsada = malloc(sizeof(int));
	*paginaUsada = paginaUtilizada;

	sacar_de_lista_lru(paginaUtilizada);
		//En list[0] queda el que menos se usa

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
	return (int *)list_get(paginasSegunUso, 0);
}

void sacar_de_lista_lru(int pagina){
	bool es_pagina_buscada(int* valor){
		return *valor == pagina;
	}
	list_remove_and_destroy_by_condition(paginasSegunUso, (void*)es_pagina_buscada, free);
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

filaTabPags *fila_con_la_key(t_list *suTablaDePaginas, uint16_t keyBuscada)
{

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

filaTabPags *fila_correspondiente_a_esa_pagina(int numeroDePagina, int *indiceEnTabla, char * segmentoQueLaTiene)
{
	//Un dictionary_find casero

	filaTabPags *filaEncontrada;
	filaTabPags *filaPosible;
	int seEncontro = 0;

	char* tablaABorrar = string_new();
	int suIndice = 0;

	void tiene_esa_pagina_la_fila(char *segmento, t_list *suTablaDePaginas)
	{
		if (seEncontro == 0)
		{
			suIndice = 0;
			filaPosible = fila_con_el_numero(suTablaDePaginas, numeroDePagina, &suIndice);
			if (filaPosible != NULL)
			{ //Se encontro la fila que tenia ese numero
				seEncontro = 1;
				filaEncontrada = filaPosible;
				tablaABorrar=string_duplicate(segmento);
			}
		}
	}

	dictionary_iterator(tablaDeSegmentos, (void *)tiene_esa_pagina_la_fila);


	t_list* tablaQueContiene = dictionary_get(tablaDeSegmentos, tablaABorrar);

	list_remove_and_destroy_element(tablaQueContiene, suIndice, free);

	if (filaPosible == NULL)
	{
		loggear_error(string_from_format("No se encontro la fila con la pagina")); //Durante el algoritmo de reemplazo nunca se debería dar
		segmentoQueLaTiene = NULL;
		return NULL;
	}
	else{
		return filaEncontrada;
	}
}



void ejecutar_instruccion_journal(instr_t *instruccion, int liberar)
{
	loggear_info(string_from_format("Ejecutando instruccion Journal"));
	int conexionConFS = obtener_fd_out("FileSystem");

	void enviar_paginas_modificadas(char *segmento, t_list *suTablaDePaginas)
	{
		char *tablaAInsertar = string_from_format("%s", segmento);

		loggear_trace(string_from_format("Chequeando una tabla de paginas"));
		void enviar_si_esta_modificada(filaTabPags * fila)
		{
			loggear_trace(string_from_format("Chequeando si tiene una fila modificada"));
			if (fila->flagModificado)
			{
				loggear_trace(string_from_format("Se encontro una pagina modificada"));
				cod_op codOp = CODIGO_INSERT;

				if (quien_pidio(instruccion) == CONSOLA_KERNEL)
				{
					loggear_trace(string_from_format("El journal lo pidio Kernel"));
					codOp += BASE_CONSOLA_KERNEL;
				}
				else
				{
					loggear_trace(string_from_format("El journal lo pidio Memoria"));
					codOp += BASE_CONSOLA_MEMORIA;
				}

				loggear_debug(string_from_format("Insertando '%s' en FileSystem", tablaAInsertar));
				instr_t *instruccionAEnviar = fila_a_instr(tablaAInsertar, fila, codOp);
				loggear_trace(string_from_format("Se genero la instruccion a enviar"));
				imprimir_instruccion(instruccionAEnviar, loggear_trace);
				t_list* listaABorrar = list_duplicate(instruccionAEnviar->parametros);
				enviar_request(instruccionAEnviar, conexionConFS);
				liberar_key(listaABorrar);
				liberar_value(listaABorrar);
			}
		}

		list_iterate(suTablaDePaginas, (void *)enviar_si_esta_modificada);
		free(tablaAInsertar);
	}

	dictionary_iterator(tablaDeSegmentos, (void *)enviar_paginas_modificadas);

	limpiar_memoria();

	loggear_trace(string_from_format("Se recorrieron todas las paginas"));
	if(quien_pidio(instruccion)!=CONSOLA_KERNEL){
		t_list *listaParam = list_create();
		char *cadena = string_from_format("Journal realizado.");
		list_add(listaParam, cadena);
		cod_op codOp = CODIGO_EXITO;
		imprimir_donde_corresponda(codOp, instruccion, listaParam);
	}
	if(liberar){
		loggear_trace(string_from_format("Se van a borrar los parametros de la instruccion"));
		list_destroy_and_destroy_elements(instruccion->parametros, free);
		loggear_trace(string_from_format("Se va a liberar instruccion"));
		free(instruccion);
	}
	loggear_trace(string_from_format("Journal finalizado por completo"));
}

void liberar_key(t_list* listaParam){
	int i = 0;

	void borrar_value(char* parametro){

		if(i==1){
			loggear_trace(string_from_format("Se va a borrar el parametro (Value): %s", parametro));
			free(parametro);
			i = -1;
		}
		i++;
	}
	list_iterate(listaParam, (void*)borrar_value);

}
void liberar_value(t_list* listaParam){

	int i = 0;

	void borrar_value(char* parametro){

		if(i==2){
			loggear_trace(string_from_format("Se va a borrar el parametro (Value): %s", parametro));
			free(parametro);
			i = -1;
		}
		i++;
	}
	list_iterate(listaParam, (void*)borrar_value);

	list_destroy(listaParam);
}

instr_t *fila_a_instr(char *tablaAInsertar, filaTabPags *fila, cod_op codOp)
{
	registro *suRegistro = obtener_registro_de_pagina(fila->ptrPagina);
//	loggear_trace(string_from_format("Se tienen los datos de la pagina"));
	return registro_a_instr(tablaAInsertar, suRegistro, codOp);
}

instr_t *registro_a_instr(char *tablaAInsertar, registro *unRegistro, cod_op codOp)
{
//	loggear_trace(string_from_format("Registro: %s\n", registro_a_str(unRegistro)));
	t_list *listaParam = list_create();

	loggear_trace(string_from_format("Lista param creada\n"));
	list_add(listaParam, tablaAInsertar);

	loggear_trace(string_from_format("Nombre de la tabla a insertar agregado\n"));
	char* keyChar = string_from_format("%d", unRegistro->key);
	loggear_trace(string_from_format("Se agrego la key %s", keyChar));
	list_add(listaParam, keyChar);

	char* valueAAgregar = strdup(unRegistro->value);
	list_add(listaParam, valueAAgregar);

	instr_t *instruccionCreada = crear_instruccion(unRegistro->timestamp, codOp, listaParam);
	free(unRegistro->value);
	free(unRegistro);
	return instruccionCreada;
}

void limpiar_memoria()
{
	loggear_info(string_from_format("Limpiando Memoria"));
	limpiar_memoria_principal();
	limpiar_segmentos();
	list_clean_and_destroy_elements(paginasSegunUso, free);
}

void limpiar_memoria_principal()
{
	loggear_debug(string_from_format("Limpiando mem ppal"));
	memset(sectorOcupado, false, cantidadDeSectores * sizeof(bool)); //Ahora indica que todos los sectores de la memoria se pueden sobreescribir
																	 //Supongo que no hace falta que hagamos un memset en 0 a la Memoria Principal en si
}

void limpiar_segmentos()
{
	loggear_debug(string_from_format("Limpiando segmentos"));
	void limpiar_tabla_de_paginas(char *segmento, t_list *suTablaDePaginas)
	{
		list_destroy_and_destroy_elements(suTablaDePaginas,free);
	}

	dictionary_iterator(tablaDeSegmentos, (void *)limpiar_tabla_de_paginas);

	dictionary_clean(tablaDeSegmentos); //No dictionary_clean_and_destroy_elements porque ya los limpio arriba
}

void *ejecutar_journal()
{


	while (1)
	{
		sem_wait(&mutex_config);
		int retardoJournal = configuracion.RETARDO_JOURNAL;
		sem_post(&mutex_config);
		usleep(retardoJournal * 1000);
		instr_t *miInstruccion = leer_a_instruccion("JOURNAL", CONSOLA_MEMORIA);
		sem_wait(&mutex_journal);
		ejecutar_instruccion_journal(miInstruccion, 1);
		sem_post(&mutex_journal);
	}
}

void *ejecutar_gossiping()
{
	while (1)
	{
		ejecutar_instruccion_gossip();
		loggear_debug(string_from_format("Fin gossip programado"));
		sem_wait(&mutex_config);
		int retardoGossiping = configuracion.RETARDO_GOSSIPING;
		sem_post(&mutex_config);
		usleep(retardoGossiping * 1000);
	}
}

void eliminar_tabla(instr_t *instruccion)
{
	char *tablaABorrar = (char *)list_get(instruccion->parametros, 0);
	t_list *segmentoABorrar = segmento_de_esa_tabla(tablaABorrar);
	if (segmentoABorrar != NULL)
	{
		sacar_segmento_de_lru(segmentoABorrar);
		dictionary_remove(tablaDeSegmentos, tablaABorrar);
		list_destroy_and_destroy_elements(segmentoABorrar, free);
	}
	loggear_info(string_from_format("Tabla %s%s borrada de Memoria", puntoMontaje, tablaABorrar));
}

void sacar_segmento_de_lru(t_list* segmentoABorrar){
	void eliminar_su_pagina(filaTabPags* fila){
		sacar_de_lista_lru(fila->numeroDePagina);
	}

	list_iterate(segmentoABorrar, (void*)eliminar_su_pagina);
}

void imprimir_segmento(char *nombreSegmento, t_list *suTablaDePaginas)
{
	loggear_info(string_from_format("Tabla: %s%s\n", puntoMontaje, nombreSegmento));
	loggear_tabla_de_paginas(suTablaDePaginas, loggear_info);
}

void imprimir_segmento_basico(char *nombreSegmento, t_list *suTablaDePaginas)
{
	char *texto = string_new();
	string_append_with_format(&texto, "------------------------\n");
	string_append_with_format(&texto, "Tabla: %s%s\n", puntoMontaje, nombreSegmento);

	void iterator(filaTabPags * fila)
	{
		char *paginaStr = pagina_a_str(fila->ptrPagina);
		string_append(&texto, paginaStr);
		free(paginaStr);
	}
	list_iterate(suTablaDePaginas, (void *)iterator);
	loggear_info(texto);
}

void mostrar_paginas(instr_t *instruccion)
{
	if (list_get(instruccion->parametros, 0) != NULL)
	{
		dictionary_iterator(tablaDeSegmentos, (void *)imprimir_segmento);
	}
	else
	{
		dictionary_iterator(tablaDeSegmentos, (void *)imprimir_segmento_basico);
	}
	list_destroy_and_destroy_elements(instruccion->parametros, free);
	free(instruccion);
}
