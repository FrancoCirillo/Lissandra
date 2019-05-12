//---------memoria.c---------

#include "memoria.h"


int main() {

	printf("\x1b[1;36m	PROCESO MEMORIA \x1b[0m \n");

	callback = ejecutar_instruccion;

	inicializar_configuracion();

	int listenner = iniciar_servidor(IP_MEMORIA, PORT);

	vigilar_conexiones_entrantes(listenner, callback);


	//config_destroy(g_config);

	return 0;
}


void inicializar_configuracion() {
	puts("Configuracion:");
	char* rutaConfig = "memoria.config";
	configuracion.PUERTO = obtener_por_clave(rutaConfig, "PUERTO");
	configuracion.IP_FS = obtener_por_clave(rutaConfig, "IP_FS");
	configuracion.PUERTO_FS = obtener_por_clave(rutaConfig, "PUERTO_FS");
	configuracion.IP_SEEDS = obtener_por_clave(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = obtener_por_clave(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "RETARDO_MEMORIA"));
	configuracion.RETARDO_FS = atoi(obtener_por_clave(rutaConfig, "RETARDO_FS"));
	configuracion.TAMANIO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "TAMANIO_MEMORIA"));
	configuracion.RETARDO_JOURNAL = atoi(obtener_por_clave(rutaConfig, "RETARDO_JOURNAL"));
	configuracion.RETARDO_GOSSIPING = atoi(obtener_por_clave(rutaConfig, "RETARDO_GOSSIPING"));
	configuracion.MEMORY_NUMBER = atoi(obtener_por_clave(rutaConfig, "MEMORY_NUMBER"));
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}


void loggear(char* valor) {
	g_logger = log_create(configuracion.RUTA_LOG, "memoria", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}


char* obtener_por_clave(char* ruta, char* clave) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}


void ejecutar_instruccion(instr_t* instruccion){
	switch(instruccion->codigo_operacion){
	case CODIGO_SELECT: ejecutar_instruccion_select(instruccion); break;
	case CODIGO_INSERT: ejecutar_instruccion_insert(instruccion); break;
	case CODIGO_CREATE: ejecutar_instruccion_create(instruccion); break;
	case CODIGO_DESCRIBE: ejecutar_instruccion_describe(instruccion); break;
	case CODIGO_DROP: ejecutar_instruccion_drop(instruccion); break;
	case CODIGO_JOURNAL: ejecutar_instruccion_journal(instruccion); break;
	default: break;		//evita warnings por no haber cases con los cod_op de errores
	}
}


void ejecutar_instruccion_select(instr_t* instruccion){
	puts("Ejecutando instruccion Select");
}


void ejecutar_instruccion_insert(instr_t* instruccion){
	puts("Ejecutando instruccion Insert");
}


void ejecutar_instruccion_create(instr_t* instruccion){
	puts("Ejecutando instruccion Create");
}


void ejecutar_instruccion_describe(instr_t* instruccion){
	puts("Ejecutando instruccion Describe");
}


void ejecutar_instruccion_drop(instr_t* instruccion){
	puts("Ejecutando instruccion Drop");
}


void ejecutar_instruccion_journal(instr_t* instruccion){
	puts("Ejecutando instruccion Journal");
}










