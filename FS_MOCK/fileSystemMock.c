//-------fileSystem.c-------

#include "fileSystemMock.h"

int main() {
	printf(COLOR_ANSI_AMARILLO"	PROCESO FILESYSTEM"COLOR_ANSI_RESET"\n");

	callback = ejecutar_instruccion;

	int conexion_con_memoria = conectar_con_proceso(MEMORIA, FILESYSTEM);

	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT);

	vigilar_conexiones_entrantes(listenner, callback, conexion_con_memoria, CONSOLA_FS);

	return 0;
}



void ejecutar_instruccion(instr_t* instruccion, int conexionReceptor){

	switch(instruccion->codigo_operacion){
	case CONSOLA_FS_SELECT:
	case CONSOLA_MEM_SELECT:
	case CONSOLA_KRN_SELECT: ejecutar_instruccion_select(instruccion, conexionReceptor); break;
	case CONSOLA_FS_INSERT:
	case CONSOLA_MEM_INSERT:
	case CONSOLA_KRN_INSERT: ejecutar_instruccion_insert(instruccion, conexionReceptor); break;
	case CONSOLA_FS_CREATE:
	case CONSOLA_MEM_CREATE:
	case CONSOLA_KRN_CREATE: ejecutar_instruccion_create(instruccion, conexionReceptor); break;
	case CONSOLA_FS_DESCRIBE:
	case CONSOLA_MEM_DESCRIBE:
	case CONSOLA_KRN_DESCRIBE: ejecutar_instruccion_describe(instruccion, conexionReceptor); break;
	case CONSOLA_FS_DROP:
	case CONSOLA_MEM_DROP:
	case CONSOLA_KRN_DROP: ejecutar_instruccion_drop(instruccion, conexionReceptor); break;
	default: break;
	}
}


void ejecutar_instruccion_select(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Select");
	sleep(1);
	int tablaPreexistente = 1;
	if(tablaPreexistente){
		t_list * listaParam = list_create();
		list_add(listaParam, (char*) list_get(instruccion->parametros, 0)); //Tabla
		list_add(listaParam, (char*) list_get(instruccion->parametros, 1)); //Key
		list_add(listaParam, "Buenas soy un value"); //Value
		enviar_a_quien_corresponda(DEVOLUCION_SELECT, instruccion,listaParam, conexionReceptor);
	}
	else{
		t_list * listaParam = list_create();
		char cadena [400];
		sprintf(cadena, "%s%s", (char*) list_get(instruccion->parametros, 0), "No existe");
		list_add(listaParam, cadena);
		enviar_a_quien_corresponda(ERROR_SELECT, instruccion,listaParam, conexionReceptor);
	}
}


void ejecutar_instruccion_insert(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Insert");
	sleep(1);
	t_list * listaParam = list_create();

	list_add(listaParam, "Se inserto TABLA1 | 3 | Joya | 150515789");

	enviar_a_quien_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionReceptor);

}


void ejecutar_instruccion_create(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Create");

	t_list * listaParam = list_create();

	list_add(listaParam, "Se creo TABLA2");

	enviar_a_quien_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionReceptor);

}


void ejecutar_instruccion_describe(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Describe");
	t_list * listaParam = list_create();

	list_add(listaParam, "Metadata Tabla3:	Tipo de consistencia: SC	Numero de Particiones: 4	Tiempo entre compactaciones: 60000");

	enviar_a_quien_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionReceptor);

}


void ejecutar_instruccion_drop(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Drop");
	t_list * listaParam = list_create();

	list_add(listaParam, "Se borro la Tabla 3");

	enviar_a_quien_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionReceptor);


}

void enviar_a_quien_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam, int conexionReceptor){
	instr_t * miInstruccion;
	switch(quienEnvio(instruccion)){
	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		enviar_request(miInstruccion, conexionReceptor);
		break;
	case CONSOLA_MEMORIA:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_MEMORIA, listaParam);
		enviar_request(miInstruccion, conexionReceptor);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		if(codigoOperacion == DEVOLUCION_SELECT){
			imprimir_registro(miInstruccion);
		}
		break;
	}

}
