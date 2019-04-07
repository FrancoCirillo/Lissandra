#include "tp0.h"

int iniciarCliente(void) {
	t_config* config = leer_config();

	int conexion = crear_conexion(
		config_get_string_value(config, "IP"),
		config_get_string_value(config, "PUERTO")
	);

	t_paquete* paquete = armar_paquete();

	enviar_paquete(paquete, conexion);

	eliminar_paquete(paquete);
	config_destroy(config);
	close(conexion);
	return 0;
}

t_config* leer_config() {
	return config_create("/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/src/Kernel/kernel.config");
}

void leer_consola(t_log* logger) {
	char* leido = readline(">");

	while(strncmp(leido, "", 1) != 0) {
		log_info(logger, leido);
		free(leido);
		leido = readline(">");
	}

	free(leido);
}

t_paquete* armar_paquete() {
	t_paquete* paquete = crear_paquete();

	char* leido = readline(">");

	while(strncmp(leido, "", 1) != 0) {
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline(">");
	}

	free(leido);

	return paquete;
}
