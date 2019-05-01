#include "memoria.h"


int main() {

	puts("PROCESO MEMORIA\n");

	//inicializarConfiguracion();

	/*
	puts("----------Esperando request----------\n");
	char* request = readline ("> ");
	instr_t* instr = leer_a_instruccion(request);
	puts("Instruccion creada:");
	print_instruccion(instr);
	*/

	//config_destroy(g_config);

	return 0;
}

void inicializarConfiguracion() {
	char* rutaConfig = "memoria.config";
	configuracion.PUERTO = getByKey(rutaConfig, "PUERTO");
	configuracion.IP_FS = getByKey(rutaConfig, "IP_FS");
	configuracion.PUERTO_FS = getByKey(rutaConfig, "PUERTO_FS");
	configuracion.IP_SEEDS = getByKey(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = getByKey(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = atoi(getByKey(rutaConfig, "RETARDO_MEMORIA"));
	configuracion.RETARDO_FS = atoi(getByKey(rutaConfig, "RETARDO_FS"));
	configuracion.TAMANIO_MEMORIA = atoi(getByKey(rutaConfig, "TAMANIO_MEMORIA"));
	configuracion.RETARDO_JOURNAL = atoi(getByKey(rutaConfig, "RETARDO_JOURNAL"));
	configuracion.RETARDO_GOSSIPING = atoi(getByKey(rutaConfig, "RETARDO_GOSSIPING"));
	configuracion.MEMORY_NUMBER = atoi(getByKey(rutaConfig, "MEMORY_NUMBER"));
	configuracion.RUTA_LOG = getByKey(rutaConfig, "RUTA_LOG");
}

void loggear(char* valor) {
	g_logger = log_create(configuracion.RUTA_LOG, "memoria", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}

char* getByKey(char* ruta, char* key) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, key);

	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------", key, valor);

	return valor;
}










