//--------componentes.c--------

#include "componentes.h"

void gran_malloc_inicial()
{
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	memoriaPrincipal = malloc(tamanioMemoria);
	if (memoriaPrincipal==NULL) log_error(g_logger, "No se pudo reservar espacio para la memoria principal");
	log_info(g_logger, "Espacio para la memoria principal reservada");

}
void inicializar_tabla_segmentos(){
	tablaDeSegmentos = dictionary_create();
	paginasSegunUso = list_create();
}

t_list *nueva_tabla_de_paginas(){
	t_list *tablaDePaginas = list_create();

	return tablaDePaginas;

}

void agregar_fila_tabla(t_list * tablaDePaginas, int numPag, void* pagina, bool flagMod){
	filaTabPags *tabla = malloc(sizeof(filaTabPags));

	tabla->numeroDePagina = numPag;
	tabla->ptrPagina = pagina;
	tabla->flagModificado = flagMod;
	list_add(tablaDePaginas, tabla);

}


void *insertar_instruccion_en_memoria(instr_t* instruccion, int* nroPag)
{

	int desplazamiento = 0;
	registro *reg = obtener_registro_de_instruccion(instruccion);
	int sectorDisponible = get_proximo_sector_disponible();
	if(sectorDisponible == -1){
		/*if(memoria__esta_full()){
			ejecutar_jourmal()
		}
		  else ejecutar_algoritmo_reemplazo():

		  pagina_lru();
		  fila_correspondiente_a_esa_pagina()
		  //guardar el numero de pagina de esa fila
		  guardar el ptr a la pagina

		  memcpy(ptrALaPagina+ desplazamiento, &reg->timestamp, sizeof(mseg_t));
		  desplazamiento += sizeof(mseg_t);
		  memcpy(ptrALaPagina + desplazamiento, &reg->key, sizeof(uint16_t));
		  desplazamiento += sizeof(uint16_t);
		  memcpy(ptrALaPagina+ desplazamiento, &reg->value, tamanioValue);
		  return ptrALaPagina;

		  list_remove(fila)

		*/
		ejecutar_instruccion_journal(instruccion);
		memset(sectorOcupado, false, cantidadDeSectores * sizeof(bool)); //Por ahora se hace "Journal" de toda la memoria (y queda vacia)
		sectorOcupado[sectorDisponible] = true;
		memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
		desplazamiento += sizeof(mseg_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
		desplazamiento += sizeof(uint16_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->value, tamanioValue);
		return memoriaPrincipal;
	}
	else
	{
		sectorOcupado[sectorDisponible] = true;
		desplazamiento += (sectorDisponible*tamanioRegistro);
		memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
		desplazamiento += sizeof(mseg_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
		desplazamiento += sizeof(uint16_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->value, tamanioValue);
		*nroPag = sectorDisponible;
		return memoriaPrincipal + (sectorDisponible*tamanioRegistro);
	}
	free(reg);

}

void actualizar_pagina(void* paginaAActualizar, mseg_t nuevoTimestamp, char* nuevoValue){

	int desplazamiento = 0;
	memcpy(paginaAActualizar + desplazamiento, &nuevoTimestamp, sizeof(mseg_t));
	desplazamiento += sizeof(mseg_t);
//	memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t)); La key no se mofidica
	desplazamiento += sizeof(uint16_t);
	memcpy(memoriaPrincipal + desplazamiento, &nuevoValue, tamanioValue);
}
registro *obtener_registro_de_instruccion(instr_t *instruccion)
{
	char*  keyChar = (char *)list_get(instruccion->parametros, 1);
	char* valueNuevo = (char *)list_get(instruccion->parametros, 2);
	mseg_t timestampNuevo = instruccion->timestamp;
	uint16_t keyNueva;
	str_to_uint16(keyChar, &keyNueva);

	registro registroCreado ={
			.timestamp= timestampNuevo,
			.key = keyNueva,
			.value = valueNuevo
	};

	registro* miReg = malloc(sizeof(registroCreado));
	memcpy(miReg, &registroCreado, sizeof(registroCreado));

	return miReg;
}

int get_proximo_sector_disponible()
{
	for(int sector = 0; sector < cantidadDeSectores; sector++)
	{
		if(sectorOcupado[sector] == false)
		{
			return sector;
		}
	}
	return -1;
}

mseg_t get_ts_pagina(void*pagina){
	mseg_t timestampPagina;
	memcpy(&timestampPagina, pagina, sizeof(mseg_t));
	return timestampPagina;
}

uint16_t get_key_pagina(void*pagina){
	uint16_t keyPagina;
	memcpy(&keyPagina, pagina + sizeof(mseg_t), sizeof(uint16_t));
	return keyPagina;
}

char* get_value_pagina(void*pagina){
	char* keyPagina = malloc(tamanioValue);
	memcpy(&keyPagina, pagina + sizeof(mseg_t) + sizeof(uint16_t), sizeof(tamanioValue));
	return keyPagina;
}

registro *obtener_registro_de_pagina(void*pagina){
	mseg_t timestamp = get_ts_pagina(pagina);
	uint16_t key = get_key_pagina(pagina);
	char* value = get_value_pagina(pagina);

	registro *miRegistro = malloc(tamanioRegistro +1);
	miRegistro->timestamp = timestamp;
	miRegistro->key = key;
	miRegistro->value = value;

	return miRegistro;
}

char* registro_a_str(registro* registro){
	char *regString = malloc(450);
	sprintf(regString, "	Timestamp:	%lu\n	Key:		%u\n	Value:		%s\n", registro->timestamp, registro->key, registro->value);
	return regString;
}

char* pagina_a_str(void* pagina){
	return registro_a_str(obtener_registro_de_pagina(pagina));
}


void imprimir_tabla_de_paginas(t_list *tablaDePaginas){

	void iterator(filaTabPags *fila){
		printf(" ~~~~~~~~~~~~~~~~~~~~\n");
		printf(" Numero de pagina: %d\n", fila->numeroDePagina);
		printf(" Puntero a pagina: %p\n", fila->ptrPagina);
		printf("  Registro en memo: \n%s",pagina_a_str(fila->ptrPagina));
		printf(" Flag modificado : %d\n", fila->flagModificado);
	}

	list_iterate(tablaDePaginas, (void *)iterator);
}


void se_utilizo(t_list *suTablaDePaginas, filaTabPags* filaUsada){
	int* paginaUsada = malloc(sizeof(int));
	*paginaUsada = filaUsada->numeroDePagina;
	list_add(paginasSegunUso, paginaUsada); //Lo agrega en el último lugar, si ya existia se duplica
	bool esPaginaRequerida(int* numeroDePagina){
			if(*paginaUsada == *numeroDePagina){
				return true;
			}
			else return false;
	}

	if((list_find(paginasSegunUso, (void*) esPaginaRequerida)) != NULL){
		list_remove_and_destroy_by_condition(paginasSegunUso, (void*) esPaginaRequerida, (void*)free);
		//En list[0] queda el que menos se usa
	}


}

filaTabPags* en_que_fila_esta_key(t_list *suTablaDePaginas, char* keyChar){
	uint16_t keyPedida;
	str_to_uint16(keyChar, &keyPedida);
	uint16_t keyEnMemoria;
	for(int i = 0; i < list_size(suTablaDePaginas); i++){
			filaTabPags *fila = list_get(suTablaDePaginas, i);
			keyEnMemoria = get_key_pagina(fila->ptrPagina);
			if(keyEnMemoria == keyPedida){

				return fila;
			}
		}
		return NULL;
}

t_list * segmento_de_esa_tabla(char* tabla)
{
	return dictionary_get(tablaDeSegmentos, tabla);
}
