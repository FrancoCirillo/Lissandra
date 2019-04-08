#include "tp0.h"

int enviarInstruccion(char* instruccion){
	t_config* config = leer_config();

	int conexion = crear_conexion(
		config_get_string_value(config, "IP"),
		config_get_string_value(config, "PUERTO")
	);

	enviar_mensaje(instruccion, conexion);

	config_destroy(config);
	close(conexion);
	return 0;
}

t_config* leer_config() {
	return config_create("/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/src/FileSystem/fileSystem.config");
}
