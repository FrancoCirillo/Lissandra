//-------fileSystem.c-------

#include "fileSystemMock.h"

int main() {
	printf(COLOR_ANSI_AMARILLO"	PROCESO FILESYSTEM"COLOR_ANSI_RESET"\n");

	callback = ejecutar_instruccion;

	int conexion_con_memoria = conectar_con_proceso(MEMORIA, FILESYSTEM);

	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT);

	vigilar_conexiones_entrantes(listenner, callback, conexion_con_memoria);

	return 0;
}



void ejecutar_instruccion(instr_t* instruccion, int conexionReceptor){

	int mostrarPorConsola = 0;
	if(instruccion->codigo_operacion >= BASE_COD_CONSOLA){
		instruccion->codigo_operacion -= BASE_COD_CONSOLA;
		mostrarPorConsola = 1;
	}
	switch(instruccion->codigo_operacion){
	case CODIGO_SELECT: ejecutar_instruccion_select(instruccion, conexionReceptor, mostrarPorConsola); break;
	case CODIGO_INSERT: ejecutar_instruccion_insert(instruccion, conexionReceptor, mostrarPorConsola); break;
	case CODIGO_CREATE: ejecutar_instruccion_create(instruccion, conexionReceptor, mostrarPorConsola); break;
	case CODIGO_DESCRIBE: ejecutar_instruccion_describe(instruccion, conexionReceptor, mostrarPorConsola); break;
	case CODIGO_DROP: ejecutar_instruccion_drop(instruccion, conexionReceptor, mostrarPorConsola); break;
	default: break;		//evita warnings por no haber cases con los cod_op de errores
	}
}


void ejecutar_instruccion_select(instr_t* instruccion, int conexionReceptor, int flagConsola){
	puts("Ejecutando instruccion Select");
	sleep(1);
	instr_t * miInstruccion = leer_a_instruccion("DEVOLUCION_SELECT TABLA3 4 Hola", 0);
	enviar_request(miInstruccion, conexionReceptor);
}


void ejecutar_instruccion_insert(instr_t* instruccion, int conexionReceptor, int flagConsola){
	puts("Ejecutando instruccion Insert");
	instr_t * miInstruccion = leer_a_instruccion("CODIGO_EXITO SeInserto TABLA1 Inserttt", 0);
	enviar_request(miInstruccion, conexionReceptor);

}


void ejecutar_instruccion_create(instr_t* instruccion, int conexionReceptor, int flagConsola){
	puts("Ejecutando instruccion Create");
	instr_t * miInstruccion = leer_a_instruccion("CODIGO_EXITO TABLA3 4 Hola", 0);
	enviar_request(miInstruccion, conexionReceptor);
}


void ejecutar_instruccion_describe(instr_t* instruccion, int conexionReceptor, int flagConsola){
	puts("Ejecutando instruccion Describe");
	instr_t * miInstruccion = leer_a_instruccion("CODIGO_EXITO MetadataTabla3etc", 0);
		enviar_request(miInstruccion, conexionReceptor);
}


void ejecutar_instruccion_drop(instr_t* instruccion, int conexionReceptor, int flagConsola){
	puts("Ejecutando instruccion Drop");
	instr_t * miInstruccion = leer_a_instruccion("CODIGO_EXITO TABLA3 4 dRRop", 0);
		enviar_request(miInstruccion, conexionReceptor);
}
