//--------componentes.c--------

#include "componentes.h"

void gran_malloc_inicial()
{
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	memoriaPrincipal = malloc(tamanioMemoria);
	if (memoriaPrincipal == NULL)
	{
		log_error(g_logger, "No se pudo reservar espacio para la memoria principal");
	}
	log_info(g_logger, "Espacio para la memoria principal reservada");
}
void inicializar_tabla_segmentos()
{
	tablaDeSegmentos = dictionary_create();
	paginasSegunUso = list_create();
}

t_list *nueva_tabla_de_paginas()
{
	t_list *tablaDePaginas = list_create();

	return tablaDePaginas;
}

filaTabPags *agregar_fila_tabla(t_list *tablaDePaginas, int numPag, void *pagina, bool flagMod)
{
	filaTabPags *tabla = malloc(sizeof(filaTabPags));

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
			ejecutar_journal();
			log_error(g_logger, "El journal todavia no esta implementado.");
		}
		else
		{ //Algoritmo de reemplazo:
			puts("Ejecutando el algoritmo de reemplazo");
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

	log_error(g_logger, "Error al insertrar la pagina en Memoria");
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
	char *regString = malloc(450);
	sprintf(regString, "	Timestamp:	%"PRIu64"\n	Key:		%u\n	Value:		%s\n", registro->timestamp, registro->key, registro->value);
	return regString;
}

char *pagina_a_str(void *pagina)
{
	return registro_a_str(obtener_registro_de_pagina(pagina));
}

void imprimir_tabla_de_paginas(t_list *tablaDePaginas)
{

	void iterator(filaTabPags * fila)
	{
		printf(" ~~~~~~~~~~~~~~~~~~~~\n");
		printf(" Numero de pagina: %d\n", fila->numeroDePagina);
		printf(" Puntero a pagina: %p\n", fila->ptrPagina);
		printf("  Registro en memo: \n%s", pagina_a_str(fila->ptrPagina));
		printf(" Flag modificado : %d\n", fila->flagModificado);
	}

	list_iterate(tablaDePaginas, (void *)iterator);
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

	printf("Lista de paginas segun su uso:[");
	void mostrarPaginasSegunUso(int* numDePag){
		printf("%d,", *numDePag);
	}
	list_iterate(paginasSegunUso, (void*) mostrarPaginasSegunUso);
	printf("]\n");
}

t_list *segmento_de_esa_tabla(char *tabla)
{
	return dictionary_get(tablaDeSegmentos, tabla);
}

bool tabla_de_paginas_full(t_list *tablaDePaginas)
{
	bool esta_modificado(filaTabPags * fila)
	{
		return fila->flagModificado == true;
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

void ejecutar_journal()
{
	puts("Journal en proceso");
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
		log_error(g_logger, "No se encontro la fila con la pagina"); //Durante el algoritmo de reemplazo nunca se debería dar
		*segmentoQueLaTiene = NULL;
		return NULL;
	}
	else
		return filaEncontrada;
}
