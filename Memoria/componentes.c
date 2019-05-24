//--------componentes.c--------

#include "componentes.h"

void gran_malloc_inicial()
{
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	memoriaPrincipal = malloc(tamanioMemoria);
	if (memoriaPrincipal==NULL) log_error(g_logger, "No se pudo reservar espacio para la memoria principal");
	log_info(g_logger, "Espacio para la memoria principal reservada");
}


void insertar_instruccion(instr_t* instruccion)
{

	int desplazamiento = 0;
	registro *reg = obtener_registro(instruccion);
	int sectorDisponible = get_proximo_sector_disponible();
	if(sectorDisponible == -1) ejecutar_instruccion_journal(instruccion);
	else
	{
		sectorOcupado[sectorDisponible] = true;
		memcpy(memoriaPrincipal + desplazamiento, &reg->timestamp, sizeof(mseg_t));
		desplazamiento += sizeof(mseg_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->key, sizeof(uint16_t));
		desplazamiento += sizeof(uint16_t);
		memcpy(memoriaPrincipal + desplazamiento, &reg->value, tamanioValue);
	}
	free(reg);

}


registro *obtener_registro(instr_t *instruccion)
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

void ejecutar_instruccion_journal(instr_t *instruccion)
{
	puts("Ejecutando instruccion Journal");
	//int conexionFS = obtener_fd_out("FileSystem");
	//enviar_request(instruccion, conexionFS);
}
