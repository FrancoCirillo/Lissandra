#include "memoria.h"

int main() {
//	pruebaHilos();

	printf("PROCESO MEMORIA\n");

	//inicializarConfiguracion();

	printf("\n\nComo cliente: \n");

	int conexion_FileSystem = conectar_con_proceso(FILESYSTEM); //connect()
	puts("Listo para enviar a FILESYSTEM");
//	int conexion_Kernel = conectar_con_proceso(KERNEL);
//	puts("Listo para enviar a KERNEL");
//	enviar_mensaje("123456;parametro1;parametro2;parametro3;parametro4;", conexion_FileSystem);
//	enviar_mensaje("123456;HOLA;SI;QUE;TUL;", conexion_Kernel);
	sleep(2);
	close(conexion_FileSystem);

	//config_destroy(g_config);

	return 0;
}

void inicializarConfiguracion() {
	char* rutaConfig = "memoria.config";
	configuracion.PUERTO = atoi(getByKey(rutaConfig, "PUERTO"));
	configuracion.IP_FS = atoi(getByKey(rutaConfig, "IP_FS"));
	configuracion.PUERTO_FS = atoi(getByKey(rutaConfig, "PUERTO_FS"));
	configuracion.IP_SEEDS = getByKey(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = getByKey(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = getByKey(rutaConfig, "RETARDO_MEMORIA");
	configuracion.RETARDO_FS = getByKey(rutaConfig, "RETARDO_FS");
	configuracion.TAMANIO_MEMORIA = getByKey(rutaConfig, "TAMANIO_MEMORIA");
	configuracion.RETARDO_JOURNAL = getByKey(rutaConfig, "RETARDO_JOURNAL");
	configuracion.RETARDO_GOSSIPING = getByKey(rutaConfig, "RETARDO_GOSSIPING");
	configuracion.MEMORY_NUMBER = getByKey(rutaConfig, "MEMORY_NUMBER");
	configuracion.RUTA_LOG = getByKey(rutaConfig, "RUTA_LOG");
}

void loggear(char *valor) {
	g_logger = log_create(configuracion.RUTA_LOG, "memoria", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}

char* getByKey(char* ruta, char* key) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, key);
	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------", key, valor);

	//config_destroy(g_config);
	return valor;
}
