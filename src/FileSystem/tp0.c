#include "tp0.h"

int enviarInstruccion(char* instruccion, cod_proceso proceso) {

	char* ip_proceso;
//	char* puerto_proceso; TODO

	switch (proceso) {
	case (KERNEL):
		ip_proceso = IP_KERNEL;
		break;
	case(FILESYSTEM):
		ip_proceso = IP_FILESYSTEM;
		break;
	case(MEMORIA):
		ip_proceso = IP_MEMORIA;
		break;
	default:
//TODO	log_warning(logger, "ERROR!");
		puts("EL CODIGO DEL PROCESO NO ES CORRECTO");
		break;
	}

	int conexion = crear_conexion(ip_proceso, PORT);

	enviar_mensaje(instruccion, conexion);

	close(conexion);
	return 0;
	}

	t_config* leer_config() {
		return config_create(
				"/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/src/FileSystem/fileSystem.config");
	}
